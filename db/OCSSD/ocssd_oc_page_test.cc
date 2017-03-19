#include "oc_ssd.h"
#include "oc_page_cache.h"

int construct()
{
	leveldb::ocssd::oc_ssd ssd;
	leveldb::ocssd::oc_page_pool *pool;

	if (ssd.ok()) {
		printf("construct ok.\n");
	}else{
		printf("ocssd construct failed: %s.\n", ssd.s.ToString().c_str()); 
	}
	pool = ssd->PagePool();
	return 0;
}

int main()
{
	construct();
	return 0;
}
