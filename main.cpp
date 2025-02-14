#include <iostream>
#include <cstring>

#include "cli_option.hpp"
#include "raw_socket.hpp"

char LISENCE_TEXT1[1024] = 
"Permission is hereby granted, free of charge, to any person obtaining a copy of "
"this software and associated documentation files (the \"Software\"), to deal in "
"the Software without restriction, including without limitation the rights to use, "
"copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, "
"and to permit persons to whom the Software is furnished to do so, subject to the following conditions:"
"The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.\n"
"THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, "
"INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR "
"PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE "
"FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, "
"ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.";

std::string ascii_str(const std::string& payload)
{
    auto ret = payload;
    for (auto& c : ret)
    {
        if (!std::isprint(c) && c == '\n')
            c = ' ';
    }
    return ret;
}

std::string hex_str(const std::string& payload)
{
    constexpr char hex[16] = { '0', '1', '2','3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    auto ret    = std::string(' ', payload.size() * 3);
    
    for (auto i = 0; i < payload.size(); i++)
    {
        ret[3 * i]      = hex[(((uint8_t)payload[i]) >> 4) & 0x0F];
        ret[3 * i + 1]  = hex[(uint8_t)payload[i] & 0x0F];
    }

    return ret;
}

int main(int argc, char* argv[])
{
    auto cli_option = CliOption();
  
    cli_option
        .add_description("Tiny Worm - simple packet analzer above the internet layer (L3).")
        .add_header("Usage: tiny_worm [OPTION]...")
        .add_copyright("MIT License")
        .add_copyright("Copyright (c) 20025 okano tomoyuki")
        .add_copyright(LISENCE_TEXT1)
        .add_option({
            {'a', "ascii", "enable console output payload by ascii text format.(default)"},
            {'A', "no-ascii", "disable console output payload by ascii text format."},
            {'b', "binary", "enable console output payload by binary hex dump format."},
            {'B', "no-binary", "disable console output payload by binary hex dump format.(default)"},
            {'f', "filter", "filter"},
            {'h', "help", "show help"},
            {'i', "ip-address", "specify ip address for capture."},
            {'l', "list", "show list ip address for capture."},
            {'o', "output", "output file path."},
            {'p', "port", "source or destination port number. (vaild in TCP/UDP)"},
            {'t', "transport", "transport protocol. [TCP | UDP | ICMP | number]"},
            {'v', "version", "show version."},
        });

    if (!cli_option.parse(argc, argv))
    {
        std::cerr << cli_option.last_error << std::endl;
        cli_option.print(CliOption::PRINT_HEADER | CliOption::PRINT_FOOTER);
        return 1;
    }

    if (cli_option.has('h'))
    {
        cli_option.print(CliOption::PRINT_DESCRIPTION | CliOption::PRINT_HEADER | CliOption::PRINT_FOOTER | CliOption::PRINT_COPYRIGHT);
        return 0;
    }

    if (cli_option.has('v'))
    {
        std::cout << "version: " << "1.0.0" << std::endl;
        return 0;
    }

    auto raw_socket = RawSocket();
    if (cli_option.has('l'))
    {
        for (const auto& nic :  raw_socket.enable_addr_list())
        {
            std::cout << nic << std::endl;
        }
        return 0;
    }

    if (raw_socket.enable_addr_list().empty())
    {
        std::cerr << "no ip address." << std::endl;
        return 1;
    }

    if (cli_option.has('i'))
    {
        const auto ip_address = cli_option.get<std::string>('i');
        raw_socket.set_addr(ip_address);
    }
    else
    {        
        const auto addr_list = raw_socket.enable_addr_list();
        for (size_t i = 0; i < addr_list.size(); i++)
        {
            std::cout << addr_list[i] << ":[" << i << "]" << std::endl;
        }

        while (true)
        {
            std::cout << "choose address:";
            
            size_t addr_no = -1;
            std::cin >> addr_no;
            
            if (addr_no < 0 || addr_list.size() <= addr_no)
            {
                std::cout << "invalid number."  << std::endl;
                continue;
            }
            
            raw_socket.set_addr(addr_list[addr_no]);
            break;
        }
    }

    auto raw_packet = std::string();
    auto meta       = std::string();
    auto payload    = std::string();

    while (true)
    {
        if (raw_socket.capture(raw_packet, meta, payload))
        {

            // std::cout << "meta: " << meta << std::endl;
            // std::cout << payload << std::endl;
        }
    }

    return 0;
}