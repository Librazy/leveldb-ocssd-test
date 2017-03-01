#ifndef YWJ_DEBUG_HELPER
#define YWJ_DEBUG_HELPER
#include <cstdio>
#include "db/version_set.h"
#include "db/version_edit.h"


namespace leveldb {

#define DBG "[debug] - "
#define BOOL_STR(x) ((x)?("true"):("false"))

class debug_helper {
public:
    static void Pr_Debug_U64(const char *str, uint64_t a) {
        printf(DBG "%s: %lu\n", str, a);
    };

    static void Pr_Debug_Bool(const char *str, bool b) {
        printf(DBG "%s: %s\n", str, BOOL_STR(b));
    }

    static void GetVersionSetNextFileNumber(const char *str, VersionSet *vset) {
        printf(DBG "%s: next_file_number: %lu\n", str, vset->next_file_number_);
    };

    static void Pr_VersionEdit(const char *str, VersionEdit *ve) {
        printf(DBG "%s： versionset\n", str);
        printf(" comparator_: %s,\n" 
               " last_sequence_: %lu,\n"
               " log_number_: %lu,\n"
               " next_file_number_: %lu,\n"
               " prev_log_number_: %lu.\n", 
               ve->comparator_.c_str(), ve->last_sequence_, ve->log_number_,
               ve->next_file_number_, ve->prev_log_number_);
    };

    static void Pr_Vector_String(const char *str, std::vector<std::string> &vec) {
        int i = 0;
        printf(DBG "%s\n", str);
        for (std::vector<std::string>::iterator itr = vec.begin();
             itr != vec.end();
             ++itr, ++i) {
            printf(" %d, %s\n", i, itr->c_str());
        }
    };

};
}

#endif
