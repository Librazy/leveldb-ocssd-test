#include "oc_ssd.h"
#include "oc_page_cache.h"

int page_pool_test()
{
	leveldb::ocssd::oc_ssd ssd;
	leveldb::ocssd::oc_page_pool *pool;
	if (ssd.ok()) {
		printf("construct ok.\n");
	}else{
		printf("ocssd construct failed: %s.\n", ssd.s.ToString().c_str()); 
	}
	pool = ssd.PagePool();
	pool->TEST_Basic();

	return 0;
}

int main()
{
	page_pool_test();
	return 0;
}
