#include "oc_block_manager.h"

#include "util/mutexlock.h"

#include <cassert>

namespace leveldb {
namespace ocssd {

namespace {

const oc_block_manager::BlkState_t BLK_UNUSED = NVM_BBT_FREE; 
const oc_block_manager::BlkState_t BLK_USED = NVM_BBT_HMRK;
const oc_block_manager::BlkState_t BLK_INVALID = NVM_BBT_GBAD;
const int CH_THIS = 0;	//Qemu OCSSD is only 1 channel
/* 
 * The flags transition 
 *  
 *             write_opr                 update_opr/remove_opr
 * BLK_UNUSED --------------> BLK_USED --------------------->BLK_INVALID
 *     |                                                       |
 *     |<------------------------------------------------------|
 *     				          erase_opr
 */
};

/* 
 * BBTs Layout:
 *  		  L0    L2   L3   L4
 *  		  p0p1  p0p1 p0p1 p0p1	    
 * B0   =     = =   = =  = =  = =
 * B1   =     = =   = =  = =  = =
 * B2   =     = =   = =  = =  = =
 * B3   =     = =   = =  = =  = =
 * ......
 * Bn   =     = =   = =  = =  = = 
 * 			  BBT0  BBT1 BBT2 BBT3 
 */

static inline int _ch_lun2_bbt_idx(int ch, int lun, const struct nvm_geo *geo)
{
	return ch * geo->nluns + lun;
}

static inline int _pl_blk2_bbtblks_idx(int pl, int blk, const struct nvm_geo *geo)
{
	return blk * geo->nplanes + pl;
}




static void my_nvm_bbt_state_pr(int state)
{
	switch (state) {
	case BLK_UNUSED:
		printf("B_00(%d)", state);
		break;
	case BLK_USED:
		printf("B_##(%d)", state);
		break;
	case BLK_INVALID:
		printf("B_!!(%d)", state);
		break;
	default:
		nvm_bbt_state_pr(state);
		break;
	}
}


//REQUIRE: Lun and Pl's value should be less then 16-length
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

static inline size_t _get_left_blks(oc_block_manager::LunAndPlane_t lap,const struct nvm_geo *geo)
{
	return ((geo->nluns - GetLun(lap)) * geo->nplanes) - GetPlane(lap); 
}

void oc_block_manager::TEST_Lap()
{
	for (int i; i < geo_->nluns; i++) {
		for (int j = 0; j < geo_->nplanes; j++) {
			printf("<L%dP%d> %zu\n", i, j, _get_left_blks(MakeLunAndPlane(i, j), geo_));
		}
	}
}

void oc_block_manager::TEST_Add()
{
	size_t testcase[] = {5, 8, 30};

}

void oc_block_manager::_add_blks(size_t blks)
{
	int itr = 0;
	uint32_t l, p;
	size_t left = _get_left_blks(rr_u_meta_.lap, geo_);
	while (1) {
		if (blks < left) {
			l = GetLun(rr_u_meta_.lap) + (blks / geo_->nplanes);
			p = GetPlane(rr_u_meta_.lap) + (blks % geo_->nplanes);
			rr_u_meta_.lap = MakeLunAndPlane(l, p);
			return;
		} else {
			assert(itr == 0); // at most go 1 time here.
			rr_u_meta_.lap = 0;
			rr_u_meta_.block++;
			blks -= left;
			if (blks > 0) {
				rr_u_meta_.block = rr_u_meta_.block + (blks / (geo_->nluns * geo_->nplanes));
				left = geo_->nluns * geo_->nplanes;
				blks = blks % (geo_->nluns * geo_->nplanes);
			}
			//a more round.
		}
		itr++;
	}
}

leveldb::Status oc_block_manager::AllocStripe(size_t bytes, StripeDes *sd)
{
	assert(bytes % opt_.chunk_size == 0);
	size_t blks = bytes / opt_.chunk_size;


}

leveldb::Status oc_block_manager::FreeStripe(StripeDes *sd)
{

}

leveldb::Status oc_block_manager::FreeStripeArray(StripeDes *sds, int num)
{

}

bool oc_block_manager::ok()
{
	return s.ok();
}
void oc_block_manager::TEST_Pr_Opt_Meta()
{
	printf("BLKMNG Opts:\n");
	printf("bbt_cached: %s.\n"
		"addralloc: %s.\n"
		"chunk_size: %zu.\n",
		BOOL2STR(opt_.bbt_cached),
		opt_.policy == oc_options::kRoundRobin_Fixed ? "RoundRobin_Fixed" : "Other",
		opt_.chunk_size);
	printf("BLKMNG Init Meta:\n");
	printf("Next LAP: L %d, P %d\n" "Next Block:\n", GetLun(rr_u_meta_.lap), GetPlane(rr_u_meta_.lap)); 
	nvm_addr_pr(rr_u_meta_.block);
}



leveldb::Status oc_block_manager::TEST_Pr_BBT()
{
	int i;
	struct nvm_addr lun_addr;
	struct nvm_ret ret;
	const struct nvm_bbt *ptr, *ptr2;

//	printf("BBTs: %p %p\n", bbts_, ssd_->dev_->bbts);


	for (i = 0; i < geo_->nluns; i++) {
		ptr = bbts_[_ch_lun2_bbt_idx(CH_THIS, i, geo_)];
		lun_addr.ppa = 0;
		lun_addr.g.lun = i;
//		printf("before: %p\n", ssd_->dev_->bbts[_ch_lun2_bbt_idx(CH_THIS, i, geo_)]);
//		if (ptr != (ptr2 = nvm_bbt_get(ssd_->dev_, lun_addr, &ret))) {
//			printf("Ptr!: %p, %p, %p\n", ptr, ssd_->dev_->bbts[_ch_lun2_bbt_idx(CH_THIS, i, geo_)],ptr2);
//		}

		TEST_My_nvm_bbt_pr(i, ptr);
	}
	return leveldb::Status::OK();

BBT_ERR:
	return leveldb::Status::IOError("Get BBT", strerror(errno));
}

#define PRBBT_VERBOSE

void oc_block_manager::TEST_My_nvm_bbt_pr(int lun, const struct nvm_bbt *bbt)
{
	int nnotfree = 0;
	const int Pr_num = 4;
	int pred = 0, pr_sr = 0;
	if (!bbt) {
		printf("bbt { NULL }\n");
		return;
	}
	printf("LUN:%d", lun);
	printf("bbt {\n");
	printf("  addr"); nvm_addr_pr(bbt->addr);
	printf("  nblks(%lu) {", bbt->nblks);
	for (int i = 0; i < bbt->nblks; i += bbt->dev->geo.nplanes) {
		int blk = i / bbt->dev->geo.nplanes;
#ifdef PRBBT_VERBOSE
		if (pred < Pr_num /*first Pr_num ones*/
			|| i == bbt->nblks - bbt->dev->geo.nplanes/*last one*/) {
#endif
			printf("\n    blk(%04d): [ ", blk);
			for (int blk = i; blk < (i + bbt->dev->geo.nplanes); ++blk) {
				my_nvm_bbt_state_pr(bbt->blks[blk]);
				printf(" ");
				if (bbt->blks[blk]) {
					++nnotfree;
				}
			}
			printf("]");
			pred++;
#ifdef PRBBT_VERBOSE
		} else if (!pr_sr) {
			printf("\n....");
			pr_sr = 1;
		}
#endif
	}
	printf("\n  }\n");
	printf("  #notfree(%d)\n", nnotfree);
	printf("}\n");
}

void oc_block_manager::def_ocblk_opt(struct Options *opt)
{
	opt->chunk_size = geo_->npages * geo_->page_nbytes; //block's size in bytes
	opt->policy = oc_options::kRoundRobin_Fixed;
	opt->bbt_cached = true;
}

//TODO - BBT_Management_Basic_Workflow: Get dev->bbts[...], Maintain, Flush

/*
 * Erase all blocks and set the corresponding BBT entry as free
 */
void oc_block_manager::InitClean()
{
	int i;
	struct nvm_bbt *ptr;
	//Erase
	for (i = 0; i < geo_->nluns; ++i) {
	//	printf("oc_block_manager::InitClean, Lun %d\n", i);
		s = oc_GC::EraseByLun(CH_THIS, i, this); 
	}
	if (!s.ok()) {
		goto OUT;
	}

	//Set BBTs as all free.
	for (i = 0; i < bbts_length_; ++i) {
		ptr = bbts_[i];
		assert(ptr);//ptr should not be null.
		for (int j = 0; j < blks_length_; ++j) {
			ptr->blks[j] = BLK_UNUSED;
		}
	}

OUT:
	return;
}

/*
 * 
 */
void oc_block_manager::InitBBTs()
{
	int i;
	struct nvm_addr lun_addr;
	struct nvm_ret ret;
	const struct nvm_bbt *ptr;
	assert(opt_.bbt_cached); //now only support bbt cached.

	if (opt_.bbt_cached) {
		if (nvm_dev_set_bbts_cached(ssd_->dev_, 1)) {
			s = leveldb::Status::IOError("oc_blk_mng: set bbt_cached", strerror(errno));
		}
	}

	if (s.ok()) {
		for (i = 0; i < geo_->nluns; i++) {
			lun_addr.ppa = 0;
			lun_addr.g.lun = i;
			ptr = nvm_bbt_get(ssd_->dev_, lun_addr, &ret);
			if (!ptr) {
				goto BBT_ERR;
			}
		}
		bbts_ = ssd_->dev_->bbts;
		bbts_length_ = geo_->nchannels * geo_->nluns; //sum of the LUNs.
		blks_length_ = geo_->nplanes * geo_->nblocks; //sum of the blks inside a LUNs.
	}
	return; //ok, then we can maintan the bbts now.

BBT_ERR:
	s = leveldb::Status::IOError("oc_blk_mng: get bbts", strerror(errno));
}

void oc_block_manager::FlushBBTs() ///WARNING - Flush the BBT will cause it free the bbt entry.
{
	struct nvm_ret ret;
	if (nvm_bbt_flush_all(ssd_->dev_, &ret)) {
		s = leveldb::Status::IOError("oc_blk_mng: flush bbts", strerror(errno));
	}
}


void oc_block_manager::Init()
{
	InitBBTs();
	InitClean();
}

oc_block_manager::oc_block_manager(ocssd *ssd) : ssd_(ssd), geo_(nvm_dev_get_geo(ssd->dev_))
{
	def_ocblk_opt(&opt_);
	Init();
}

/*
 * 
 */
leveldb::Status oc_block_manager::New_oc_block_manager(ocssd *ssd,  oc_block_manager **oc_blk_mng_ptr)
{
	oc_block_manager *ptr = new oc_block_manager(ssd);
	*oc_blk_mng_ptr = ptr->ok() ? ptr : NULL;
	return ptr->s;
}

} //namespace ocssd
} //namespace leveldb
