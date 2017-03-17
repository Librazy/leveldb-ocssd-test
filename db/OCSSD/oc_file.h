#ifndef YWJ_OC_FILE_H
#define YWJ_OC_FILE_H

#include "leveldb/status.h"


#include "oc_block_manager.h"


namespace leveldb {
namespace ocssd {

class oc_block_manager;
class oc_ssd;

class oc_file { //a "file" to support RandomAccessRead/AppendingWrite
public:
	typedef oc_block_manager::rr_addr RR_Addr;
	typedef oc_block_manager::StripeDes StripeDescriptor;//the "stripe" is in Block-Lun-Plane address format.

	struct metadata{ 			//a "file"'s metadata to dump
		std::string name;       //name
		size_t size;			//size in bytes
		size_t node_num;		//number of nodes
		std::string nodelist_rep_;	//	
		metadata(const char *fname) : name(fname), size(0), node_num(0){ }
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

	struct node{
		StripeDescriptor* stripe;
		size_t size; 
		struct node *next;
		struct node *prev;
		node() : next(this), prev(this), stripe(NULL), size(0){ }
		node(StripeDescriptor *s,const struct nvm_geo *g) : next(this), prev(this), stripe(s) 
		{ 
			size = stripe->ed.Minus(stripe->st, g);
		}
	};

	struct node* AddNodeTail(struct node *listhead, struct node *n);
	//TESTS
	void TEST_TravelNodeList(struct node *listhead);


	friend class oc_ssd;


	struct metadata *meta_;
	port::Mutex mu_;    			//lock to protect meta.

	struct node dummy_stphead_;		//dummy stripe head.

	oc_block_manager *const held_by_blkmng_;
	oc_ssd *const held_by_;
	leveldb::Status s;


	oc_file(oc_ssd *ssd, const char* fname);
	/*
	 * oc_file factory function
	 */
	static leveldb::Status New_oc_file(oc_ssd *ssd, const char* filename, oc_file **oc_file_ptr); 


	// No copying allowed
	oc_file(const oc_file&);
	void operator=(const oc_file&);
}; //oc_file


}//namespace ocssd
}//namespace leveldb

#endif
