#include "ocssd.h"
#include "oc_block_manager.h"

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
	ocblkmng_stripe_test_lap();
	return 0;
}
