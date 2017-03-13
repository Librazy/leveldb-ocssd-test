#ifndef YWJ_OCSSD_OC_BLK_MNG_H
#define YWJ_OCSSD_OC_BLK_MNG_H
namespace leveldb {
namespace ocssd {

class oc_block_manager { //allocation is done in a granularity of <Block>
public:
	typedef uint32_t LunAndPlane_t;

	struct AllocBlkDes {
	};

	/*
	 * @approximate_size - how many bytes to alloc(approximately)
	 */
	struct AllocBlkDes AllocBlocks(size_t approximate_size);

	leveldb::Status FreeBlocks(struct AllocBlkDes blks);

	bool ok();

	void TEST_Pr_Opt_Meta();
	void TEST_Pr_BBT();
	static void TEST_My_nvm_bbt_pr(const struct nvm_bbt *bbt);
private:
	/*  Problems to be issued:
	 *  1. a file can be discontinuous in lun : (a file's size is changable at any time: support Append as a FS?)
	 *  2. chunk size is flexible to each incoming request or fixed by a definition before ?
	 * 
	 */
	struct Options {
		size_t chunk_size;          //default: a block's size in bytes.
		kAddrAllocPolicy policy;    //default: kRoundRobin_Fixed(an allocation will alloc the same chunks for each plane.)
		bool bbt_cached;            //default: enabled
	};
	struct rr_usage_meta {
		LunAndPlane_t next_lap;
		struct nvm_addr next_block;
		pap_rr_usage_meta() : next_lap(0)
		{
			next_block.ppa = 0;
		}
	};
	friend class ocssd;

	void def_ocblk_opt(struct Options *opt);
	void clean_all_set_free();


	oc_block_manager(ocssd *ssd);

	/*
	 * 
	 */
	static leveldb::Status New_oc_block_manager(ocssd *ssd,  oc_block_manager **oc_blk_mng_ptr);

	ocssd *const ssd_;
	const struct nvm_geo *const geo_;

	struct rr_usage_meta rr_u_meta_;
	struct Options opt_;
	leveldb::Status s;
};

}
}
#endif
