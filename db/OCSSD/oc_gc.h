#ifndef YWJ_OCSSD_OC_GC_H
#define YWJ_OCSSD_OC_GC_H

#include "leveldb/status.h"

#include "oc_block_manager.h"

//liblightnvm Headers
#include "liblightnvm.h"
#include "nvm.h"


namespace leveldb {
namespace ocssd {

class oc_block_manager;

class oc_GC{
public:
	static leveldb::Status EraseByLun(int ch, int lun, oc_block_manager *blk_mng);
	static leveldb::Status EraseByBlock(int ch, int lun, int blk, oc_block_manager *blk_mng);

private:
	
};

}//namespace ocssd
}//namespace leveldb

#endif
