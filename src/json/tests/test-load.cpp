#include "yocto/json/value.hpp"
#include "yocto/utest/run.hpp"

using namespace yocto;

YOCTO_UNIT_TEST_IMPL(load)
{
    JSON::Value value;
    if(argc>1)
    {
        for(int i=1;i<argc;++i)
        {
            Lang::Module::Handle hModule( Lang::Module::OpenFile(argv[1]) );
            Lang::Source         source(hModule);
            value = JSON::Value::LoadFrom(source);
        }
    }
    else
    {
        Lang::Module::Handle hModule( Lang::Module::OpenSTDIN() );
        Lang::Source         source(hModule);
        value = JSON::Value::LoadFrom(source);
    }
    std::cerr << "value=" << value << std::endl;
}
YOCTO_UNIT_TEST_DONE()
