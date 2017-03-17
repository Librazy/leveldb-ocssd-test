//OCSSD-Optimization Modules headers
#include "oc_ssd.h"
#include "oc_block_manager.h"


#include "util/coding.h"
#include "util/mutexlock.h"

namespace leveldb {
namespace ocssd {

static void TEST_Pr_pmode(int pmode)
{
	const char *str;
	switch (pmode) {
	case NVM_FLAG_PMODE_SNGL:
		str = "NVM_FLAG_PMODE_SNGL";break;
	case NVM_FLAG_PMODE_DUAL:
		str = "NVM_FLAG_PMODE_DUAL";break;
	case NVM_FLAG_PMODE_QUAD:
		str = "NVM_FLAG_PMODE_QUAD";break;
	default:
		str = "ERRORPMODE";break;
	}
	printf("%s\n", str);
}

static bool pmode_is_good(int pmode)
{
	switch (pmode) {
		case NVM_FLAG_PMODE_SNGL:
		case NVM_FLAG_PMODE_DUAL:
		case NVM_FLAG_PMODE_QUAD:
			return true;
		default:
			return false;
	}
}


oc_ssd::oc_ssd() : des_(new oc_ssd_descriptor(oc_options::kDevPath)), dev_(NULL)
{
	Setup();
	if (s.ok()) {
		s = oc_block_manager::New_oc_block_manager(this, &blkmng_);
	}
}
oc_ssd::~oc_ssd()
{
	Cleanup();
}

void oc_ssd::Setup()
{
	//Open device
	dev_ = nvm_dev_open(des_->dev_path_.c_str());
	if (!dev_) {
		s = Status::IOError("OCSSD Setup", strerror(errno));
		return;
	}

	//plane access mode
	pmode_ = nvm_dev_get_pmode(dev_);
	if(!pmode_is_good(pmode_)){	///To be clean - is not necessary ?
		s = Status::IOError("OCSSD get pmode error");
	}

}
void oc_ssd::Cleanup()
{
	nvm_dev_close(dev_);
	delete des_;
}


void oc_ssd::EncodeTo(struct oc_ssd_descriptor *ocdes, char *buf)
{
	std::string str;
	PutLengthPrefixedSlice(&str, ocdes->dev_path_);
	size_t num = ocdes->files_.size();
}
void oc_ssd::DecodeFrom(struct oc_ssd_descriptor *ocdes, char *buf)
{

}


oc_file* oc_ssd::TEST_New_file(const char *fname)
{
	oc_file *ptr;
	leveldb::Status s = oc_file::New_oc_file(this, fname, &ptr);
	return ptr;
}


} //namespace ocssd
} //namespace leveldb
