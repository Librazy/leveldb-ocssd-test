#include "oc_file.h"
#include "oc_ssd.h"

int oc_file_nodelist_test()
{
	leveldb::ocssd::oc_ssd ssd;
	if (ssd.ok()) {
		printf("construct ok.\n");
	}else{
		printf("ocssd construct failed: %s.\n", ssd.s.ToString().c_str()); 
	}
	leveldb::ocssd::oc_file* f = ssd.TEST_New_file("1.txt");
	f->TEST_NodeList();
	return 0;
}

int main()
{
	oc_file_nodelist_test();
	return 0;
}
