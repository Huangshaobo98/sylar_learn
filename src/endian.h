#ifndef __ENDIAN_H__
#define __ENDIAN_H__

#include <byteswap.h>
#include <stdint.h>

namespace sylar {

    template<typename T>
    typename std::enable_if<sizeof(T) == sizeof(uint64_t), T>::type
    byteswap(T value) {
        return (T)bswap_64((uint64_t)value);
    }

    template<typename T>
    typename std::enable_if<sizeof(T) == sizeof(uint32_t), T>::type
    byteswap(T value) {
        return (T)bswap_32((uint32_t)value);
    }

    template<typename T>
    typename std::enable_if<sizeof(T) == sizeof(uint16_t), T>::type
    byteswap(T value) {
        return (T)bswap_16((uint16_t)value);
    }

    #if BYTE_ORDER == BIG_ENDIAN

    template<typename T>
    T byteswapOnLittleEndian(T t) {
        return t;
    }

    template<typename T>
    T byteswapOnBigEndian(T t) {
        return byteswap(t);
    }

    #else
    template<typename T>
    T byteswapOnLittleEndian(T t) {
        return byteswap(t);
    }

    template<typename T>
    T byteswapOnBigEndian(T t) {
        return t;
    }
    #endif
}
#endif
