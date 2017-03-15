#ifndef YWJ_OCSSD_OC_BLK_MNG_H
#define YWJ_OCSSD_OC_BLK_MNG_H

#include "ocssd.h"
#include "oc_options.h"
#include "oc_gc.h"

//mutex & condition varieble headers
#include "port/port.h"


//liblightnvm headers
#include "liblightnvm.h"
#include "nvm.h"

#include <cstddef>
#include <cstdint>



namespace leveldb {
namespace ocssd {

class ocssd;
class oc_GC;

class oc_block_manager { //allocation is done in a granularity of <Block>
public:


	typedef uint32_t LunAndPlane_t;
	typedef uint8_t BlkState_t;

	struct StripeDes {	//Stripe Descriptor: an parellel unit, consist of blocks
		LunAndPlane_t st;
		LunAndPlane_t ed;
		int blk_num_st;
	};

	//REQUIRE - bytes should be multiple of <chunk_size>.
	leveldb::Status AllocStripe(size_t bytes, StripeDes *sd);
	leveldb::Status FreeStripe(StripeDes *sd);
	leveldb::Status FreeStripeArray(StripeDes *sds, int num);


	bool ok();

	//TESTS
	void TEST_Pr_Opt_Meta();
	leveldb::Status TEST_Pr_BBT();
	static void TEST_My_nvm_bbt_pr(int lun, const struct nvm_bbt *bbt);
	void TEST_Lap();
	void TEST_Add();


	
private:
	/*  Problems to be issued:
	 *  1. a file can be discontinuous in lun : (a file's size is changable at any time: support Append as a FS?)
	 *  2. chunk size is flexible to each incoming request or fixed by a definition before ?
	 * 
	 */
	struct Options {
		size_t chunk_size;          				//default: a block's size in bytes.
		oc_options::AddrAllocPolicy policy;			//default: kRoundRobin_Fixed(an allocation will alloc the same chunks for each plane.)
		bool bbt_cached;            				//default: enabled
	};
	struct rr_usage_meta {
		LunAndPlane_t lap;
		struct nvm_addr block;
		rr_usage_meta() : lap(0)
		{
			block.ppa = 0;
		}
	};
	

	friend class ocssd;
	friend class oc_GC;

	void def_ocblk_opt(struct Options *opt);

	void InitClean();
	void InitBBTs();
	void FlushBBTs();
	void Init();

	void _add_blks(size_t blks);

	oc_block_manager(ocssd *ssd);

	/*
	 * 
	 */
	static leveldb::Status New_oc_block_manager(ocssd *ssd,  oc_block_manager **oc_blk_mng_ptr);

	ocssd *const ssd_;
	const struct nvm_geo *const geo_;
	struct nvm_bbt **bbts_;
	int bbts_length_;
	int blks_length_;


	struct rr_usage_meta rr_u_meta_;
	leveldb::port::Mutex mu_;

	struct Options opt_;
	leveldb::Status s;
};

}
}
#endif
