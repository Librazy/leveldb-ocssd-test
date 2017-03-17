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
	dummy_stphead_.next = dummy_stphead_.prev = &dummy_stphead_;
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

oc_file::Node* oc_file::AllocNode(int degree = oc_options::kOCFileNodeDegree)
{
}
void oc_file::FreeNode(oc_file::Node *n)
{
}
size_t AddStripe2Node(oc_file::Node *n, oc_file::StripeDescriptor *stripe)
{
}

oc_file::Node* oc_file::AddListTail(oc_file::Node *listhead, oc_file::Node *n)
{
	n->prev = listhead->prev;
	n->next = listhead;

	listhead->prev->next = n;
	listhead->prev = n;
	return n;
}

void oc_file::TravelList(oc_file::Node *listhead, const char *job)
{

}


void oc_file::TEST_NodeList()
{

}


leveldb::Status oc_file::New_oc_file(oc_ssd *ssd, const char* filename, oc_file **oc_file_ptr)
{
	oc_file *ptr = new oc_file(ssd, filename);
	*oc_file_ptr = ptr->ok() ? ptr : NULL;
	return ptr->s;
}
} //namespace ocssd
} //namespcae leveldb
