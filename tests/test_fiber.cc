#include "../src/sylar.h"

sylar::Logger::ptr g_logger = __LOG_ROOT;

void run_in_fiber() {
    __LOG_INFO(g_logger) << "run_in_fiber begin";
    sylar::Fiber::GetThis()->YieldToHold();
    __LOG_INFO(g_logger) << "run_in_fiber end";
    sylar::Fiber::GetThis()->YieldToHold();
}

void test_fiber() {
    sylar::Fiber::GetThis();
    __LOG_INFO(g_logger) << "main begin";
    {
        // 创建了一个新的协程对象，其中封装着run_in_fiber方法
        sylar::Fiber::ptr fiber(new sylar::Fiber(run_in_fiber));
        fiber->swapIn();
        __LOG_INFO(g_logger) << "main after run_in_fiber begin";
        fiber->swapIn();
        __LOG_INFO(g_logger) << "main after run_in_fiber end";
        fiber->swapIn();
    }
    __LOG_INFO(g_logger) << "main after run_in_fiber end2";
}
int main(int argc, char** argv) {
    sylar::Thread::SetName("main");
    std::vector<sylar::Thread::ptr> thrs;
    for(int i = 0; i < 3; ++ i) {
        thrs.push_back(sylar::Thread::ptr(new sylar::Thread(&test_fiber, "name_" + std::to_string(i))));
    }

    for(auto& i : thrs) {
        i->join();
    }

    return 0;
} 