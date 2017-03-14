#ifndef YWJ_OCSSD_OC_BLK_MNG_H
#define YWJ_OCSSD_OC_BLK_MNG_H

#include "ocssd.h"
#include "oc_options.h"
#include "oc_gc.h"


//liblightnvm Headers
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

	struct AllocBlkDes {
	};


	/*
	 * @approximate_size - how many bytes to alloc(approximately)
	 */
	leveldb::Status AllocBlocks(size_t approximate_size, AllocBlkDes *abd);

	leveldb::Status FreeBlocks(AllocBlkDes *blks);

	bool ok();

	void TEST_Pr_Opt_Meta();
	leveldb::Status TEST_Pr_BBT();
	static void TEST_My_nvm_bbt_pr(int lun, const struct nvm_bbt *bbt);
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
		LunAndPlane_t next_lap;
		struct nvm_addr next_block;
		rr_usage_meta() : next_lap(0)
		{
			next_block.ppa = 0;
		}
	};
	friend class ocssd;
	friend class oc_GC;

	void def_ocblk_opt(struct Options *opt);
	void InitClean();
	void InitBBTs();
	void Init();

	oc_block_manager(ocssd *ssd);

	/*
	 * 
	 */
	static leveldb::Status New_oc_block_manager(ocssd *ssd,  oc_block_manager **oc_blk_mng_ptr);

	ocssd *const ssd_;
	const struct nvm_geo *const geo_;
	struct nvm_bbt **bbts_;
	int bbts_length_;
	struct rr_usage_meta rr_u_meta_;
	struct Options opt_;
	leveldb::Status s;
};

}
}
#endif
