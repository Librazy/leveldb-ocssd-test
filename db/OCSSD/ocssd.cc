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
enum PageAllocPolicy{
	kRoundRobin_Fixed = 0x00,
	kRoundRobin_Flexible = 0x01
};

const ChunkingAlgor kChunkingAlgor = kRaid0;
const GCPolicy kGCPolicy = kOneLunSerial;
const PageAllocPolicy kPageAllocPolicy = kRoundRobin_Fixed;
};




class oc_block_manager { //alloc in a granularity of <Block>
public:
	typedef uint32_t LunAndPlane_t;
	//REQUIRE: Lun and Pl's type should be 32-length
	#define MakeLunAndPlane(Lun, Pl) ({ \
			uint32_t l = Lun;       \
			uint32_t p = Pl;        \
			uint32_t lap = (l << 16) | p; \
			lap; \
			})

	#define GetLun(lap) ({ uint32_t mask = ((uint32_t)1 << 16) - 1; \
			(lap & (mask << 16)) >> 16; \
			})

	#define GetPlane(lap) ({ uint32_t mask = ((uint32_t)1 << 16) - 1; \
			lap & mask; \
			})


	struct AllocBlkDes{
	};

	struct Options{

	};
	/*
	 * @approximate_size - how many bytes to alloc(approximately)
	 */
	struct AllocBlkDes AllocBlocks(size_t approximate_size)
	{
		return 0;
	}

private:
	/*  Problems to be issued:
	 *  1. a file can be discontinuous in lun : (a file's size is changable at any time: support Append as a FS?)
	 *  2. chunk size is flexible to each incoming request or fixed by a definition before ?
	 * 
	 */

	struct pap_rr_usage_meta{
		int next_lun;							
		struct nvm_addr next_page;	
	};
	friend class ocssd;

	oc_block_manager(ocssd *ssd, PageAllocPolicy pap) 
	: ssd_(ssd), geo_(nvm_dev_get_geo(ssd_->dev_)), pap_(pap)
	{
	}

	static oc_block_manager* New_oc_block_manager()
	{
	}
	




	ocssd * const ssd_;
	const struct nvm_geo * const geo_;

	struct Options opt_;
	leveldb::Status s;
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
