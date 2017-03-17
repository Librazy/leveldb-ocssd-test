#ifndef YWJ_OC_FILE_H
#define YWJ_OC_FILE_H

#include "leveldb/status.h"

#include "oc_options.h"
#include "oc_block_manager.h"


namespace leveldb {
namespace ocssd {

class oc_block_manager;
class oc_ssd;

class oc_file { //a "file" to support RandomAccessRead/AppendingWrite
public:
	typedef oc_block_manager::rr_addr RR_Addr;
	typedef oc_block_manager::StripeDes StripeDescriptor; //the "stripe" is in Block-Lun-Plane address format.

	struct metadata {            	//a "file"'s metadata to dump
		std::string name;       	//name
		size_t size;            	//size in bytes
		size_t node_num;        	//number of nodes
		std::string nodelist_rep_;  //
		metadata(const char *fname) : name(fname), size(0), node_num(0) { }
	};

	typedef struct metadata oc_file_descriptor;


	bool ok()
	{
		return s.ok();
	}

	~oc_file();

	leveldb::Status Append();
	leveldb::Status RandomReadByOffset(); //Offset is byte-oriented.


	//TESTS
	void TEST_NodeList();
private:

	/* 
	 *  
	 * a oc_file::Node is consist of several stripes allocated from oc_block_manager, 
	 * whose default max stripe's number(AKA degree) is oc_options::kOCFileNodeDegree. 
	 */
	struct Node {
		size_t size;                    //Node's size in bytes
		size_t degree;
		struct Node *next;
		struct Node *prev;
		StripeDescriptor *stp_arr[1];
	};

	//oc_file's Node
	struct Node* AllocNode(int degree = oc_options::kOCFileNodeDegree);
	void FreeNode(struct Node *n);
	size_t AddStripe2Node(struct Node *n, StripeDescriptor *stripe);

	struct Node* AddListTail(struct Node *listhead, struct Node *n);
	void TravelList(struct Node *listhead, const char *job);




	friend class oc_ssd;


	struct metadata *meta_;
	port::Mutex mu_;                //lock to protect meta.

	struct Node dummy_stphead_;     //dummy stripe head.

	oc_block_manager *const held_by_blkmng_;
	oc_ssd *const held_by_;
	leveldb::Status s;


	oc_file(oc_ssd *ssd, const char *fname);
	/*
	 * oc_file factory function
	 */
	static leveldb::Status New_oc_file(oc_ssd *ssd, const char *filename, oc_file **oc_file_ptr);


	// No copying allowed
	oc_file(const oc_file&);
	void operator=(const oc_file&);
}; //oc_file


} //namespace ocssd
} //namespace leveldb

#endif
