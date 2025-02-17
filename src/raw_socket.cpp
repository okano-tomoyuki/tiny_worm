#include <iostream>
#include "raw_socket.hpp"

constexpr char PROTO_NAMES[256][20] =     
{
    "HOPOPT",       "ICMP",         "IGMP",         "GGP",      
    "IP_IN_IP",     "ST",           "TCP",          "CBT", 
    "EGP",          "IGP",          "BBN_RCC_MON",  "NVP_II",   
    "PUP",          "ARGUS",        "EMCON",        "XNET", 
    "CHAOS",        "UDP",          "MUX",          "DCN_MEAS", 
    "HMP",          "PRM",          "XNS_IDP",      "TRUNK_1", 
    "TRUNK_2",      "LEAF_1",       "LEAF_2",       "RDP", 
    "IRTP",         "ISO_TP4",      "NETBLT",       "MFE_NSP", 
    "MERIT_INP",    "DCCP",         "THIRD_PC",     "IDPR", 
    "XTP",          "DDP",          "IDPR_CMTP",    "TPPP", 
    "IL",           "IPV6",         "SDRP",         "IPV6_ROUTE",   
    "IPV6_FRAG",    "IDRP",         "RSVP",         "GRES", 
    "DSR",          "BNA",          "ESP",          "AH", 
    "I_NLSP",       "SWIPE",        "NARP",         "MOBILE", 
    "TLSP",         "SKIP",         "IPV6_ICMP",    "IPV6_NO_NEXT", 
    "IPV6_OPT",     "OPT_INTERNAL", "CFTP",         "OPT_LOCAL", 
    "SAT_EXPAK",    "KRYPTOLAN",    "RVD",          "IPPC", 
    "OPT_FSYS",     "SAT_MON",      "VISA",         "IPCU", 
    "CPNX",         "CPHB",         "WSN",          "PVP", 
    "BR_SAT_MON",   "SUN_ND",       "WB_MON",       "WB_EXPAK", 
    "ISO_IP",       "VMTP",         "SECURE_VMTP",  "VINES", 
    "TTP",          "IPTM",         "NSFNET_IGP",   "DGP",
    "TCF",          "EIGRP",        "OSPF",         "SPRITE_RPC", 
    "LARP",         "MTP",          "AX_25",        "OS", 
    "MICP",         "SCC_SP",       "ETHERIP",      "ENCAP", 
    "" ,            "GMTP",         "IFMP",         "PNNI", 
    "PIM" ,         "ARIS",         "SCPS",         "QNX" , 
    "A_N" ,         "IP_COMP",      "SNP",          "COMPAQ_PEER", 
    "IPX_IN_IP",    "VRRP",         "PGM",          "OPT_0_HOP" ,   
    "L2TP",         "DDX",          "IATP",         "STP", 
    "SRP",          "UTI",          "SMP",          "SM",
    "PTP",          "IS_OVER_IPV4", "FIRE",         "CRTP", 
    "CRUDP" ,       "SSCOPMCE",     "IPLT",         "SPS", 
    "PIPE",         "SCTP",         "FC",           "RSVP_E2E_IGNORE", 
    "MOB_HEADER",   "UDP_LITE",     "MPLS_IN_IP",   "MANET", 
    "HIP",          "SHIM6",        "WESP",         "ROHC", 
    "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",
    "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",
    "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",
    "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",
    "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",
    "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",
    "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",
    "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",
    "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",
    "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",
    "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",
    "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",
    "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",
    "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",
    "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",
    "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",
    "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",
    "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",
    "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",
    "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",
    "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",
    "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",
    "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",
    "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",
    "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",
    "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",
    "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",     "UNKKNOWN",
    "UNKKNOWN",     "TEST_1",       "TEST_2",       "RESERVED",
};

#pragma pack(1)
struct IpHeader
{
    uint8_t     vhl;
    uint8_t     tos;
    uint16_t    len;
    uint16_t    id;
    uint16_t    flag_offset;
    uint8_t     ttl;
    uint8_t     protocol;
    uint16_t    checksum;
    uint32_t    src_addr;
    uint32_t    dst_addr;
};

struct IcmpHeader
{
    uint8_t     type;
    uint8_t     code;
    uint16_t    chksum;
    uint16_t    id;
    uint16_t    seq;
};

struct IgmpHeader
{
    uint8_t     type;
    uint8_t     code;
    uint16_t    chksum;
    uint32_t    addr;
};

struct TcpHeader
{
    uint16_t    src_port;
    uint16_t    dst_port;
    uint32_t    seq;
    int32_t     ack;
    uint8_t     offset;
    uint8_t     flags;
    uint16_t    window;
    uint16_t    checksum;
    uint16_t    urgent;
};

struct UdpHeader
{
    uint16_t    src_port;
    uint16_t    dst_port;
    uint16_t    len;
    uint16_t    checksum;
};
#pragma pack()

namespace 
{

template<typename T>
T byte_swap(T value)
{
    uint8_t* first = (uint8_t*)&value;
    uint8_t* last  = first + sizeof(T);
    std::reverse(first, last);
    return value;        
}

template<typename T>
std::string to_str(const T& v)
{
    return std::to_string(v);
}

std::string get_error()
{
#ifdef __unix__
    return std::to_string(errno);
#else
    return std::to_string(WSAGetLastError());
#endif
}

template<typename T, typename U>
bool is_target(const T& v, const std::vector<U>& list)
{
    return list.empty() || std::find(list.begin(), list.end(), v) != list.end();
}

}

RawSocket::RawSocket()
{
#ifdef __unix__
    sock_ = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sock_ < 0)
    {
        std::cout << "socket is error: " << get_error() << std::endl;
    }
#else
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
    {
        std::cout << "WSAStartup is error: " << get_error() << std::endl;
    }

    sock_ = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
    if (sock_ == INVALID_SOCKET)
    {
        std::cout << "socket is error: " << get_error() << std::endl;
    }
#endif
}

RawSocket::~RawSocket()
{
#ifdef __unix__
    close(sock_);
#else
    closesocket(sock_);
    WSACleanup();
#endif
}

std::vector<std::string> RawSocket::enable_addr_list() const
{
#ifdef __unix__
    struct ifaddrs *ifap;
    char addr[INET_ADDRSTRLEN];

    if (getifaddrs(&ifap) != 0) 
    {
        std::cout << "getifaddrs error" << get_error() << std::endl;
    }

    auto ret    = std::vector<std::string>();
    for (auto ifa = ifap; ifa != nullptr; ifa = ifa->ifa_next) 
    {
        if (ifa->ifa_addr->sa_family == AF_INET) 
        {
            auto sa = (sockaddr_in*)(ifa->ifa_addr);
            inet_ntop(AF_INET, &(sa->sin_addr), addr, INET_ADDRSTRLEN);

            if (std::string(ifa->ifa_name) != "lo")
            {
                ret.push_back(addr);
            }
        }
    }
    freeifaddrs(ifap);

    return ret;
    
#else
    char buffer[1024];
    auto bytes  = DWORD{0};
    
    if (WSAIoctl(sock_, SIO_ADDRESS_LIST_QUERY, NULL, 0, buffer, sizeof(buffer), &bytes, NULL, NULL) != 0)
    {
        std::cout << "WSAIoctrl is error: " << get_error() << std::endl;
    }

    auto list   = (SOCKET_ADDRESS_LIST*)buffer;
    auto ret    = std::vector<std::string>();
    for (auto i = 0; i < list->iAddressCount; i++)
    {
        ret.push_back(inet_ntoa(((sockaddr_in*)list->Address[i].lpSockaddr)->sin_addr));
    }
    
    return ret;

    // auto ret    = std::vector<std::string>();  
    // ULONG size = 0;
    // GetAdaptersAddresses(AF_INET, 0, NULL, NULL, &size);

    // char buffer[size];
    // if (GetAdaptersAddresses(AF_INET, 0, NULL, (IP_ADAPTER_ADDRESSES*)&buffer, &size) == NO_ERROR) 
    // {
    //     for (auto adapter = (IP_ADAPTER_ADDRESSES*)&buffer; adapter; adapter = adapter->Next) 
    //     {
    //         for (auto addr = adapter->FirstUnicastAddress; addr; addr = addr->Next) 
    //         {
    //             if (addr->Address.lpSockaddr->sa_family == AF_INET) 
    //             {
    //                 char ipStr[INET_ADDRSTRLEN] = {0};
    //                 sockaddr_in* sa_in = (sockaddr_in*)addr->Address.lpSockaddr;
    //                 inet_ntop(AF_INET, &sa_in->sin_addr, ipStr, sizeof(ipStr));
    //                 ret.push_back(ipStr);
    //             }
    //         }
    //     }
    // }
    // return ret;
#endif
}

void RawSocket::set_addr(const std::string& addr_name)
{
    auto addr                   = sockaddr_in{};
    inet_pton(AF_INET, addr_name.c_str(), &addr.sin_addr);
    addr.sin_family             = AF_INET;
    addr.sin_port               = htons(0);

    if (bind(sock_, (struct sockaddr*)&addr, sizeof(addr)) != 0)
    {
        std::cout << "bind is error: " << get_error() << std::endl;
    }

#ifndef __unix__
    auto recv_opt = ULONG{RCVALL_ON};
    auto bytes    = DWORD{0};
    if (WSAIoctl(sock_, SIO_RCVALL, &recv_opt, sizeof(recv_opt), NULL, 0, &bytes, NULL, NULL) != 0)
    {
        std::cout << "WSAIoctrl is error: " << get_error() << std::endl;
    }
#endif
}

bool RawSocket::capture(std::string& raw_packet, std::string& meta, std::string& payload) const
{
    raw_packet.clear();
    meta.clear();
    payload.clear();
    const auto bytes    = recv(sock_, (char*)buffer_, sizeof(buffer_), 0);
    
    auto ntoa = [](const uint32_t addr)
    {
        return          to_str((addr >>  0) & 0xFF) 
                + "." + to_str((addr >>  8) & 0xFF) 
                + "." + to_str((addr >> 16) & 0xFF) 
                + "." + to_str((addr >> 24) & 0xFF);
    };

    if (bytes > 0)
    {
        if (bytes >= sizeof(IpHeader))
        {
            raw_packet          = std::string((char*)&buffer_, bytes);
            const auto ih       = (IpHeader*)&buffer_;
            const auto ihl      = (ih->vhl & 0x0f) * 4;
            const auto flags    = (byte_swap(ih->flag_offset) >> 13) & 0x07;
            const auto offset   = (byte_swap(ih->flag_offset) & 0x1FFF) * 8;
            const auto src_addr = ntoa(ih->src_addr);
            const auto dst_addr = ntoa(ih->dst_addr);
            const auto protocol = std::string(PROTO_NAMES[ih->protocol]);

            if (!is_target(src_addr, addrs) && !is_target(dst_addr, addrs))
            {
                return false;
            }

            meta += "IP Header:\n";
            meta += "  Version              : " + to_str((ih->vhl >> 4) & 0x0F) + "\n";
            meta += "  Header Length        : " + to_str(ihl) + " bytes \n";
            meta += "  Type of Service      : " + to_str(ih->tos) + "\n";
            meta += "  Total Length         : " + to_str(byte_swap(ih->len)) + " bytes \n";
            meta += "  Identification       : " + to_str(byte_swap(ih->id)) + "\n";
            meta += "  Flags                : " + to_str(flags) + "\n";
            meta += "  Fragment Offset      : " + to_str(offset) + "\n";
            meta += "  Time to Live         : " + to_str(ih->ttl) + "\n";
            meta += "  Protocol             : " + protocol + "\n";
            meta += "  Header Checksum      : " + to_str(ih->checksum) + "\n";
            meta += "  Source Address       : " + src_addr + "\n";
            meta += "  Destination Address  : " + dst_addr + "\n";

            if (ih->protocol == IPPROTO_ICMP && is_target(protocol, protocols))
            {
                const auto icmp_header  = (IcmpHeader*)&buffer_[ihl];
                return true;
            }

            if (ih->protocol == IPPROTO_IGMP && is_target(protocol, protocols))
            {
                const auto igmp_header  = (IgmpHeader*)&buffer_[ihl];
                const auto igmp_ver     = (igmp_header->type >> 4) & 0x0F;
                const auto igmp_type    = igmp_header->type & 0x0F;

                return true;
            }

            if (ih->protocol == IPPROTO_TCP && is_target(protocol, protocols))
            {
                const auto tcp_header   = (TcpHeader*)&buffer_[ihl];
                const auto src_port     = byte_swap(tcp_header->src_port);
                const auto dst_port     = byte_swap(tcp_header->dst_port);

                if (!is_target(src_port, ports) && is_target(dst_port, ports))
                {
                    raw_packet.clear();
                    meta.clear();
                    return false;
                }
                payload = std::string((char*)&buffer_[ihl + sizeof(TcpHeader)], bytes - ihl - sizeof(TcpHeader));

                return true;
            }

            if (ih->protocol == IPPROTO_UDP && is_target(protocol, protocols))
            {
                const auto udp_header   = (UdpHeader*)&buffer_[ihl];
                const auto src_port     = byte_swap(udp_header->src_port);
                const auto dst_port     = byte_swap(udp_header->dst_port);
                const auto len          = byte_swap(udp_header->len);

                if (!is_target(src_port, ports) && is_target(dst_port, ports))
                {
                    raw_packet.clear();
                    meta.clear();
                    return false;
                }

                meta += "UDP Header:\n";
                meta += "  Source Port          : " + to_str(src_port)   + "\n";
                meta += "  Destination Port     : " + to_str(dst_port)   + "\n";
                meta += "  Length               : " + to_str(len)        + "\n";
                payload = std::string((char*)&buffer_[ihl + sizeof(UdpHeader)], len);

                return true;
            }
        }
    }

    return false;
}