#include "../src/sylar.h"


sylar::Logger::ptr g_logger = __LOG_ROOT;
int count = 0;
//sylar::RWMutex s_mutex;
sylar::Mutex s_mutex;
void func1() {
    __LOG_INFO(g_logger) << "name: " << sylar::Thread::GetName()
                         << " this.name: " << sylar::Thread::GetThis()->getName()
                         << " id:" << sylar::GetThreadId()
                         << " this.id:" << sylar::Thread::GetThis()->getId();
    //sleep(1);
    
    for(int i = 0; i < 10000000; ++i) {
        //
        sylar::Mutex::Lock lock(s_mutex);
        ++count;
    }
}

void func2() {
    int i = 1000;
    while(i--) {
        __LOG_INFO(g_logger) << "+++++++++++++++++++++++++++++++++++";
    }
}

void func3() {
    int i = 1000;
    while(i--) {
        __LOG_INFO(g_logger) << "-----------------------------------";
    }
}

void test_mutex() {
    YAML::Node root = YAML::LoadFile("../log1.yml");
    sylar::Config::LoadFromYaml(root);
    //std::vector<sylar::Thread::ptr> thrs;
    sylar::Thread::ptr thr1(new sylar::Thread(&func2, "name_1"));
    sylar::Thread::ptr thr2(new sylar::Thread(&func3, "name_2"));
    thr1->join();
    thr2->join();
}

void test_thread() {
    __LOG_INFO(g_logger) << "thread test begin";
    std::vector<sylar::Thread::ptr> thrs;
    for(int i = 0; i < 5; ++i) {
        sylar::Thread::ptr thr(new sylar::Thread(&func1, "name_" + std::to_string(i)));
        thrs.push_back(thr); 
    }
    for(int i = 0; i < 5; ++i) {
        thrs[i]->join();
    }
    __LOG_INFO(g_logger) << "thread test end";
    __LOG_INFO(g_logger) << "count=" << count;
}
int main(int argc, char** argv) {
    test_mutex();
    return 0;
}