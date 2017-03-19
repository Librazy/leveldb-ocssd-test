#ifndef YWJ_OCSSD_OC_PAGE_CACHE_H
#define YWJ_OCSSD_OC_PAGE_CACHE_H

//liblightnvm headers
#include "liblightnvm.h"
#include "nvm.h"

#include "leveldb/status.h"

//mutex & condition varieble headers
#include "port/port.h"



#include <queue>
#include <vector>

namespace leveldb{
namespace ocssd{


class oc_ssd;
class oc_page; 


class oc_page_pool {
public:

	struct p_entry {
		int degree_;
		int usage_;
		uint32_t bitmap_;
		oc_page *reps[1];
	};
	
	~oc_page_pool();
	leveldb::Status AllocPage(oc_page **pptr); 
	void DeallocPage(oc_page* p);


private:

	friend class oc_ssd;

	oc_page_pool(struct nvm_dev *dev);

	struct p_entry_cmp{
		bool operator()(p_entry *a, p_entry* b){
			a->usage_ > b->usage_;
		}
	};

	std::priority_queue<p_entry*, std::vector<p_entry*>, p_entry_cmp> pool_;
	leveldb::port::Mutex pool_lock_;

	p_entry* Alloc_p_entry();
	void Dealloc_p_entry(p_entry *pe); 

	const size_t page_size_;
	const struct nvm_geo * const geo_;
	leveldb::Status s;
};


class oc_page{
public:
	size_t Append(const char* str, size_t len);
	inline size_t Left(){
		return reinterpret_cast<char*>(ptr_) + size_ - reinterpret_cast<char*>(ofs_);  
	}
	~oc_page();

private:
	friend class oc_page_pool;
	
	void *ptr_;
	void *ofs_;
	oc_page_pool::p_entry *held_;
	int idx_;
	size_t size_;
	
	oc_page(void *mem, int i, oc_page_pool::p_entry *p);

	//no copy
	oc_page(oc_page const &);
	const oc_page operator=(oc_page const &);
};

/* 
 * a wrapper to oc_page. 
 *  
 */
class oc_buffer{	
public:
	oc_buffer();
	~oc_buffer();
	append();
	clear();
	inline bool empty(){
		return size_ == 0;
	}
	inline size_t size(){
		return size_;
	}

	dump2file();


private:
	oc_page_pool *page_pool_;
	std::vector<oc_page *> pages_;
	size_t size_;
};

}//namespace ocssd
}//namespace leveldb


#endif
