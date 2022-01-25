#include "../src/http/http_server.h"
#include "../src/iomanager.h"
#include "../src/log.h"
sylar::Logger::ptr g_logger = __LOG_ROOT;

void run() {
    sylar::http::HttpServer::ptr server(new sylar::http::HttpServer);
    auto addr = sylar::IPAddress::LookupAny("0.0.0.0:8020");
    while(!server->bind(addr)) {
        sleep(2);
    }
    server->start();
}

int main(int argc, char** argv) {
    
    sylar::IOManager iom(2);
    iom.schedule(run);
    return 0;
}