#include "oc_ssd.h"
#include "oc_table_builder.h"


#include "../db_impl.h"				//SanitizeOptions
#include "../dbformat.h"
#include "../filename.h"
#include "../table_cache.h"

#include "port/port.h"

#include "leveldb/options.h"
#include "leveldb/env.h"
#include "leveldb/table_builder.h" 	//for table write
#include "leveldb/table.h"			//for table read
#include "leveldb/slice.h"


#include <cstdlib>
#include <cstdio>
#include <functional>
#include <map>
#include <string>

#include <inttypes.h>


uint64_t file_number = 1;
std::string TestDBName = "db/OCSSD/resource/Test";

int CMapLen;
char CMap[] = "qwertyuiopasdfghjklzxcvbnmABCDQWERTYUIOPASDFGHJKLZXCVBNM1234567890[]`!@#$%^&*()";

typedef std::string kvtype;
typedef std::map<kvtype, kvtype> KVMap;
KVMap KV;

void GetRandomString(std::string& s)
{
	int idx;
	int len = rand() % 200;
	s.clear();
	while (len) {
		idx = rand() % CMapLen;
		s.append(1, CMap[idx]);
		len--;
	}
}

void constructKV(int h)
{
	std::pair<KVMap::iterator, bool> ret;
	std::string k, v;
	while (h) {
		GetRandomString(k);
		GetRandomString(v);

		ret = KV.insert(std::pair<kvtype, kvtype>(k, v));
		if (ret.second) {
			h--;
		}
	}
}

void IterateKV()
{
	KVMap::iterator itr;
	for (itr = KV.begin(); itr != KV.end(); ++itr) {
		printf("<%s,%s>\n", itr->first.c_str(), itr->second.c_str());
	}
}

void IterateKV(leveldb::TableBuilder *b)
{
	leveldb::Status s;
	KVMap::iterator itr;
	printf("Insert Into File:\n");
	for (itr = KV.begin(); itr != KV.end(); ++itr) {
		printf("%s, %s\n", itr->first.c_str(), itr->second.c_str());
		b->Add(itr->first, itr->second);
	}
	if (s.ok()) {
		s = b->Finish();
		if (s.ok()) {
			printf("FileSize: %" PRIu64 "\n", b->FileSize());
		}
	} else {
		b->Abandon();
	}
}
void VerifyKVOrg(leveldb::Options& opt, uint64_t filenumber, uint64_t file_size)
{
	leveldb::Status s;
	leveldb::Table *table = NULL;
	leveldb::TableCache *table_cache;
	int table_cache_size = opt.max_open_files - 10;

	table_cache = new leveldb::TableCache(TestDBName, &opt, table_cache_size);

	leveldb::Iterator *itr = table_cache->NewIterator(leveldb::ReadOptions(), filenumber, file_size, &table);
	s = itr->status();
	if (!s.ok()) {
		printf("itr error: %s\n", s.ToString().c_str());
	}else{
		printf("itr status: %s\n", s.ToString().c_str());
	}

	//verify k,v
	itr->SeekToFirst();
	if (itr->Valid()) {
		printf("Read From File:\n");
		 for (; itr->Valid(); itr->Next()) {
			 printf("%s, %s\n", itr->key().ToString().c_str(),  itr->value().ToString().c_str()); 
		 }
	}else{
		printf("First Iterator not valid.\n");
	}

}

void VerifyKVOc(leveldb::Env *env, const char *fname)
{
}

void Construct_TESTKV()
{
	srand(0);
	CMapLen = sizeof(CMap) / sizeof(CMap[0]);

	constructKV(3);
}

int table_builder()
{
	leveldb::ocssd::oc_ssd ssd;
	leveldb::Options raw_opt;
	leveldb::InternalKeyComparator internal_comparator(raw_opt.comparator);
	leveldb::InternalFilterPolicy internal_filter_policy(raw_opt.filter_policy);

	leveldb::Options ldb_opts = SanitizeOptions(TestDBName, &internal_comparator, &internal_filter_policy, raw_opt);

	leveldb::WritableFile * file1;
	leveldb::Env *env = leveldb::Env::Default();
	leveldb::Status s;

	if (ssd.ok()) {
		printf("construct ok.\n");
	} else {
		printf("ocssd construct failed: %s.\n", ssd.s.ToString().c_str());
		return -1;
	}

	Construct_TESTKV();

	std::string file_name = leveldb::TableFileName(TestDBName, file_number);

	s = env->NewWritableFile(file_name, &file1);
	if (!s.ok()) {
		printf("New file1 failed.\n");
		return -1;
	}

	leveldb::TableBuilder *builder_org = new leveldb::TableBuilder(ldb_opts, file1);
	IterateKV(builder_org);

	
	if (s.ok()) {
		s = file1->Sync();
	}
	if (s.ok()) {
		s = file1->Close();
	}
	if (s.ok()) {
		printf("File1 (Org) OK.\n");
	}

	VerifyKVOrg(ldb_opts, file_number, builder_org->FileSize());

	delete builder_org;
	return 0;
}

int main()
{
	table_builder();
	return 0;
}
