#include "oc_page_cache.h"
#include "oc_options.h"


#include "util/mutexlock.h"

#include <cstring>
#include <cassert>
#include <cstdint>
#include <malloc.h>

namespace leveldb {
namespace ocssd {
const int kPEntry_Degree = 32; //Must Be 32.


oc_page::oc_page(void *mem, int i, oc_page_pool::p_entry *p) : ptr_(mem), ofs_(mem), idx_(i), held_(p)
{
}

oc_page::~oc_page()
{
	if (ptr_) {
		free(ptr_);
	}
}
/*
 * usable range: [ofs_, ptr_ + size_)
 */
size_t oc_page::Append(const char* str, size_t len)
{
	size_t actual = NVM_MIN(Left(), len);
	memcpy(ofs_, str, actual);
	ofs_ = reinterpret_cast<void*>(reinterpret_cast<char*>(ofs_) + actual);
	assert(ofs_ - ptr_ <= size_);
	return actual;
}
/*
 * gaudent algorithm
 * require 	- only 1 bit of @x is 1. 
 * return 	- the index of this bit.
 * e.g.
 * 0x00000400 --> 10
 */
static inline int bitmap_ntz32(uint32_t x)
{
	int b4,b3,b2,b1,b0;
	b4 = (x & 0x0000ffff)? 0:16;
	b3 = (x & 0x00ff00ff)? 0:8;
	b2 = (x & 0x0f0f0f0f)? 0:4;
	b1 = (x & 0x33333333)? 0:2;
	b0 = (x & 0x55555555)? 0:1;
	return b0 + b1 + b2 + b3 + b4;
}

/*
 * get an empty slot(bit == 0) to use
 */
static int bitmap_get_slot(uint32_t bitmap)
{
	if(~bitmap == 0){
		return -1;//FULL
	}
	uint32_t x = bitmap | (bitmap + 1);//first bit 0 ==> 1
	x = x & (~bitmap);

	return bitmap_ntz32(x);
}

static inline void bitmap_set(uint32_t *bitmap, int idx)
{
	*bitmap = *bitmap | (1 << idx );
}

static inline void bitmap_unset(uint32_t *bitmap, int idx)
{
	*bitmap = *bitmap & (~(1 << idx ));
}




oc_page_pool::oc_page_pool(struct nvm_dev *dev) : geo_(nvm_dev_get_geo(dev)), page_size_(geo_->page_nbytes)
{
}

oc_page_pool::~oc_page_pool()
{
	oc_page_pool::p_entry *ptr;
	{ 
		leveldb::MutexLock l(&pool_lock_);
		while (!pool_.empty()) {
			ptr = pool_.top();
			Dealloc_p_entry(ptr);
			pool_.pop();
		}
	}
}

oc_page_pool::p_entry* oc_page_pool::Alloc_p_entry()
{
	int degree = kPEntry_Degree;
	p_entry* ptr = (p_entry*) malloc(sizeof(p_entry) + (degree - 1) * sizeof(oc_page *));
	if(!ptr){
		s = leveldb::Status::IOError("oc_page_pool::Alloc_p_entry", strerror(errno));
		return NULL;
	}
	ptr->degree_ = degree;
	ptr->usage_ = 0;
	ptr->bitmap_ = 0;

	for (int i = 0; i < ptr->degree_; i++) {
		void *mem = nvm_buf_alloc(geo_, page_size_);
		if (!mem) {
			s = leveldb::Status::IOError("oc_page_pool::Alloc_p_entry nvm_buf_alloc", strerror(errno));
			return NULL;
		}
		ptr->reps[i] = new oc_page(mem, i, ptr);
		ptr->reps[i]->size_ = page_size_;
	}

	return ptr;
}

void oc_page_pool::Dealloc_p_entry(oc_page_pool::p_entry *pe)
{
	if(pe){
		for (int i = 0; i < pe->degree_; i++) {
			delete pe->reps[i];
		}
		free(pe);
	}
}

leveldb::Status oc_page_pool::AllocPage(oc_page** pptr)
{
	oc_page_pool::p_entry *ptr = NULL;
	int slot = -1;
	*pptr = NULL;
	bool need_alloc = true;
	if (!pool_.empty()) {
		{
			leveldb::MutexLock l(&pool_lock_);
			ptr = pool_.top();
			slot = bitmap_get_slot(ptr->bitmap_);

			assert(slot < kPEntry_Degree);
			if (slot >= 0) {
				pool_.pop();
				need_alloc = false;
			}
		}
	}

	if (need_alloc) {
		ptr = Alloc_p_entry();
		if (!s.ok()) {
			goto OUT;
		}
		slot = bitmap_get_slot(ptr->bitmap_);
	}

	assert(slot >= 0 && slot < kPEntry_Degree);

	//ptr is out of the queue. so don't need to LOCK.
	bitmap_set(&(ptr->bitmap_), slot);
	ptr->usage_++;

	{
		leveldb::MutexLock l(&pool_lock_);
		pool_.push(ptr);
	}

	*pptr = ptr->reps[slot];
	
OUT:
	return s;
}


/*
 * 
 */
void oc_page_pool::DeallocPage(oc_page *p)
{
	{
		leveldb::MutexLock l(&pool_lock_);
		bitmap_unset(&(p->held_->bitmap_), p->idx_);
		p->held_->usage_--;
		std::make_heap(const_cast<oc_page_pool::p_entry**>(&pool_.top()),
            const_cast<oc_page_pool::p_entry**>(&pool_.top()) + pool_.size(),
			p_entry_cmp());
	}
}

}//namespace ocssd
}//namespace leveldb
