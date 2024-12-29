#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cstring>

#ifdef __unix__
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#else
#include <winsock2.h>
// #include <mstcpip.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#pragma comment(lib, "iphlpapi.lib")
#endif

#define ip_v ip_vhl >> 4
#define ip_hl ip_vhl & 0x0F
#define IP_DF 0x4000			   // dont fragment flag
#define IP_MF 0x2000			   // more fragment flag
#define IP_OFFMASK 0x1fff		   // mask for fragmenting bits
#define th_hl th_hlr >> 4
#define th_flags th_rfl & 0x3F

typedef struct IP_HEADER
{
	uint8_t ip_vhl; 				// version and header length
	uint8_t ip_tos;				   	// type of service
	uint16_t ip_len;			   	// total length
	uint16_t ip_id;				   	// identification
	uint16_t ip_off;			   	// fragment offset field
	uint8_t ip_ttl;				   	// time to live
	uint8_t ip_p;				   	// protocol
	uint16_t ip_sum;			   	// checksum
	struct in_addr ip_src;			// source address
	struct in_addr ip_dst; 			// destination address
} IP_HEADER, *PIP_HEADER;

typedef struct TCP_HEADER
{
	uint16_t th_sport; // 16-bit source port number
	uint16_t th_dport; // 16-bit destination port number
	uint32_t th_seq;   // 32-bit sequence number
	uint32_t th_ack;   // 32-bit acknowledgement number
	uint8_t th_hlr;	   // header length and reserved
	uint8_t th_rfl;	   // reserved and flags
	uint16_t th_win; // 16-bit window size
	uint16_t th_sum; // 16-bit TCP checksum
	uint16_t th_urp; // 16-bit urgent offset
} TCP_HEADER, *PTCP_HEADER;

typedef struct MY_TCPROW
{
	DWORD dwState;
	in_addr LocalAddr;
	SHORT sLocalPort;
	in_addr RemoteAddr;
	SHORT sRemotePort;
} MY_TCPROW, *PMY_TCPROW;

void PrintHexDump(DWORD length, PBYTE buffer);
VOID PrintTCPRow(PMIB_TCPROW pTcpRow);
// VOID PrintAddrInfo(addrinfo* pAddrInfo);
VOID PrintIPHeader(IP_HEADER ip_header);

void PrintHexDump(DWORD length, PBYTE buffer)
{
	DWORD i, count, index;
	CHAR rgbDigits[] = "0123456789abcdef";
	CHAR rgbLine[100];
	char cbLine;

	for (index = 0; length; length -= count, buffer += count, index += count)
	{
		count = (length > 16) ? 16 : length;

		sprintf_s(rgbLine, sizeof(rgbLine), "%4.4x ", index);
		cbLine = 5;

		for (i = 0; i < count; i++)
		{
			rgbLine[cbLine++] = rgbDigits[buffer[i] >> 4];
			rgbLine[cbLine++] = rgbDigits[buffer[i] & 0x0f];
			if (i == 7)
			{
				rgbLine[cbLine++] = ':';
			}
			else
			{
				rgbLine[cbLine++] = ' ';
			}
		}

		for (; i < 16; i++)
		{
			rgbLine[cbLine++] = ' ';
			rgbLine[cbLine++] = ' ';
			rgbLine[cbLine++] = ' ';
		}

		rgbLine[cbLine++] = ' ';

		for (i = 0; i < count; i++)
		{
			if (buffer[i] < 32 || buffer[i] > 126)
			{
				rgbLine[cbLine++] = '.';
			}
			else
			{
				rgbLine[cbLine++] = buffer[i];
			}
		}
		rgbLine[cbLine++] = 0;
		printf("%s\n", rgbLine);
	}
}

VOID PrintTCPRow(PMIB_TCPROW pTcpRow)
{
	char *lpState = NULL;
	MY_TCPROW MyTcpRow;

	MyTcpRow.dwState 				= pTcpRow->dwState;
	MyTcpRow.LocalAddr.S_un.S_addr 	= pTcpRow->dwLocalAddr;
	MyTcpRow.sLocalPort 			= ntohs(pTcpRow->dwLocalPort & 0xFF00);
	MyTcpRow.RemoteAddr.S_un.S_addr = MIB_TCP_STATE_LISTEN == MyTcpRow.dwState ? 0 : pTcpRow->dwRemoteAddr;
	MyTcpRow.sRemotePort 			= MIB_TCP_STATE_LISTEN == MyTcpRow.dwState ? 0 : ntohs(pTcpRow->dwRemotePort & 0xFF00);
	printf("%s:%d\t\t%s:%d ***",
		   inet_ntoa(MyTcpRow.LocalAddr), MyTcpRow.sLocalPort,
		   inet_ntoa(MyTcpRow.RemoteAddr), MyTcpRow.sRemotePort);

	struct {
		int code;
		const char* name;
	} tcp_state[] = {
		{MIB_TCP_STATE_CLOSED, "CLOSED"},
		{MIB_TCP_STATE_LISTEN, "LISTENING"},
		{MIB_TCP_STATE_SYN_SENT, "SYN_SENT"},
		{MIB_TCP_STATE_SYN_RCVD, "SYN_RCVD"},
	};

	// TCP State
	switch (pTcpRow->dwState)
	{
	case MIB_TCP_STATE_CLOSED:
		std::strcpy(lpState, "CLOSED");
		break;
	case MIB_TCP_STATE_LISTEN:
		std::strcpy(lpState, "LISTENING");
		break;
	case MIB_TCP_STATE_SYN_SENT:
		std::strcpy(lpState, "SIN_SENT");
		break;
	case MIB_TCP_STATE_SYN_RCVD:
		std::strcpy(lpState, "SIN_RCVD");
		break;
	case MIB_TCP_STATE_ESTAB:
		std::strcpy(lpState, "ESTABLISHED");
		break;
	case MIB_TCP_STATE_FIN_WAIT1:
		std::strcpy(lpState, "FIN_WAIT1");
		break;
	case MIB_TCP_STATE_FIN_WAIT2:
		std::strcpy(lpState, "FIN_WAIT2");
		break;
	case MIB_TCP_STATE_CLOSE_WAIT:
		std::strcpy(lpState, "CLOSE_WAIT");
		break;
	case MIB_TCP_STATE_CLOSING:
		std::strcpy(lpState, "CLOSING");
		break;
	case MIB_TCP_STATE_LAST_ACK:
		lpState = "LAST_ACK";
		break;
	case MIB_TCP_STATE_TIME_WAIT:
		lpState = "TIME_WAIT";
		break;
	case MIB_TCP_STATE_DELETE_TCB:
		lpState = "DELETE_TCP";
		break;
	default:
		lpState = "Unknown";
		break;
	}
	printf("\t%s\n", lpState);
}

VOID PrintIPHeader(IP_HEADER ip_header)
{
	printf("Version: %u\n", ip_header.ip_v); // version. This is always 4.
	printf("Header Length: %u * 4\n", ip_header.ip_hl);
	printf("Type of Service: %u\n", ip_header.ip_tos);		// type of service
	printf("Total Length: %u\n", ntohs(ip_header.ip_len));	// Total length
	printf("Identification: %u\n", ntohs(ip_header.ip_id)); // Identification
	printf("Flags: %u\n", ntohs(ip_header.ip_off));
	printf("Time To Live: %u\n", ip_header.ip_ttl);
	printf("Protocol: %u\n", ip_header.ip_p);
	printf("Checksum: %u\n", ntohs(ip_header.ip_sum));
	printf("IP src: %s\n", inet_ntoa(ip_header.ip_src));
	printf("IP dst: %s\n", inet_ntoa(ip_header.ip_dst));
}

VOID PrintTCPHeader(PTCP_HEADER tcp_header)
{
	if (!tcp_header)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return;
	}

	printf("Src Port: %u\n", ntohs(tcp_header->th_sport));
	printf("Dst Port: %u\n", ntohs(tcp_header->th_dport));
	printf("Sequence number: %u\n", ntohl(tcp_header->th_seq));
	printf("Acknowledgement: %u\n", ntohl(tcp_header->th_ack));
	printf("Header Length: %u\n", tcp_header->th_hl);
	printf("Flags:\n");
	printf("\tURG: %u\n", tcp_header->th_flags & 0x20 ? 1 : 0);
	printf("\tACK: %u\n", tcp_header->th_flags & 0x10 ? 1 : 0);
	printf("\tPSH: %u\n", tcp_header->th_flags & 0x8 ? 1 : 0);
	printf("\tRST: %u\n", tcp_header->th_flags & 0x4 ? 1 : 0);
	printf("\tSYN: %u\n", tcp_header->th_flags & 0x2 ? 1 : 0);
	printf("\tFIN: %u\n", tcp_header->th_flags & 0x1 ? 1 : 0);
	printf("Window size: %u\n", ntohs(tcp_header->th_win));
	printf("Checksum: %u\n", ntohs(tcp_header->th_sum));
	printf("Urgent offset: %u\n", ntohs(tcp_header->th_urp));
}

#ifdef __unix__
typedef int Sock;
#else
typedef SOCKET Sock;
#endif

class TinyWorm
{
public:
	TinyWorm()
	{
#ifdef __unix__

#else
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
		sock_ 	= socket(AF_INET, SOCK_RAW, IPPROTO_IP);

		if (sock_ == INVALID_SOCKET)
		{
			std::stringstream ss;
			ss << "socket error : " << WSAGetLastError() << std::endl;
			throw std::runtime_error(ss.str());
		}

		char addr_list_buff[4096];
		DWORD ret_bytes;
		if (WSAIoctl(sock_, SIO_ADDRESS_LIST_QUERY, NULL, 0, addr_list_buff, sizeof(addr_list_buff), &ret_bytes, NULL, NULL) != 0)
		{
			std::stringstream ss;
			ss << "WSAIoctl SIO_ADDRESS_LIST_QUERY error : " << WSAGetLastError() << std::endl;
			throw std::runtime_error(ss.str());
		}

		SOCKET_ADDRESS_LIST* sock_addr_list = (SOCKET_ADDRESS_LIST *)addr_list_buff;
		for (int i = 0; i < sock_addr_list->iAddressCount; i++)
		{
			SOCKADDR_IN* nic_addr = (SOCKADDR_IN *)sock_addr_list->Address[i].lpSockaddr;
			nics_.push_back(inet_ntoa(nic_addr->sin_addr));
		}
#endif
	}

	~TinyWorm()
	{
		closesocket(sock_);
		WSACleanup();		
	}

	void set_nic(const size_t& id)
	{
		SOCKADDR_IN addr;
		inet_pton(AF_INET, nics_[id].c_str(), &addr.sin_addr.s_addr);
		addr.sin_family 		= AF_INET;
		addr.sin_port 			= htons(0);
		
		if (bind(sock_, (SOCKADDR *)&addr, sizeof(addr)) != 0)
		{
			std::stringstream ss;
			ss << "bind error : " << WSAGetLastError() << std::endl;
			throw std::runtime_error(ss.str());
		}

		ULONG rcall_option = RCVALL_ON;
		DWORD ret_bytes;
		if (WSAIoctl(sock_, SIO_RCVALL, &rcall_option, sizeof(ULONG), NULL, 0, &ret_bytes, NULL, NULL) != 0)
		{
			std::stringstream ss;
			ss << "WSAIoctl SIO_RCVALL error : " << WSAGetLastError() << std::endl;
			throw std::runtime_error(ss.str());
		}
	}

	void capture() const
	{
		char buff[4096];
		int ret = recv(sock_, buff, sizeof(buff), 0);
		if (ret > 0)
		{
			IP_HEADER ip_header;
			std::memmove(&ip_header, buff, sizeof(IP_HEADER));
			
			// TCPの場合
			if (ip_header.ip_p == 6)
			{
				std::cout << "TCP " << std::endl;
			}

			if (ip_header.ip_p == 17)
			{
				std::cout << "UDP " << std::endl;
			}
		}
	}

	void describe() const
	{
		for (size_t i = 0; i < nics_.size(); i++)
		{
			std::cout << "NIC[" << std::to_string(i) << "] : " << nics_[i] << std::endl;
		}
	}

private:
	Sock sock_;
	std::vector<std::string> nics_;
};

// int main()
// {
// 	auto tiny_worm = TinyWorm();
// 	tiny_worm.describe();
// 	tiny_worm.set_nic(0);

// 	while (true)
// 	{
// 		tiny_worm.capture();
// 		Sleep(100);
// 	}

// 	Sleep(10000);
// }


int main()
{
	INT i, nRet, nNIC;
	char szBuff[4096], *lpNIC;
	DWORD dwBytesReturned;
	SOCKET_ADDRESS_LIST *sock_addr_list;
	SOCKADDR_IN *nic_addr, addr;
	ULONG uRCVALL_OPTION = RCVALL_ON;

	IP_HEADER ip_header;
	TCP_HEADER tcp_header;
	DWORD dwTotalHeaderLen;
	char *lpBuffer;

	// WSAStartup
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// Create Raw Socket
	SOCKET hSocket = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
	if (INVALID_SOCKET == hSocket)
	{
		printf("socket Failed.\n");
		return 1;
	}

	// Enumerate Interfaces and Prompt to Select (if any)
	nRet = WSAIoctl(
		hSocket,
		SIO_ADDRESS_LIST_QUERY, NULL, 0,
		szBuff, sizeof(szBuff), &dwBytesReturned,
		NULL, NULL);

	if (WSAEFAULT == nRet)
	{
		printf("ERROR: WSAIoctl Failed.\n");
		return 1;
	}

	sock_addr_list = (SOCKET_ADDRESS_LIST *)szBuff;

	if (sock_addr_list->iAddressCount > 1)
	{
		for (i = 0; i < sock_addr_list->iAddressCount; i++)
		{
			nic_addr = (SOCKADDR_IN *)sock_addr_list->Address[i].lpSockaddr;
			lpNIC = inet_ntoa(nic_addr->sin_addr);
			printf("NIC [%02d]: %s\n", i, lpNIC);
		}
		printf("Enter NIC Number you want to monitor --> ");
		scanf_s("%d", &nNIC);
		printf("\n");
		if (nNIC < 0 || sock_addr_list->iAddressCount <= nNIC)
		{
			printf("Invalid number.\n");
			return 1;
		}
	}
	else if (sock_addr_list->iAddressCount == 1)
	{
		nNIC = 0;
	}
	else
	{
		printf("No Network Available.\n");
		return 1;
	}

	nic_addr = (SOCKADDR_IN *)sock_addr_list->Address[nNIC].lpSockaddr;
	printf("[INFO] %s\n", inet_ntoa(nic_addr->sin_addr));

	// Bind
	addr.sin_addr.s_addr = nic_addr->sin_addr.s_addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(0);
	nRet = bind(hSocket, (SOCKADDR *)&addr, sizeof(addr));
	if (SOCKET_ERROR == nRet)
	{
		printf("bind Failed. %d\n", WSAGetLastError());
		return 1;
	}

	// Set SIO_RCVALL Option
	nRet = WSAIoctl(
		hSocket, SIO_RCVALL, &uRCVALL_OPTION, sizeof(ULONG),
		NULL, 0, &dwBytesReturned, NULL, NULL);

	if (0 != nRet)
	{
		printf("WSAIoctl Failed. SIO_RCVALL mode setting.\n");
		return 1;
	}

	// Capture Packets
	while (1)
	{

		dwBytesReturned = recv(hSocket, szBuff, sizeof(szBuff), 0);

		if (SOCKET_ERROR == dwBytesReturned || 0 == dwBytesReturned)
		{
			printf("recv returned SOCKET_ERROR or closed.\n");
			break;
		}
		memmove(&ip_header, szBuff, sizeof(IP_HEADER));

		printf("---------------------------------------------------------\n");
		printf("* IP HEADER\n");
		PrintHexDump(((ip_header.ip_hl) * 4), (PBYTE)szBuff);

		if (6 == ip_header.ip_p)
		{ // TCP Packet

			lpBuffer = szBuff;
			lpBuffer += ((ip_header.ip_hl) * 4);
			memmove(&tcp_header, lpBuffer, sizeof(TCP_HEADER));

			printf("* TCP HEADER\n");
			PrintHexDump(sizeof(TCP_HEADER), (PBYTE)&tcp_header);
			PrintTCPHeader(&tcp_header);

			printf("* TCP Payload\n");
			dwTotalHeaderLen = ((ip_header.ip_hl) * 4) + ((tcp_header.th_hl) * 4);
			lpBuffer = szBuff;
			lpBuffer += dwTotalHeaderLen;
			PrintHexDump(
				dwBytesReturned - dwTotalHeaderLen,
				(PBYTE)lpBuffer);
		}
		printf("* RAW DATA\n");
		PrintHexDump(dwBytesReturned, (PBYTE)szBuff);
	}

	// Clean up
	closesocket(hSocket);
	WSACleanup();

	return 0;
}