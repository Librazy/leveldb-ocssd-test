#include "ocssd.h"

#include "util/coding.h"
#include "util/mutexlock.h"

namespace leveldb {
namespace ocssd {

namespace oc_options {
const char *kDevPath = "/dev/nvme0n1";
const char *kOCSSDMetaFileNameSuffix = ".ocssd";
enum ChunkingAlgor {
	kRaid0 = 0x00,   //No parity ,so Reconstruction_Read is not supported.
	kRaid5 = 0x01    //with parity spread around the stripe.
};
enum GCPolicy {
	kOneLunSerial = 0x00,    //Erase Operation is emited to one LUN by serializtion
	kAllIn = 0x01
};
enum AddrAllocPolicy {
	kRoundRobin_Fixed = 0x00,
	kRoundRobin_Flexible = 0x01
};

const ChunkingAlgor kChunkingAlgor = kRaid0;
const GCPolicy kGCPolicy = kOneLunSerial;
const AddrAllocPolicy kAddrAllocPolicy = kRoundRobin_Fixed;
};



ocssd::ocssd() : des_(new ocssd_descriptor(oc_options::kDevPath)), dev_(NULL)
{
	Setup();
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
