#include "../src/socket.h"
#include "../src/sylar.h"
#include "../src/iomanager.h"
static sylar::Logger::ptr g_logger = __LOG_ROOT;


void test1() {
    sylar::IPAddress::ptr addr = sylar::Address::LookupAnyIPAddress("www.baidu.com");
    if(addr) {
        __LOG_INFO(g_logger) << "get address " << addr->toString();
    } else {
        __LOG_ERROR(g_logger) << "get address fail" ;
        return;
    }
    sylar::Socket::ptr sock = sylar::Socket::CreateTCP(addr);
    addr->setPort(80);
    if(!sock->connect(addr)) {
        __LOG_ERROR(g_logger) << "connect " << addr->toString() << " fail" ;
        return;
    } else {
        __LOG_INFO(g_logger) << "connected " << addr->toString();
    }
    const char buff[] = "GET / HTTP/1.0\r\n\r\n";
    int rt = sock->send(buff, sizeof(buff));
    if(rt <= 0) {
        __LOG_INFO(g_logger) << "send fail rt=" << rt;
        return;
    }
    std::string buffs;
    buffs.resize(4096);
    rt = sock->recv(&buffs[0], buffs.size());
    if(rt <= 0) {
        __LOG_INFO(g_logger) << "recv fail rt=" << rt;
        return;
    }
    buffs.resize(rt);
    __LOG_INFO(g_logger) << buffs;

}
void test_socket(){
    sylar::IOManager iom;
    iom.schedule(&test1);
}
int main() {
    test_socket();
    return 0;
}
