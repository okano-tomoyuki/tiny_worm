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
    RawSocket();
    ~RawSocket();

    std::vector<std::string> enable_addr_list() const;
    void set_addr(const std::string& addr_name);
    std::string capture() const;

private:
#ifdef __unix__
    int sock_;
#else
    SOCKET sock_;
#endif
    uint8_t buffer_[1024 * 20];
};

#endif // RAW_SOCKET_HPP