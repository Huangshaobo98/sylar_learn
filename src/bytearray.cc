#include "bytearray.h"
#include "endian.h"
#include "util.h"
#include "log.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <math.h>
#include <vector>
#include <cstring>
namespace sylar {
    static Logger::ptr g_logger = __LOG_NAME("system");
    ByteArray::Node::Node(size_t s) 
        :ptr(new char[s])
        ,size(s)
        ,next(nullptr){
    }
    ByteArray::Node::Node() 
        :ptr(nullptr)
        ,size(0)
        ,next(nullptr){
    }
    ByteArray::Node::~Node() {
        if(ptr) {
            delete [] ptr;
        }
    }
    ByteArray::ByteArray(size_t base_size) 
        :m_baseSize(base_size)
        ,m_capacity(base_size)
        ,m_position(0)
        ,m_size(0)
        ,m_endian(BIG_ENDIAN)
        ,m_root(new Node(base_size))
        ,m_cur(m_root)
        {
    }
    ByteArray::~ByteArray() {
        Node* tmp = m_root;
        while(tmp) {
            m_cur = tmp;
            tmp = tmp->next;
            delete m_cur;
        }
    }

    bool ByteArray::isLittleEndian() const {
        return m_endian == LITTLE_ENDIAN;
    }
    void ByteArray::setIsLittleEndian(bool val) {
        if(val) {
            m_endian = LITTLE_ENDIAN;
        } else {
            m_endian = BIG_ENDIAN;
        }
    }

    // write
    void ByteArray::writeFint8(int8_t value) {
        write(&value, sizeof(value));
    }
    void ByteArray::writeFuint8(uint8_t value) {
        write(&value, sizeof(value));
    }
    void ByteArray::writeFint16(int16_t value) {
        if(m_endian != BYTE_ORDER) {
            value = byteswap(value);
        }
        write(&value, sizeof(value));
    }
    void ByteArray::writeFuint16(uint16_t value) {
        if(m_endian != BYTE_ORDER) {
            value = byteswap(value);
        }
        write(&value, sizeof(value));
    }
    void ByteArray::writeFint32(int32_t value) {
        if(m_endian != BYTE_ORDER) {
            value = byteswap(value);
        }
        write(&value, sizeof(value));
    }
    void ByteArray::writeFuint32(uint32_t value) {
        if(m_endian != BYTE_ORDER) {
            value = byteswap(value);
        }
        write(&value, sizeof(value));
    }
    void ByteArray::writeFint64(int64_t value) {
        if(m_endian != BYTE_ORDER) {
            value = byteswap(value);
        }
        write(&value, sizeof(value));
    }
    void ByteArray::writeFuint64(uint64_t value) {
        if(m_endian != BYTE_ORDER) {
            value = byteswap(value);
        }
        write(&value, sizeof(value));
    }

    static uint32_t EncodeZigzag32(const int32_t& v) {
        if(v < 0) {
            return ((uint32_t) (-v)) * 2 - 1;
        } else {
            return v * 2;
        }
    }

    void ByteArray::writeInt32(int32_t value) {
        writeUint32(EncodeZigzag32(value));
    }
    void ByteArray::writeUint32(uint32_t value) {
        uint8_t tmp[5];
        uint8_t i = 0;
        while(value >= 0x80) {
            tmp[i++] = (value & 0x7F) | 0x80;
            value >>= 7;
        }
        tmp[i++] = value;
        write(tmp, i);
    }

    static uint64_t EncodeZigzag64(const int64_t& v) {
        if(v < 0) {
            return ((uint64_t) (-v)) * 2 - 1;
        } else {
            return v * 2;
        }
    }

    void ByteArray::writeInt64(int64_t value) {
        writeUint64(EncodeZigzag64(value));
    }
    void ByteArray::writeUint64(uint64_t value) {
        uint8_t tmp[10];
        uint8_t i = 0;
        while(value >= 0x80) {
            tmp[i++] = (value & 0x7F) | 0x80;
            value >>= 7;
        }
        tmp[i++] = value;
        write(tmp, i);
    }


    void ByteArray::writeFloat(float value) {
        uint32_t v;
        memcpy(&v, &value, sizeof(value));
        writeUint32(v);
    }
    void ByteArray::writeDouble(double value) {
        uint64_t v;
        memcpy(&v, &value, sizeof(value));
        writeUint64(v);
    }

    //length:int16, data
    void ByteArray::writeString16(const std::string& value) {
        writeFuint16(value.size());
        write(value.c_str(), value.size());
    }
    //length:int32, data
    void ByteArray::writeString32(const std::string& value) {
        writeFuint32(value.size());
        write(value.c_str(), value.size());
    }
    //length:int64, data
    void ByteArray::writeString64(const std::string& value) {
        writeFuint64(value.size());
        write(value.c_str(), value.size());
    }
    //length:Varint, data
    void ByteArray::writeStringVint(const std::string& value) {
        writeFuint64(value.size());
        write(value.c_str(), value.size());
    }
    //data
    void ByteArray::writeStringWithoutLength(const std::string& value) {
        write(value.c_str(), value.size());
    }

    //read
    int8_t ByteArray::readFint8() {
        int8_t value;
        read(&value, sizeof(value));
        return value;
    }
    uint8_t ByteArray::readFuint8() {
        uint8_t value;
        read(&value, sizeof(value));
        return value;
    }
    int16_t ByteArray::readFint16() {
        int16_t value;
        read(&value, sizeof(value));
        if(m_endian == BYTE_ORDER) {
            return value;
        }
        return byteswap(value);
    }
    uint16_t ByteArray::readFuint16() {
        uint16_t value;
        read(&value, sizeof(value));
        if(m_endian == BYTE_ORDER) {
            return value;
        }
        return byteswap(value);
    }
    int32_t ByteArray::readFint32() {
        int32_t value;
        read(&value, sizeof(value));
        if(m_endian == BYTE_ORDER) {
            return value;
        }
        return byteswap(value);
    }
    uint32_t ByteArray::readFuint32() {
        uint32_t value;
        read(&value, sizeof(value));
        if(m_endian == BYTE_ORDER) {
            return value;
        }
        return byteswap(value);
    }
    int64_t ByteArray::readFint64() {
        int64_t value;
        read(&value, sizeof(value));
        if(m_endian == BYTE_ORDER) {
            return value;
        }
        return byteswap(value);
    }
    uint64_t ByteArray::readFuint64() {
        uint64_t value;
        read(&value, sizeof(value));
        if(m_endian == BYTE_ORDER) {
            return value;
        }
        return byteswap(value);
    }

    static int32_t DecodeZigzag32(const uint32_t& value) {
        return (value >> 1) ^ -(value & 1);
    }

    int32_t ByteArray::readInt32(){
        return DecodeZigzag32(readUint32());
    }
    uint32_t ByteArray::readUint32() {
        uint32_t result = 0;
        for(int i = 0; i < 32; i += 7) {
            uint8_t b = readFuint8();
            if(b < 0x80) {
                result |= ((uint32_t)b) << i;
                break;
            } else {
                result |= (((uint32_t)(b & 0x7f)) << i);
            }
        }
        return result;
    }
    static int64_t DecodeZigzag64(const uint64_t& value) {
        return (value >> 1) ^ -(value & 1);
    }

    int64_t ByteArray::readInt64() {
        return DecodeZigzag64(readUint64());
    }
    uint64_t ByteArray::readUint64() {
        uint64_t result = 0;
        for(int i = 0; i < 64; i += 7) {
            uint8_t b = readFuint8();
            if(b < 0x80) {
                result |= ((uint64_t)b) << i;
                break;
            } else {
                result |= (((uint64_t)(b & 0x7f)) << i);
            }
        }
        return result;
    }

    float ByteArray::readFloat() {
        uint32_t v = readFuint32();
        float value;
        memcpy(&value, &v, sizeof(v));
        return value;
    }
    double ByteArray::readDouble() {
        uint64_t v = readFuint64();
        float value;
        memcpy(&value, &v, sizeof(v));
        return value;
    }

    std::string ByteArray::readString16() {
        uint16_t len = readFuint16();
        std::string buff(len, '\0');
        read(&buff[0], len);
        return buff;
    }
    std::string ByteArray::readString32() {
        uint32_t len = readFuint32();
        std::string buff(len, '\0');
        read(&buff[0], len);
        return buff;
    }
    std::string ByteArray::readString64() {
        uint64_t len = readFuint64();
        std::string buff(len, '\0');
        read(&buff[0], len);
        return buff;
    }
    std::string ByteArray::readStringVint() {
        uint64_t len = readFuint64();
        std::string buff(len, '\0');
        read(&buff[0], len);
        return buff;
    }

    // 内部操作
    void ByteArray::clear() {
        m_position = m_size = 0;
        m_capacity = m_baseSize;
        Node* tmp = m_root->next;
        while(tmp) {
            m_cur = tmp;
            tmp = tmp->next;
            delete m_cur;
        }
        m_cur = m_root;
        m_root->next = nullptr;
    }

    void ByteArray::write(const void* buf, size_t size) {
        if(size == 0) {
            return;
        }
        addCapacity(size);
        size_t npos = m_position % m_baseSize;  // 当前存储量对基量取余，表示第ncap节点已经存储的量
        size_t ncap = m_cur->size - npos;       // 当前节点的剩余容量
        size_t bpos = 0;                        // 当前写入的偏移量
        
        while(size > 0) {
            if(ncap >= size) {  //当前节点能够容下剩余容量
                memcpy(m_cur->ptr + npos, (char *)buf + bpos, size);
                if(m_cur->size == (npos + size)) {
                    m_cur = m_cur->next;
                }
                m_position += size;
                bpos += size;
                size = 0;
            } else {            // 当前节点不能容纳剩余size
                memcpy(m_cur->ptr + npos, (char *)buf + bpos, ncap);
                m_position += ncap;
                bpos += ncap;
                size -= ncap;
                m_cur = m_cur->next;
                ncap = m_cur->size;
                npos = 0;
            }
        }
        if(m_position > m_size) {
            m_size = m_position;
        }
    }
    void ByteArray::read(void* buf, size_t size) {
        if(size > getReadSize()){
            throw std::out_of_range("not enough length");
        }
        size_t npos = m_position % m_baseSize;  // 当前存储量对基量取余，表示第ncap节点已经存储的量
        size_t ncap = m_cur->size - npos;       // 当前节点的剩余容量
        size_t bpos = 0;                        // 当前读取的偏移量
        while(size > 0) {
            if(ncap >= size) {                                  // 当前节点剩余量超过size
                memcpy((char*)buf + bpos, m_cur->ptr + npos, size);    // 将m_cur节点npos偏移开始的size字节读入buf+bpos处
                if(m_cur->size == npos + size) {                // m_cur的大小如果等于当前节点的存储量加存储大小
                    m_cur = m_cur->next;                        // 移动到下一节点
                }
                m_position += size;                             
                bpos += size;                                   // 已读取量+size
                size = 0;                                       // 清空size
            } else {                                            // 当前节点剩余量小于size
                memcpy((char*)buf + bpos, m_cur->ptr + npos, ncap);    // 将m_cur节点npos偏移开始的ncap字节读入buf+bpos处
                m_position += ncap;                             
                bpos += ncap;
                size -= ncap;                                   // 剩余要读的长度-nacp
                m_cur = m_cur->next;                            // 移到下一节点
                ncap = m_cur->size;                             // 填充满下一节点的可读空间
                npos = 0;                                       // 当前存储量基量清空
            }
        }
    }

    void ByteArray::read(void* buf, size_t size, size_t position) const {
        if(size > (m_size - position)){
            throw std::out_of_range("not enough length");
        }
        size_t npos = position % m_baseSize;  // 当前存储量对基量取余，表示第ncap节点已经存储的量
        size_t ncap = m_cur->size - npos;       // 当前节点的剩余容量
        size_t bpos = 0;                        // 当前读取的偏移量
        Node* cur = m_cur;
        while(size > 0) {
            if(ncap >= size) {                                  // 当前节点剩余量超过size
                memcpy((char*)buf + bpos, cur->ptr + npos, size);    // 将m_cur节点npos偏移开始的size字节读入buf+bpos处
                if(cur->size == npos + size) {                // m_cur的大小如果等于当前节点的存储量加存储大小
                    cur = cur->next;                        // 移动到下一节点
                }
                position += size;                             
                bpos += size;                                   // 已读取量+size
                size = 0;                                       // 清空size
            } else {                                            // 当前节点剩余量小于size
                memcpy((char*)buf + bpos, cur->ptr + npos, ncap);    // 将m_cur节点npos偏移开始的ncap字节读入buf+bpos处
                position += ncap;                             
                bpos += ncap;
                size -= ncap;                                   // 剩余要读的长度-nacp
                cur = cur->next;                            // 移到下一节点
                ncap = cur->size;                             // 填充满下一节点的可读空间
                npos = 0;                                       // 当前存储量基量清空
            }
        }
    }

    void ByteArray::setPosition(size_t v) {
        if(v > m_size) {
            throw std::out_of_range("setPosition out of range");
        }
        m_position = v;
        if(m_position > m_size) {
            m_size = m_position;
        }
        m_cur = m_root;
        while(v > m_cur->size) {
            v -= m_cur->size;
            m_cur = m_cur->next;
        }
        if(v == m_cur->size) {
            m_cur = m_cur->next;
        }
    }

    bool ByteArray::writeToFile(const std::string& name) const {
        std::ofstream ofs;
        ofs.open(name, std::ios::trunc | std::ios::binary);
        if(!ofs) {
            __LOG_ERROR(g_logger) << "writeToFile name=" << name
                << " error , errno=" << errno << " errstr=" << strerror(errno);
            return false;
        }

        int64_t read_size = getReadSize();
        int64_t pos = m_position;
        Node* cur = m_cur;

        while(read_size > 0) {
            int diff = pos % m_baseSize;
            int64_t len = (read_size > (int64_t)m_baseSize ? m_baseSize : read_size) - diff;
            ofs.write(cur->ptr + diff, len);
            cur = cur->next;
            pos += len;
            read_size -= len;
        }

        return true;
    }
    bool ByteArray::readFromFile(const std::string& name) {
        std::ifstream ifs;
        ifs.open(name, std::ios::binary);
        if(!ifs) {
            __LOG_ERROR(g_logger) << "readFromFile name=" << name
                << " error, errno=" << errno << " errstr=" << strerror(errno);
            return false;
        }

        std::shared_ptr<char> buff(new char[m_baseSize], [](char* ptr) { delete[] ptr;});
        while(!ifs.eof()) {
            ifs.read(buff.get(), m_baseSize);
            write(buff.get(), ifs.gcount());
        }
        return true;

    }

    void ByteArray::addCapacity(size_t size) {
        if(size == 0) {
            return;
        }
        size_t old_cap = getCapacity();     // 剩余容量大于size，不需要在增加
        if(old_cap >= size) {
            return;
        }

        size = size - old_cap;
        size_t count = ceil(1.0 * size / m_baseSize);
        Node* tmp = m_root;
        while(tmp->next) {      // 指向最后一个节点
            tmp = tmp->next;
        }

        Node* first = NULL;
        for(size_t i = 0; i < count; ++i) {
            tmp->next = new Node(m_baseSize);
            if(first == NULL) {
                first = tmp->next;
            }
            tmp = tmp->next;
            m_capacity += m_baseSize;
        }

        if(old_cap == 0) {
            m_cur = first;
        }
    }

    std::string ByteArray::toString() const {
        std::string str;
        str.resize(getReadSize());
        if(str.empty()) {
            return str;
        }
        read(&str[0], str.size(), m_position);
        return str;
    }
    std::string ByteArray::toHexString() const {
        std::string str = toString();
        std::stringstream ss;
        for(size_t i = 0; i < str.size(); ++i) {
            if(i > 0 && i % 32 == 0) {
                ss << std::endl;
            }
            ss << std::setw(2) << std::setfill('0') << std::hex
               << (int)(uint8_t)str[i] << " ";
        }
        return ss.str();
    }
    uint64_t ByteArray::getReadBuffers(std::vector<iovec>& buffers, uint64_t len) {
        // 申请长度为len的iovec类型的buffer
        len = len > getReadSize() ? getReadSize() : len;
        if(len == 0) {
            return 0;
        }

        uint64_t size = len;

        size_t npos = m_position % m_baseSize;  // 取当前节点的pos
        size_t ncap = m_cur->size - npos;       // 取最后一个节点的容量
        struct iovec iov;
        Node* cur = m_cur;

        while(len > 0) {
            if(ncap >= len) {                   // 剩余容量大于len
                iov.iov_base = cur->ptr + npos;
                iov.iov_len = len;
                len = 0;
            } else {
                iov.iov_base = cur->ptr + npos;
                iov.iov_len = ncap;
                len -= ncap;
                cur = cur->next;
                ncap = cur->size;
                npos = 0;
            }
            buffers.push_back(iov);
        }
        return size;
    }
    uint64_t ByteArray::getReadBuffers(std::vector<iovec>& buffers, uint64_t len, uint64_t position) const {
        // 从指定位置获取iovec类型的buffer
        len = len > getReadSize() ? getReadSize() : len;
        if(len == 0) {
            return 0;
        }

        uint64_t size = len;

        size_t npos = position % m_baseSize;        // 取当前节点位置
        size_t count = position / m_baseSize;       // 取当前是第count个节点
        Node* cur = m_root;
        while(count > 0) {                          // 指向最后一个节点
            cur = cur->next;
            --count;
        }

        size_t ncap = cur->size - npos;
        struct iovec iov;
        while(len > 0) {
            if(ncap >= len) {
                iov.iov_base = cur->ptr + npos;
                iov.iov_len = len;
                len = 0;
            } else {
                iov.iov_base = cur->ptr + npos;
                iov.iov_len = ncap;
                len -= ncap;
                cur = cur->next;
                ncap = cur->size;
                npos = 0;
            }
            buffers.push_back(iov);
        }
        return size;
    }
    uint64_t ByteArray::getWriteBuffers(std::vector<iovec>& buffers, uint64_t len) {
        if(len == 0) {
            return 0;
        }
        addCapacity(len);
        uint64_t size = len;

        size_t npos = m_position % m_baseSize;
        size_t ncap = m_cur->size - npos;
        struct iovec iov;
        Node* cur = m_cur;
        while(len > 0) {
            if(ncap >= len) {
                iov.iov_base = cur->ptr + npos;
                iov.iov_len = len;
                len = 0;
            } else {
                iov.iov_base = cur->ptr + npos;
                iov.iov_len = ncap;

                len -= ncap;
                cur = cur->next;
                ncap = cur->size;
                npos = 0;
            }
            buffers.push_back(iov);
        }
        return size;
    }
}