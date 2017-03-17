#include "ocssd.h"
#include "oc_block_manager.h"
int ocblkmng_stripe_test_allocstripe()
{
	leveldb::ocssd::oc_block_manager::StripeDes sd;
	leveldb::ocssd::ocssd ssd;
	
	if (ssd.ok()) {
		printf("ocssd construct ok.\n");
	}else{
		printf("ocssd construct failed: %s.\n", ssd.s.ToString().c_str()); 
	}
	{
		size_t blks = 2;
		size_t bytes = blks * ssd.TEST_Get_BLK_MNG()->TEST_Get_ChunkSize();

		printf("Before:");
		ssd.TEST_Get_BLK_MNG()->TEST_Pr_UM();
		printf("\n");
		ssd.TEST_Get_BLK_MNG()->AllocStripe(bytes, &sd);

		printf("After:");
		ssd.TEST_Get_BLK_MNG()->TEST_Pr_UM();
		printf("\n");
	}

	
	{
		size_t blks = 11;
		size_t bytes = blks * ssd.TEST_Get_BLK_MNG()->TEST_Get_ChunkSize();

		printf("Before:");
		ssd.TEST_Get_BLK_MNG()->TEST_Pr_UM();
		printf("\n");
		ssd.TEST_Get_BLK_MNG()->AllocStripe(bytes, &sd);

		printf("After:");
		ssd.TEST_Get_BLK_MNG()->TEST_Pr_UM();
		printf("\n");
	}
	

	ssd.TEST_Get_BLK_MNG()->Pr_BlocksState("1.txt"); 
}

int ocblkmng_stripe_test_rr_itr()
{
	leveldb::ocssd::ocssd ssd;
	if (ssd.ok()) {
		printf("ocssd construct ok.\n");
	}else{
		printf("ocssd construct failed: %s.\n", ssd.s.ToString().c_str()); 
	}
	ssd.TEST_Get_BLK_MNG()->TEST_Itr_rr_addr(); 
	return 0;
}

int ocblkmng_stripe_test_add()
{
	leveldb::ocssd::ocssd ssd;
	if (ssd.ok()) {
		printf("ocssd construct ok.\n");
	}else{
		printf("ocssd construct failed: %s.\n", ssd.s.ToString().c_str()); 
	}
	ssd.TEST_Get_BLK_MNG()->TEST_Add(); 
	return 0;
}


int ocblkmng_stripe_test_lap()
{
	leveldb::ocssd::ocssd ssd;
	if (ssd.ok()) {
		printf("ocssd construct ok.\n");
	}else{
		printf("ocssd construct failed: %s.\n", ssd.s.ToString().c_str()); 
	}
	ssd.TEST_Get_BLK_MNG()->TEST_Lap(); 
	return 0;
}

int ocblkmng_construct_test()
{
	leveldb::ocssd::ocssd ssd;
	if (ssd.ok()) {
		printf("ocssd construct ok.\n");
	}else{
		printf("ocssd construct failed: %s.\n", ssd.s.ToString().c_str()); 
	}
	ssd.TEST_Get_BLK_MNG()->TEST_Pr_Opt_Meta();
	leveldb::Status s = ssd.TEST_Get_BLK_MNG()->TEST_Pr_BBT();
	if (!s.ok()) {
		printf("ocssd Blk Mng pr BBT failed: %s.\n", s.ToString().c_str()); 
	}
	return 0;
}

int main()
{
	ocblkmng_stripe_test_allocstripe();
	return 0;
}
