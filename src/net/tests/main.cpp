#include "yocto/utest/driver.hpp"

YOCTO_UNIT_TEST_INIT(16)
{
    YOCTO_UNIT_TEST_DECL(get_host_name);
    YOCTO_UNIT_TEST_DECL(resolve);
    YOCTO_UNIT_TEST_DECL(tcp_server);
    YOCTO_UNIT_TEST_DECL(tcp_client);
    YOCTO_UNIT_TEST_DECL(udp_server);
    YOCTO_UNIT_TEST_DECL(udp_client);
    YOCTO_UNIT_TEST_DECL(tcp_multi);
    YOCTO_UNIT_TEST_DECL(io);
    YOCTO_UNIT_TEST_DECL(server);
    YOCTO_UNIT_TEST_DECL(client);
    YOCTO_UNIT_TEST_DECL(sizes);
}
YOCTO_UNIT_TEST_EXEC()
