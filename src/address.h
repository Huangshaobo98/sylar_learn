#ifndef __ADDRESS_H__
#define __ADDRESS_H__

#include <memory>
#include <string>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
namespace sylar {
    class Address {
        public:
            typedef std::shared_ptr<Address> ptr;
            virtual ~Address() {}

            int getFamily() const;

            virtual const sockaddr* getAddr() const = 0;
            virtual const socklen_t getAddrLen() const = 0;

            virtual std::ostream& insert(std::ostream& os) const = 0;
            std::string toString();

            bool operator<(const Address& rhs) const;
            bool operator==(const Address& rhs) const;
            bool operator!=(const Address& rhs) const;

    };

    class IPAddress : public Address {
        public:
            typedef std::shared_ptr<IPAddress> PTR;

            virtual IPAddress::ptr broadcastAddress(uint32_t prefix_len) = 0;
            virtual IPAddress::ptr networkAddress(uint32_t prefix_len) = 0;
            virtual IPAddress::ptr subnetMask(uint32_t prefix_len) = 0;

            virtual uint32_t getPort() const = 0;
            virtual void setPort(uint32_t v) = 0;
    };

    class IPv4Address : public IPAddress {
        public:
            typedef std::shared_ptr<IPv4Address>ptr;

            IPv4Address(uint32_t address = INADDR_ANY, uint32_t port = 0);
            const sockaddr* getAddr() const override;
            const socklen_t getAddrLen() const override;
            std::ostream& insert(std::ostream& os) const override;
            IPAddress::ptr broadcastAddress(uint32_t prefix_len) override;
            IPAddress::ptr networkAddress(uint32_t prefix_len) override;
            IPAddress::ptr subnetMask(uint32_t prefix_len) override;

            uint32_t getPort() const override;
            void setPort(uint32_t v) override;
        private:
            sockaddr_in m_addr;

    };
}

#endif