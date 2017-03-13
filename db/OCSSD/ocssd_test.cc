#include "ocssd.h"

int construct()
{
	leveldb::ocssd::ocssd ssd;
	if (ssd.ok()) {
		printf("construct ok.\n");
	}
	return 0;
}

int main()
{
	return 0;
}
