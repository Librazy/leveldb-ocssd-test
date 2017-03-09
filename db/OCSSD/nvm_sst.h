#ifndef YWJ_NVM_SST_H
#define YWJ_NVM_SST_H
#include <string>
//DB Headers
#include "leveldb/slice.h"
#include "db/version_edit.h"
#include "db/dbformat.h"

//OCSSD Headers
#include "liblightnvm.h"
#include "nvm.h"


namespace leveldb {


namespace ocssd {
namespace oc_options {
const char *kDevPath = "/dev/nvme0n1";
const char *kOCSSDMetaFileNameSuffix = ".ocssd";
enum ChunkingAlgor{
    kRaid0 = 0x0,   //No parity
    kRaid4 = 0x1    //with parity spread around the stripe
};
enum GCPolicy{
    kOneLunSerial = 0x0,    //Erase Operation is emited to one LUN by serializtion 
    kAllIn = 0x1
};
};


class leveldb::debug_helper;
class oc_file;
struct oc_file_descriptor;

class oc_descriptor {  //an ocssd device
public:
    oc_descriptor();
    ~oc_descriptor();
    int Setup();

private:
    friend class leveldb::debug_helper;
    const std::string dev_path_;

    struct nvm_dev *dev_;
    const struct nvm_geo *geo_;

    std::set<oc_file_descriptor*> files_;

    void EncodeTo(std::string fname);
    void DecodeFrom(std::string fname);

    // No copying allowed
    oc_descriptor(const oc_descriptor&);
    void operator=(const oc_descriptor&);

}; //oc_descriptor

struct oc_file_descriptor{ //a "file"'s metadata
    //start from which ppa
    //granularity to chunking, in page_size(page_size is 4k bytes by default)
    //how many chunks this file owned
};

class oc_file { //a "file" to support RandomAccessRead/AppendingWrite

private:
    friend class leveldb::debug_helper;
    

    // No copying allowed
    oc_file(const oc_file&);
    void operator=(const oc_file&);
}; //oc_file

} //namespace ocssd
} //namespace leveldb

#endif
