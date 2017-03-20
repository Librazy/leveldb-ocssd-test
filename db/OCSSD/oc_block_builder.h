#ifndef YWJ_OCSSD_OC_BLOCK_BUILDER_H
#define YWJ_OCSSD_OC_BLOCK_BUILDER_H

#include <vector>

#include <stdint.h>
#include "leveldb/slice.h"
#include "leveldb/options.h"
#include "oc_page_cache.h"


namespace leveldb {
namespace ocssd{

/*
 * BlockBuilder for ocssd port. slightly modified from leveldb's original BlockBuilder.
 */

class BlockBuilder {
 public:
  explicit BlockBuilder(const leveldb::Options* options);

  // Reset the contents as if the BlockBuilder was just constructed.
  void Reset();

  // REQUIRES: Finish() has not been called since the last call to Reset().
  // REQUIRES: key is larger than any previously added key
  void Add(const Slice& key, const Slice& value);

  // Finish building the block and return a slice that refers to the
  // block contents.  The returned slice will remain valid for the
  // lifetime of this builder or until Reset() is called.
  ocssd::oc_buffer* Finish();

  // Returns an estimate of the current (uncompressed) size of the block
  // we are building.
  size_t CurrentSizeEstimate() const;

  // Return true iff no entries have been added since the last Reset()
  bool empty() const {
    return buffer_->empty();
  }

 private:
  const Options*        options_;
  ocssd::oc_buffer*     buffer_;      // Destination buffer(Use oc_page_cache)
  std::vector<uint32_t> restarts_;    // Restart points
  int                   counter_;     // Number of entries emitted since restart
  bool                  finished_;    // Has Finish() been called?
  std::string           last_key_;

  // No copying allowed
  BlockBuilder(const BlockBuilder&);
  void operator=(const BlockBuilder&);
};

}//namespace ocssd

}//namespace leveldb

#endif  // STORAGE_LEVELDB_TABLE_BLOCK_BUILDER_H_
