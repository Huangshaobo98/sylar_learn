#include "../src/sylar.h"

static sylar::Logger::ptr g_logger = __LOG_ROOT;

void test_fiber() {
    __LOG_INFO(g_logger) << "test in fiber";
    sleep(1);
}

int main() {
    __LOG_INFO(g_logger) << "begin";
    sylar::Scheduler sc(3);
    
    sc.start();
    sc.schedule(&test_fiber);
    sc.stop();
    __LOG_INFO(g_logger) << "over";
    return 0;
}