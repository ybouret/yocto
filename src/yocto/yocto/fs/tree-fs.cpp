#include "yocto/fs/tree-fs.hpp"
#include "yocto/string/tokenizer.hpp"

namespace yocto
{
    static inline bool isSep(const char C) throw()
    {
        return (C=='/');
    }
    
    void __TreeFS:: SplitPath(sequence<string> &ids, const string &path)
    {
        ids.free();
        tokenizer::split(ids,path,isSep);
    }
}
