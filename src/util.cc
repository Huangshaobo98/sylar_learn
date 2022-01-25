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
#include <sys/time.h>
namespace sylar {
    
    Logger::ptr g_logger = __LOG_NAME("system");

    pid_t GetThreadId() {
        return syscall(SYS_gettid);
    }

    uint32_t GetFiberId() {
        return sylar::Fiber::GetFiberId();
    }

    uint64_t GetCurrentMS() {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec * 1000 + tv.tv_usec / 1000;
    }

    uint64_t GetCurrentUS() {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec * 1000 * 1000ul + tv.tv_usec;
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
    bool FSUtil::Unlink(const std::string& filename, bool exist) {
        if(!exist && __lstat(filename.c_str())) {
            return true;
        }
        return ::unlink(filename.c_str()) == 0;
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
    std::string StringUtil::Format(const char* fmt, ...) {
        va_list ap;
        va_start(ap, fmt);
        auto v = Formatv(fmt, ap);
        va_end(ap);
        return v;
    }

    std::string StringUtil::Formatv(const char* fmt, va_list ap) {
        char* buf = nullptr;
        auto len = vasprintf(&buf, fmt, ap);
        if(len == -1) {
            return "";
        }
        std::string ret(buf, len);
        free(buf);
        return ret;
    }

    static const char uri_chars[256] = {
        /* 0 */
        0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 1, 1, 0,
        1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 0, 0, 0, 1, 0, 0,
        /* 64 */
        0, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 0, 0, 0, 0, 1,
        0, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 0, 0, 0, 1, 0,
        /* 128 */
        0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
        /* 192 */
        0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
    };

    static const char xdigit_chars[256] = {
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,1,2,3,4,5,6,7,8,9,0,0,0,0,0,0,
        0,10,11,12,13,14,15,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,10,11,12,13,14,15,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    };

    #define CHAR_IS_UNRESERVED(c)           \
        (uri_chars[(unsigned char)(c)])

    //-.0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz~
    std::string StringUtil::UrlEncode(const std::string& str, bool space_as_plus) {
        static const char *hexdigits = "0123456789ABCDEF";
        std::string* ss = nullptr;
        const char* end = str.c_str() + str.length();
        for(const char* c = str.c_str() ; c < end; ++c) {
            if(!CHAR_IS_UNRESERVED(*c)) {
                if(!ss) {
                    ss = new std::string;
                    ss->reserve(str.size() * 1.2);
                    ss->append(str.c_str(), c - str.c_str());
                }
                if(*c == ' ' && space_as_plus) {
                    ss->append(1, '+');
                } else {
                    ss->append(1, '%');
                    ss->append(1, hexdigits[(uint8_t)*c >> 4]);
                    ss->append(1, hexdigits[*c & 0xf]);
                }
            } else if(ss) {
                ss->append(1, *c);
            }
        }
        if(!ss) {
            return str;
        } else {
            std::string rt = *ss;
            delete ss;
            return rt;
        }
    }

    std::string StringUtil::UrlDecode(const std::string& str, bool space_as_plus) {
        std::string* ss = nullptr;
        const char* end = str.c_str() + str.length();
        for(const char* c = str.c_str(); c < end; ++c) {
            if(*c == '+' && space_as_plus) {
                if(!ss) {
                    ss = new std::string;
                    ss->append(str.c_str(), c - str.c_str());
                }
                ss->append(1, ' ');
            } else if(*c == '%' && (c + 2) < end
                        && isxdigit(*(c + 1)) && isxdigit(*(c + 2))){
                if(!ss) {
                    ss = new std::string;
                    ss->append(str.c_str(), c - str.c_str());
                }
                ss->append(1, (char)(xdigit_chars[(int)*(c + 1)] << 4 | xdigit_chars[(int)*(c + 2)]));
                c += 2;
            } else if(ss) {
                ss->append(1, *c);
            }
        }
        if(!ss) {
            return str;
        } else {
            std::string rt = *ss;
            delete ss;
            return rt;
        }
    }

    std::string StringUtil::Trim(const std::string& str, const std::string& delimit) {
        auto begin = str.find_first_not_of(delimit);
        if(begin == std::string::npos) {
            return "";
        }
        auto end = str.find_last_not_of(delimit);
        return str.substr(begin, end - begin + 1);
    }

    std::string StringUtil::TrimLeft(const std::string& str, const std::string& delimit) {
        auto begin = str.find_first_not_of(delimit);
        if(begin == std::string::npos) {
            return "";
        }
        return str.substr(begin);
    }

    std::string StringUtil::TrimRight(const std::string& str, const std::string& delimit) {
        auto end = str.find_last_not_of(delimit);
        if(end == std::string::npos) {
            return "";
        }
        return str.substr(0, end);
    }

    std::string StringUtil::WStringToString(const std::wstring& ws) {
        std::string str_locale = setlocale(LC_ALL, "");
        const wchar_t* wch_src = ws.c_str();
        size_t n_dest_size = wcstombs(NULL, wch_src, 0) + 1;
        char *ch_dest = new char[n_dest_size];
        memset(ch_dest,0,n_dest_size);
        wcstombs(ch_dest,wch_src,n_dest_size);
        std::string str_result = ch_dest;
        delete []ch_dest;
        setlocale(LC_ALL, str_locale.c_str());
        return str_result;
    }

    std::wstring StringUtil::StringToWString(const std::string& s) {
        std::string str_locale = setlocale(LC_ALL, "");
        const char* chSrc = s.c_str();
        size_t n_dest_size = mbstowcs(NULL, chSrc, 0) + 1;
        wchar_t* wch_dest = new wchar_t[n_dest_size];
        wmemset(wch_dest, 0, n_dest_size);
        mbstowcs(wch_dest,chSrc,n_dest_size);
        std::wstring wstr_result = wch_dest;
        delete []wch_dest;
        setlocale(LC_ALL, str_locale.c_str());
        return wstr_result;
    }
    std::string Time2Str(time_t ts, const std::string& format) {
        struct tm tm;
        localtime_r(&ts, &tm);
        char buf[64];
        strftime(buf, sizeof(buf), format.c_str(), &tm);
        return buf;
    }
}
