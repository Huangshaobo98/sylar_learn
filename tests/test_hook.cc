#include "../src/hook.h"
#include "../src/iomanager.h"
#include "../src/sylar.h"
sylar::Logger::ptr g_logger = __LOG_ROOT;

void test_sleep() {
    sylar::IOManager iom(1);
    iom.schedule([](){
        sleep(2);
        __LOG_INFO(g_logger) << "sleep 2";
    });

    iom.schedule([](){
        sleep(3);
        __LOG_INFO(g_logger) << "sleep 3";
    });
    __LOG_INFO(g_logger) << "test sleep";
}

int main() {
    test_sleep();
    return 0;
}