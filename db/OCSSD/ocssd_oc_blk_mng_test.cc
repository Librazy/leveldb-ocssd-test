#include "ocssd.h"
#include "oc_block_manager.h"

int ocblkmng()
{
	leveldb::ocssd::ocssd ssd;
	if (ssd.ok()) {
		printf("ocssd construct ok.\n");
	}else{
		printf("ocssd construct failed: %s.\n", ssd.s.ToString().c_str()); 
	}
	ssd.TEST_Get_BLK_MNG()->TEST_Pr_Opt_Meta();

	return 0;
}

int main()
{
	ocblkmng();
	return 0;
}
