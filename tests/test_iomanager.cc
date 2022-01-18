#include "../src/sylar.h"
#include "../src/iomanager.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <sys/epoll.h>

sylar::Logger::ptr g_logger = __LOG_ROOT;

int sock = 0;

void test_fiber() {
    __LOG_INFO(g_logger) << "test_fiber sock=" << sock;

    //sleep(3);

    //close(sock);
    //sylar::IOManager::GetThis()->cancelAll(sock);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(443);
    inet_pton(AF_INET, "14.215.177.38", &addr.sin_addr.s_addr);

    if(!connect(sock, (const sockaddr*)&addr, sizeof(addr))) {
    } else if(errno == EINPROGRESS) {
        __LOG_INFO(g_logger) << "add event errno=" << errno << " " << strerror(errno);
        sylar::IOManager::GetThis()->addEvent(sock, sylar::IOManager::READ, [](){
            __LOG_INFO(g_logger) << "read callback";
        });
        sylar::IOManager::GetThis()->addEvent(sock, sylar::IOManager::WRITE, [](){
            __LOG_INFO(g_logger) << "write callback";
            //close(sock);
            sylar::IOManager::GetThis()->cancelEvent(sock, sylar::IOManager::READ);
            close(sock);
        });
    } else {
        __LOG_INFO(g_logger) << "else " << errno << " " << strerror(errno);
    }

}
sylar::Timer::ptr s_timer;
void test_timer() {
    sylar::IOManager iom(2);
    s_timer = iom.addTimer(1000, [](){
        static int i = 0;
        __LOG_INFO(g_logger) << "Hello Timer" << i;
        if(++i == 5) {
            s_timer->reset(2000, true);
        }
    }, true);
}

void test1() {
    std::cout << "EPOLLIN=" << EPOLLIN
              << " EPOLLOUT=" << EPOLLOUT << std::endl;
    sylar::IOManager iom(2);
    iom.schedule(&test_fiber);
}

int main(int argc, char** argv) {
    //test1();
    test_timer();
    return 0;
}
