#ifndef YWJ_OCSSD_H
#define YWJ_OCSSD_H
#include <string>
//DB Headers
#include "leveldb/slice.h"
#include "leveldb/status.h"
#include "db/version_edit.h"
#include "db/dbformat.h"
#include "port/port.h"
#include "port/thread_annotations.h"

//liblightnvm Headers
#include "liblightnvm.h"
#include "nvm.h"

#define BOOL2STR(b) ((b) ? "true" : "false")


namespace leveldb {

namespace ocssd {


class oc_file;
class oc_block_manager;
struct oc_file_descriptor;
class oc_GC;

class ocssd {  //an ocssd device
public:
	ocssd();
	~ocssd();
	void Setup();
	void Cleanup();
	bool ok()
	{
		return s.ok();
	}
	leveldb::Status NewOCFile(oc_file **ret);

	struct ocssd_descriptor {
		const std::string dev_path_;
		std::set<oc_file_descriptor *> files_;
		ocssd_descriptor(const char *name) : dev_path_(name)
		{
		}
	};

	oc_block_manager* TEST_Get_BLK_MNG()
	{
		return blkmng_;
	}
public:
	leveldb::Status s;

private:
	friend class oc_block_manager;
	friend class oc_GC;

	struct nvm_dev *dev_;
	int pmode_;
	struct ocssd_descriptor *des_;
	oc_block_manager *blkmng_;
	


	void EncodeTo(struct ocssd_descriptor *ocdes, char *buf);
	void DecodeFrom(struct ocssd_descriptor *ocdes, char *buf);

	// No copying allowed
	ocssd(const ocssd&);
	void operator=(const ocssd&);

}; //ocssd

struct oc_file_descriptor { //a "file"'s metadata
	std::string name;       //name
	struct nvm_addr start;  //start from which ppa
	int chunksize;          //granularity to chunking, in page_size(page_size is 4k bytes by default)
	int chunks;             //how many chunks this file owned
};

class oc_file { //a "file" to support RandomAccessRead/AppendingWrite
public:

	~oc_file();

	leveldb::Status Append()
	{
	}
	leveldb::Status RandomReadByOffset() //Offset is byte-oriented.
	{
	}
private:
	oc_file()
	{
	}

	friend class ocssd;

	struct oc_file_descriptor *meta;
	port::Mutex mu_;    //lock to protect meta.

	ocssd *held_by;
	leveldb::Status s;


	// No copying allowed
	oc_file(const oc_file&);
	void operator=(const oc_file&);
}; //oc_file

} //namespace ocssd
} //namespace leveldb

#endif
