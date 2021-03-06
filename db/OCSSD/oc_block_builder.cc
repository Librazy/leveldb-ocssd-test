
/*
 * BlockBuilder for ocssd port. slightly modified from leveldb's original BlockBuilder.
 */

// BlockBuilder generates blocks where keys are prefix-compressed:
//
// When we store a key, we drop the prefix shared with the previous
// string.  This helps reduce the space requirement significantly.
// Furthermore, once every K keys, we do not apply the prefix
// compression and store the entire key.  We call this a "restart
// point".  The tail end of the block stores the offsets of all of the
// restart points, and can be used to do a binary search when looking
// for a particular key.  Values are stored as-is (without compression)
// immediately following the corresponding key.
//
// An entry for a particular key-value pair has the form:
//     shared_bytes: varint32
//     unshared_bytes: varint32
//     value_length: varint32
//     key_delta: char[unshared_bytes]
//     value: char[value_length]
// shared_bytes == 0 for restart points.
//
// The trailer of the block has the form:
//     restarts: uint32[num_restarts]
//     num_restarts: uint32
// restarts[i] contains the offset within the block of the ith restart point.

#include "oc_block_builder.h"

#include <algorithm>
#include <assert.h>
#include "leveldb/comparator.h"
#include "leveldb/table_builder.h"
#include "util/coding.h"

namespace leveldb {

namespace ocssd {

static void OBPutFixed32(oc_buffer *dst, uint32_t value)
{
	char buf[sizeof(value)];
	leveldb::EncodeFixed32(buf, value);
	dst->append(buf, sizeof(buf));
}

static void OCPutVarint32(oc_buffer *dst, uint32_t v)
{
	char buf[5];
	char *ptr = leveldb::EncodeVarint32(buf, v);
	dst->append(buf, ptr - buf);
}

BlockBuilder::BlockBuilder(const leveldb::Options *options, oc_page_pool *p)
	: options_(options),
	  buffer_(new ocssd::oc_buffer(p)),
	  restarts_(),
	  counter_(0),
	  finished_(false)
{
	assert(options->block_restart_interval >= 1);
	restarts_.push_back(0);       // First restart point is at offset 0
}

BlockBuilder::~BlockBuilder()
{
	delete buffer_;
}
void BlockBuilder::Reset()
{
	buffer_->clear();
	restarts_.clear();
	restarts_.push_back(0);       // First restart point is at offset 0
	counter_ = 0;
	finished_ = false;
	last_key_.clear();
}

size_t BlockBuilder::CurrentSizeEstimate() const
{
	return (buffer_->size() +                        // Raw data buffer
		restarts_.size() * sizeof(uint32_t) +   // Restart array
		sizeof(uint32_t));                      // Restart array length
}

ocssd::oc_buffer* BlockBuilder::Finish()
{
	// Append restart array
	for (size_t i = 0; i < restarts_.size(); i++) {
		OBPutFixed32(buffer_, restarts_[i]);
	}
	OBPutFixed32(buffer_, restarts_.size());
	finished_ = true;
	return buffer_;
}

void BlockBuilder::Add(const Slice& key, const Slice& value)
{
	Slice last_key_piece(last_key_);
	assert(!finished_);
	assert(counter_ <= options_->block_restart_interval);
	assert(buffer_->empty() // No values yet?
		|| options_->comparator->Compare(key, last_key_piece) > 0);
	size_t shared = 0;
	if (counter_ < options_->block_restart_interval) {
		// See how much sharing to do with previous string
		const size_t min_length = std::min(last_key_piece.size(), key.size());
		while ((shared < min_length) && (last_key_piece[shared] == key[shared])) {
			shared++;
		}
	} else {
		// Restart compression
		restarts_.push_back(buffer_->size());
		counter_ = 0;
	}
	const size_t non_shared = key.size() - shared;

	// Add "<shared><non_shared><value_size>" to buffer_
	OCPutVarint32(buffer_, shared);
	OCPutVarint32(buffer_, non_shared);
	OCPutVarint32(buffer_, value.size());

	// Add string delta to buffer_ followed by value
	buffer_->append(key.data() + shared, non_shared);
	buffer_->append(value.data(), value.size());

	// Update state
	last_key_.resize(shared);
	last_key_.append(key.data() + shared, non_shared);
	assert(Slice(last_key_) == key);
	counter_++;
}

} //namespace ocssd
} //namespace leveldb
