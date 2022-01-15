#include "../src/sylar.h"
#include <assert.h>

sylar::Logger::ptr g_logger = __LOG_ROOT;

void test_assert() {
    __LOG_INFO(g_logger) << sylar::BacktraceToString(10);
    __ASSERT2(0==1, "0 is 1");
}
int main(int argc, char** argv) {
    test_assert();
    return 0;
}