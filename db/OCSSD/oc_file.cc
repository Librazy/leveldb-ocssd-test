//Dependencies headers
#include "oc_ssd.h"
#include "oc_block_manager.h"


#include "oc_file.h"


namespace leveldb {
namespace ocssd {

oc_file::oc_file(oc_ssd *ssd, const char *fname)
	: held_by_(ssd),
	  held_by_blkmng_(ssd->blkmng_),
	  meta_(new oc_file::metadata(fname))
{
}

oc_file::~oc_file()
{
	delete meta_;
}

leveldb::Status oc_file::Append()
{
}
leveldb::Status oc_file::RandomReadByOffset() //Offset is byte-oriented.
{
}

oc_file::node* oc_file::AddNodeTail(oc_file::node *listhead, oc_file::node *n)
{
	n->prev = listhead->prev;
	n->next = listhead;

	listhead->prev->next = n;
	listhead->prev = n;
	return n;
}



void oc_file::TEST_NodeList()
{
	oc_file::RR_Addr r0, r1, r2, r3;
	r0.lap = 0;
	r1.lap = 1;
	r2.lap = 2;
	r3.lap = 3;
	oc_file::StripeDescriptor des0(r0,r3), des1(r1,r3), des2(r2,r3);
	oc_file::node n0(&des0, held_by_blkmng_->geo_), n1(&des1, held_by_blkmng_->geo_), n2(&des2, held_by_blkmng_->geo_); 
	AddNodeTail(&dummy_stphead_, &n0);
	AddNodeTail(&dummy_stphead_, &n1);
	AddNodeTail(&dummy_stphead_, &n2);
	TEST_TravelNodeList(&dummy_stphead_);
}

void oc_file::TEST_TravelNodeList(oc_file::node *listhead)
{
	oc_file::node* ptr;
	for (ptr = listhead->next; ptr != listhead; ptr = ptr->next) {
		printf("%d ", ptr->stripe->st.lap);
	}
	printf("\n");
}


leveldb::Status oc_file::New_oc_file(oc_ssd *ssd, const char* filename, oc_file **oc_file_ptr)
{
	oc_file *ptr = new oc_file(ssd, filename);
	*oc_file_ptr = ptr->ok() ? ptr : NULL;
	return ptr->s;
}
} //namespace ocssd
} //namespcae leveldb
