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
            const string         fileName = argv[1];
            Lang::Module::Handle hModule( new Lang::Module(fileName) );
            Lang::Source         source(hModule);
            value = JSON::Value::LoadFrom(source);
        }
    }
    else
    {
        Lang::Module::Handle hModule( new Lang::Module() );
        Lang::Source         source(hModule);
        value = JSON::Value::LoadFrom(source);
    }
}
YOCTO_UNIT_TEST_DONE()
