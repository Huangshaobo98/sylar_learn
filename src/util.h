/*
 * @Description: 
 * @Version: 1.0
 * @Autor: huang
 * @Date: 2021-12-30 22:57:03
 * @LastEditors: huang
 * @LastEditTime: 2021-12-30 23:04:09
 */
#ifndef __UTIL_H__
#define __UTIL_H__

#include <string>
#include <fstream>
#include <unistd.h>
#include <stdint.h>
#include <vector>
#include <stdarg.h>
namespace sylar {

    /**
     * @brief 获取线程ID
     * 
     * @return pid_t 
     */
    pid_t GetThreadId();

    /**
     * @brief 获取协程ID
     * 
     * @return uint32_t 
     */
    uint32_t GetFiberId();
    
    uint64_t GetCurrentMS();

    uint64_t GetCurrentUS();

    void Backtrace(std::vector<std::string>& bt, int size, int skip);

    std::string BacktraceToString(int size = 64, int skip = 2, const std::string& prefix = "");
    
    /**
     * @brief 处理文件系统的方法
     */
    class FSUtil{
        public:
            // 获取文件所在目录
            static std::string Dirname(const std::string& filename);
            // 逐级创建目录
            static bool Mkdir(const std::string& dirname);
            // 以读方式打开目录
            static bool OpenForRead(std::ifstream& ifs, const std::string& filename
                    ,std::ios_base::openmode mode);
            // 以写方式打开目录
            static bool OpenForWrite(std::ofstream& ofs, const std::string& filename
                    ,std::ios_base::openmode mode);
            static bool Unlink(const std::string& filename, bool exist = false);
    };
    void toLower(std::string& str);
    void toUpper(std::string& str);
    class StringUtil {
        public:
            static std::string Format(const char* fmt, ...);
            static std::string Formatv(const char* fmt, va_list ap);

            static std::string UrlEncode(const std::string& str, bool space_as_plus = true);
            static std::string UrlDecode(const std::string& str, bool space_as_plus = true);

            static std::string Trim(const std::string& str, const std::string& delimit = " \t\r\n");
            static std::string TrimLeft(const std::string& str, const std::string& delimit = " \t\r\n");
            static std::string TrimRight(const std::string& str, const std::string& delimit = " \t\r\n");


            static std::string WStringToString(const std::wstring& ws);
            static std::wstring StringToWString(const std::string& s);
    };
    std::string Time2Str(time_t ts = time(0), const std::string& format = "%Y-%m-%d %H:%M:%S");
}

#endif
