//OCSSD-Optimization Modules headers
#include "ocssd.h"
#include "oc_block_manager.h"


#include "util/coding.h"
#include "util/mutexlock.h"

namespace leveldb {
namespace ocssd {




ocssd::ocssd() : des_(new ocssd_descriptor(oc_options::kDevPath)), dev_(NULL)
{
	Setup();
	if (s.ok()) {
		s = oc_block_manager::New_oc_block_manager(this, &blkmng_);
	}
}
ocssd::~ocssd()
{
	Cleanup();
}

void ocssd::Setup()
{
	dev_ = nvm_dev_open(des_->dev_path_.c_str());
	if (!dev_) {
		s = Status::IOError("OCSSD Setup", strerror(errno));
	}
}
void ocssd::Cleanup()
{
	nvm_dev_close(dev_);
	delete des_;
}

leveldb::Status NewOCFile(oc_file **ret)
{
}

void ocssd::EncodeTo(struct ocssd_descriptor *ocdes, char *buf)
{
	std::string str;
	PutLengthPrefixedSlice(&str, ocdes->dev_path_);
	size_t num = ocdes->files_.size();
}
void ocssd::DecodeFrom(struct ocssd_descriptor *ocdes, char *buf)
{

}


} //namespace ocssd
} //namespace leveldb
