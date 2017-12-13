#include "yocto/json/value.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/ocstream.hpp"

using namespace yocto;

YOCTO_UNIT_TEST_IMPL(load)
{
    JSON::Value value;
    if(argc>1)
    {
        for(int i=1;i<argc;++i)
        {
            Lang::Source         source( Lang::Module::OpenFile(argv[1]) );
            value = JSON::Value::LoadFrom(source);
        }
    }
    else
    {
        Lang::Source         source(Lang::Module::OpenSTDIN());
        value = JSON::Value::LoadFrom(source);
    }
    std::cerr << "value=" << value << std::endl;
    std::cerr << "output: " << std::endl;
    std::cerr.flush();
    ios::ocstream fp( ios::cstderr );
    value.output(fp);
    fp << "\n";
}
YOCTO_UNIT_TEST_DONE()
