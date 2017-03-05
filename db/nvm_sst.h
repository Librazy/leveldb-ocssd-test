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


namespace nvm_ocssd {
namespace nvm_options {
const char *kDevPath = "/dev/nvme0n1";
};


class leveldb::debug_helper;
class nvm_file;

class nvm_descriptor {
public:
    nvm_descriptor();
    ~nvm_descriptor();
    int Setup();

private:
    friend class leveldb::debug_helper;
    struct nvm_dev *dev;
    const struct nvm_geo *geo;

    const std::string dev_path_;


    // No copying allowed
    nvm_descriptor(const nvm_descriptor&);
    void operator=(const nvm_descriptor&);

}; //nvm_descriptor



class nvm_dir {

private:
    friend class leveldb::debug_helper;
    typedef std::set<nvm_file *> Files;

    std::string dbname_; //is the db's dir
    Files ssts_;

    // No copying allowed
    nvm_dir(const nvm_dir&);
    void operator=(const nvm_dir&);
}; //nvm_dir

class nvm_file {

private:
    friend class leveldb::debug_helper;
    uint64_t number;

    // No copying allowed
    nvm_file(const nvm_file&);
    void operator=(const nvm_file&);
}; //nvm_file

} //namespace nvm_ocssd
} //namespace leveldb

#endif
