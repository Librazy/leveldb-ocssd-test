#include "oc_block_manager.h"

namespace leveldb {
namespace ocssd {

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


/*
 * @approximate_size - how many bytes to alloc(approximately)
 */
leveldb::Status oc_block_manager::AllocBlocks(size_t approximate_size, oc_block_manager::AllocBlkDes *abd)
{
	return leveldb::Status::OK();
}

leveldb::Status oc_block_manager::FreeBlocks(oc_block_manager::AllocBlkDes* blks)
{
	return leveldb::Status::OK();
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
	printf("Next LAP: L %d, P %d\n" "Next Block:\n", GetLun(rr_u_meta_.next_lap), GetPlane(rr_u_meta_.next_lap));
	nvm_addr_pr(rr_u_meta_.next_block);
}

leveldb::Status oc_block_manager::TEST_Pr_BBT()
{
	int i;
	struct nvm_addr lun_addr;
	struct nvm_ret ret;
	const struct nvm_bbt *ptr;
	for (i = 0; i < geo_->nluns; i++) {
		lun_addr.ppa = 0;
		lun_addr.g.lun = i;
		ptr = nvm_bbt_get(ssd_->dev_, lun_addr, &ret);
		if (!ptr) {
			goto BBT_ERR;
		}
		TEST_My_nvm_bbt_pr(i, ptr);
	}
	return leveldb::Status::OK();

	BBT_ERR:
	return leveldb::Status::IOError("get BBT", strerror(errno));
}

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
		if (pred < Pr_num /*first Pr_num ones*/
			|| i == bbt->nblks - bbt->dev->geo.nplanes/*last one*/) {
			printf("\n    blk(%04d): [ ", blk);
			for (int blk = i; blk < (i + bbt->dev->geo.nplanes); ++blk) {
				nvm_bbt_state_pr(bbt->blks[blk]);
				printf(" ");
				if (bbt->blks[blk]) {
					++nnotfree;
				}
			}
			printf("]");
			pred++;
		} else if (!pr_sr) {
			printf("\n....");
			pr_sr = 1;
		}
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

void oc_block_manager::Clean()
{

}


oc_block_manager::oc_block_manager(ocssd *ssd) : ssd_(ssd), geo_(nvm_dev_get_geo(ssd->dev_))
{
	def_ocblk_opt(&opt_);
	if (opt_.bbt_cached) {
		if (nvm_dev_set_bbts_cached(ssd->dev_, 1)) {
			s = leveldb::Status::IOError("oc_blk_mng: set bbt_cached", strerror(errno));
		}
	}
	Clean();
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