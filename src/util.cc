#include "util.h"
#include "log.h"
#include "fiber.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <fstream>
#include <sys/syscall.h>
#include <execinfo.h>
#include <sstream>
#include <iostream>
namespace sylar {
    
    Logger::ptr g_logger = __LOG_NAME("system");

    pid_t GetThreadId() {
        return syscall(SYS_gettid);
    }

    uint32_t GetFiberId() {
        return sylar::Fiber::GetFiberId();
    }

    void Backtrace(std::vector<std::string>& bt, int size, int skip) {
        void **array = (void**) malloc((sizeof(void*) * size));
        // backtrace用来追踪堆栈上的函数调用地址，并将地址保存在array中。
        // size表示所追踪的层数，返回值表示得到了几层地址
        size_t s = ::backtrace(array, size);

        // 将上面的backtrace得到的地址翻译为一串string，返回为null时表示翻译失败
        char** strings = backtrace_symbols(array, s);
        if(strings == NULL) {
            __LOG_ERROR(g_logger) << "backtrace_symbols error";
            return;
        }
        for(size_t i = skip; i < s; ++i) {
            bt.push_back(strings[i]);
        }
        free(strings);
        free(array);
    }

    std::string BacktraceToString(int size, int skip, const std::string& prefix) {
        std::vector<std::string> bt;
        Backtrace(bt, size, skip);
        std::stringstream ss;
        for(size_t i = 0; i < bt.size(); ++i) {
            ss << prefix << bt[i] << std::endl;
        }
        return ss.str();
    }

    /**
     * @brief 安全获取stat结构体
     * @param[in] file 路径名
     * @param[in/out] st stat结构体指针
     * @return int 0 表示正确打开 -1 表示打开失败
     */
    static int __lstat(const char* file, struct stat* st = nullptr) {
        struct stat lst;
        int ret = lstat(file, &lst);
        //通过文件名file获取信息，保存在lst指向的变量中
        if(st) {
            *st = lst;
        }
        return ret;
    }

    static int __mkdir(const char* dirname){
        if(access(dirname, F_OK) == 0){
            //检查目标文件是否存在
            return 0;
        }
        return mkdir(dirname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        //权限 rwx|rwx|r_x，正确创建目录返回0，否则为-1
    }

    bool FSUtil::Mkdir(const std::string& dirname) {
        if(__lstat(dirname.c_str()) == 0){
            // 目录已存在
            return 0;
        }
        char* path = strdup(dirname.c_str());
        // 拷贝dirname到path，需要free
        char* ptr = strchr(path + 1, '/');
        // 从path指针+1处寻找'/'，找到后保存在ptr中，否则返回null
        do {
            while(ptr) {
                /**
                 * @detail 尝试逐级创建目录，例如first/second/third
                 * @首先创建 first'\0'second/third
                 * @其次创建 first/second'\0'third
                 * @最后创建 first/secpmd/third
                 */
                *ptr = '\0';
                if(__mkdir(path) != 0) {
                    // 创建目录失败，跳出循环，返回错误
                    break;
                }
                *ptr = '/';
                ptr = strchr(ptr + 1, '/');
            }
            if(ptr != nullptr) {
                // 创建目录失败
                break;
            } else if (__mkdir(path) != 0) {
                // 创建最后一级目录失败
                break;
            }
            free(path);
            return true;
        } while(0);    
        free(path);
        return true;
    }

    std::string FSUtil::Dirname(const std::string& filename) {
        if(filename.empty()){
            //位于当前目录，直接返回当前目录信息
            return ".";
        }
        auto pos = filename.rfind('/'); // 找到上一级目录
        if(pos == 0){
            // 位于根目录
            return "/";
        } else if (pos == std::string::npos) {
            // 找不到/，则返回当前目录
            return ".";
        } else {
            return filename.substr(0, pos);
        }
    }

    bool FSUtil::OpenForRead(std::ifstream& ifs, const std::string& filename
            ,std::ios_base::openmode mode) {
        ifs.open(filename.c_str(), mode);
        return ifs.is_open();
    }

    bool FSUtil::OpenForWrite(std::ofstream& ofs, const std::string& filename
            ,std::ios_base::openmode mode) {
        ofs.open(filename.c_str(), mode);
        if(!ofs.is_open()) {
            std::string dir = Dirname(filename);
            Mkdir(dir);
            ofs.open(filename.c_str(), mode);
        }
        return ofs.is_open();
    }
    void toLower(std::string& str) {
        for(auto& chr : str) {
            chr = (chr >= 'A' && chr <= 'Z') ? (chr - 'A' + 'a') : chr;
        }
    }
    void toUpper(std::string& str) {
        for(auto& chr : str) {
            chr = (chr >= 'a' && chr <= 'z') ? (chr - 'a' + 'A') : chr;
        }
    }
}
