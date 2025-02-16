#ifndef RAW_SOCKET_HPP
#define RAW_SOCKET_HPP

#include <vector>
#include <string>
#include <memory>
#include <algorithm>

#ifdef  __unix__
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#else
#include <winsock2.h>
#include <mstcpip.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#if  !defined(__MINGW32__) && !defined(__MINGW64__)
#pragma comment(lib, "ws2_32.lib") 
#endif
#endif

class RawSocket
{
public:
    std::vector<std::string>    protocols;
    std::vector<std::string>    addrs;
    std::vector<int>            ports;

    template<typename T>
    class Option
    {
    private:
        static void assign(std::vector<std::string>& dst, const std::vector<int>& src);
        static void assign(std::vector<std::string>& dst, const std::vector<std::string>& src);
    };

    RawSocket();
    ~RawSocket();

    std::vector<std::string> enable_addr_list() const;
    void set_addr(const std::string& addr_name);
    bool capture(std::string& raw_packet, std::string& meta, std::string& payload) const;

    RawSocket& add_filter();

private:
#ifdef __unix__
    int sock_;
#else
    SOCKET sock_;
#endif
    uint8_t buffer_[1024 * 20];
};

#endif // RAW_SOCKET_HPP