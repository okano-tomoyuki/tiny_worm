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

template<typename T>
static T byte_swap(T value)
{
    uint8_t* first = (uint8_t*)&value;
    uint8_t* last  = first + sizeof(T);
    std::reverse(first, last);
    return value;        
}

static std::string get_error()
{
#ifdef __unix__
    return std::to_string(errno);
#else
    return std::to_string(WSAGetLastError());
#endif
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
    return std::vector<std::string>();
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

std::string RawSocket::capture() const
{
    auto ret            = std::string();
    const auto bytes    = recv(sock_, (char*)buffer_, sizeof(buffer_), 0);
    
    auto ntoa = [](const uint32_t addr)
    {
        return          std::to_string((addr >>  0) & 0xFF) 
                + "." + std::to_string((addr >>  8) & 0xFF) 
                + "." + std::to_string((addr >> 16) & 0xFF) 
                + "." + std::to_string((addr >> 24) & 0xFF);
    };

    if (bytes > 0)
    {
        if (bytes >= sizeof(IpHeader))
        {
            const auto ih       = (IpHeader*)((void*)&buffer_);
            const auto ihl      = (ih->vhl & 0x0f) * 4;
            const auto flags    = (byte_swap(ih->flag_offset) >> 13) & 0x07;
            const auto offset   = (byte_swap(ih->flag_offset) & 0x1FFF) * 8;

            ret += "IP Header:\n";
            ret += "  Version: " + std::to_string((ih->vhl >> 4) & 0x0F) + "\n";
            ret += "  Header Length: " + std::to_string(ihl) + " bytes \n";
            ret += "  Type of Service: " + std::to_string(ih->tos) + "\n";
            ret += "  Total Length: " + std::to_string(byte_swap(ih->len)) + " bytes \n";
            ret += "  Identification: " + std::to_string(byte_swap(ih->id)) + "\n";
            ret += "  Flags: " + std::to_string(flags) + "\n";
            ret += "  Fragment Offset: " + std::to_string(offset) + "\n";
            ret += "  Time to Live: " + std::to_string(ih->ttl) + "\n";
            ret += "  Protocol: " + std::string(PROTO_NAMES[ih->protocol]) + "\n";
            ret += "  Header Checksum: " + std::to_string(ih->checksum) + "\n";
            ret += "  Source Address: " + ntoa(ih->src_addr) + "\n";
            ret += "  Destination Address: " + ntoa(ih->dst_addr) + "\n";

            if (ih->protocol == IPPROTO_ICMP)
            {
                const auto icmp_header = (IcmpHeader*)((void*)&buffer_[ihl]);
                if (icmp_header->type == 8) // ICMP Echo Request
                {
                    return ret + std::string((char*)&buffer_[ihl + sizeof(IcmpHeader)], bytes - ihl - sizeof(IcmpHeader));
                }
                if (icmp_header->type == 0) // ICMP Echo Reply
                {
                    return ret + std::string((char*)&buffer_[ihl + sizeof(IcmpHeader)], bytes - ihl - sizeof(IcmpHeader));
                }
                if (icmp_header->type == 3) // ICMP Destination Unreachable
                {
                    return ret + std::string((char*)&buffer_[ihl + sizeof(IcmpHeader)], bytes - ihl - sizeof(IcmpHeader));
                }
                if (icmp_header->type == 11) // ICMP Time Exceeded
                {
                    return ret + std::string((char*)&buffer_[ihl + sizeof(IcmpHeader)], bytes - ihl - sizeof(IcmpHeader));
                }
                if (icmp_header->type == 12) // ICMP Parameter Problem
                {
                    return ret + std::string((char*)&buffer_[ihl + sizeof(IcmpHeader)], bytes - ihl - sizeof(IcmpHeader));
                }
                if (icmp_header->type == 13) // ICMP Redirect
                {
                    return ret + std::string((char*)&buffer_[ihl + sizeof(IcmpHeader)], bytes - ihl - sizeof(IcmpHeader));
                }
            }

            if (ih->protocol == IPPROTO_IGMP)
            {
                const auto igmp_header  = (IgmpHeader*)((void*)&buffer_[ihl]);
                const auto igmp_ver     = (igmp_header->type >> 4) & 0x0F;
                const auto igmp_type    = igmp_header->type & 0x0F;
                

                return "IGMP Header:\n  Version: " + std::to_string(igmp_ver) + "\n" + "  Type: " + std::to_string(igmp_type) + "\n";
            }

            if (ih->protocol == IPPROTO_TCP)
            {
                const auto tcp_header = (TcpHeader*)((void*)&buffer_[ihl]);
                if (tcp_header->dst_port == 80) // HTTP
                {
                    // 
                    return ret + std::string((char*)&buffer_[ihl + sizeof(TcpHeader)], bytes - ihl - sizeof(TcpHeader));
                }
            }

            if (ih->protocol == IPPROTO_UDP)
            {
                auto udp_header = (UdpHeader*)((void*)&buffer_[ihl]);
                if (udp_header->dst_port == 53) // DNS
                {
                    return ret + std::string((char*)&buffer_[ihl + sizeof(UdpHeader)], bytes - ihl - sizeof(UdpHeader));
                }
            }
        }

        return ret;
    }

    return ret;
}