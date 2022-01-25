#include "../src/tcp_server.h"
#include "../src/iomanager.h"
#include "../src/log.h"
sylar::Logger::ptr g_logger = __LOG_ROOT;

void run() {
    auto addr = sylar::IPAddress::LookupAny("0.0.0.0:8083");
    auto addr2 = sylar::UnixAddress::ptr (new sylar::UnixAddress("/tmp/unix_addr"));
    std::vector<sylar::Address::ptr> addrs;
    __LOG_INFO(g_logger) << *addr;
    __LOG_INFO(g_logger) << *addr2;
    addrs.push_back(addr);
    addrs.push_back(addr2);
    sylar::TcpServer::ptr tcp_server(new sylar::TcpServer);
    std::vector<sylar::Address::ptr> fails;
    while(!tcp_server->bind(addrs, fails)) {
        sleep(2);
    }
    tcp_server->start();
}

int main(int argc, char** argv) {
    
    sylar::IOManager iom(2);
    iom.schedule(run);
    return 0;
}