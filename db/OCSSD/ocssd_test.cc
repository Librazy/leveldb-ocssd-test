#include "oc_ssd.h"

int construct()
{
	leveldb::ocssd::oc_ssd ssd;
	if (ssd.ok()) {
		printf("construct ok.\n");
	}else{
		printf("ocssd construct failed: %s.\n", ssd.s.ToString().c_str()); 
	}
	return 0;
}

int main()
{
	construct();
	return 0;
}
