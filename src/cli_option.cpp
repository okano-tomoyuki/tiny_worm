#include "cli_option.hpp"

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <tuple>

static void add_lines(const std::string& str, std::vector<std::string>& lines)
{
    constexpr int line_size = 95;
    auto remain = str;
    auto pos    = std::string::size_type{0};

    const auto min_value = [](const std::string::size_type& lhs, const std::string::size_type& rhs)
    {
        return (lhs < rhs) ? lhs : rhs;
    };

    while (true)
    {
        if (remain.size() < line_size)
        {
            lines.push_back(remain);
            lines.push_back("");
            break;
        }

        pos    = min_value(remain.find_first_of("\n"), remain.find_last_of(",. ", line_size));

        if (pos == std::string::npos)
        {
            lines.push_back(remain.substr(0, line_size));
            remain = remain.substr(line_size);
        }
        else
        {
            if (remain[pos] == '\n')
            {
                lines.push_back(remain.substr(0, pos));
                lines.push_back("");
                remain = remain.substr(pos + 1);
            }
            else
            {
                lines.push_back(remain.substr(0, pos + 1));
                remain = remain.substr(pos + 1);
            }
        }
    }
}
 
CliOption& CliOption::add_option(const char& short_name, const std::string& long_name, const std::string& description)
{
    option_list_.push_back(Option{short_name, long_name, description});
    return *this;
}

CliOption& CliOption::add_option(const std::vector<std::tuple<char, std::string, std::string>>& list)
{
    for (const auto& t : list)
    {
        add_option(std::get<0>(t), std::get<1>(t), std::get<2>(t));
    }
    return *this;
}

CliOption& CliOption::add_description(const std::string& description)
{
    add_lines(description, description_);
    return *this;
}

CliOption& CliOption::add_header(const std::string& header)
{
    add_lines(header, header_);
    return *this;
}

CliOption& CliOption::add_footer(const std::string& footer)
{
    add_lines(footer, footer_);
    return *this;
}

CliOption& CliOption::add_copyright(const std::string& copyright)
{
    add_lines(copyright, copyright_);
    return *this;    
}

bool CliOption::parse(int argc, char** argv)
{
    auto list = std::vector<std::string>();
    while (argc > 1)
    {
        std::string elem(argv[argc - 1]);
        if (elem.size() == 2 && elem[0] == '-' && elem[1] != '-')
        {
            const auto iter = std::find_if(option_list_.begin(), option_list_.end(), [&elem](const Option& option){ return (elem[1] == option.short_name); });
            if (iter != option_list_.end())
            {
                if (iter->found)
                {
                    last_error = "duplicated option: " + elem;
                    return false;
                }

                std::reverse(list.begin(), list.end());
                iter->found = true;
                iter->arg_list = list;
                list.clear();
            }
            else
            {
                last_error = "unknown option: " + elem;
                return false;
            }
        }
        else if (elem.size() > 2 && elem[0] == '-' && elem[1] == '-')
        {
            const auto iter = std::find_if(option_list_.begin(), option_list_.end(), [&elem](const Option& option){ return (elem.substr(2) == option.long_name); });
            if (iter != option_list_.end())
            {
                if (iter->found)
                {
                    last_error = "duplicated option: " + elem;
                    return false;
                }

                std::reverse(list.begin(), list.end());
                iter->found     = true;
                iter->arg_list  = list;
                list.clear();
            }
            else
            {
                last_error = "unknown option: " + elem;
                return false;
            }
        }
        else
        {
            list.push_back(elem);
        }
        argc--;
    }

    return true;
}

void CliOption::print(const int& print_option) const
{
    const auto print_description    = (((print_option >> 0) & 1) == 1);
    const auto print_header         = (((print_option >> 1) & 1) == 1);
    const auto print_footer         = (((print_option >> 2) & 1) == 1);
    const auto print_copyright      = (((print_option >> 3) & 1) == 1);

    if (print_description && !description_.empty())
    {
        std::cout << std::endl;
        std::cout << std::string(100, '-') << std::endl;
        std::cout << std::endl;
        for (const auto& line : description_)
        {
            std::cout << "  " << line << std::endl;
        }
        std::cout << std::string(100, '-') << std::endl;  
    }

    if (print_header && !header_.empty())
    {
        std::cout << std::endl;
        for (const auto& line : header_)
        {
            std::cout << line << std::endl;
        }
    }

    for (const auto& option : option_list_)
    {
        std::string prefix = "   -" + std::string(1, option.short_name) + ", --" + option.long_name;
        std::cout << prefix << std::string(30 - prefix.size(), ' ') << " ... " << option.description << std::endl;
        std::cout << std::endl;
    }

    if (print_footer && !footer_.empty())
    {
        for (const auto& line : footer_)
        {
            std::cout << line << std::endl;
        }
    }

    if (print_copyright && !copyright_.empty())
    {
        std::cout << std::string(100, '-') << std::endl;
        std::cout << std::endl;
        for (const auto& line : copyright_)
        {
            std::cout << "  " << line << std::endl;
        }
        std::cout << std::string(100, '-') << std::endl;    
    }   
}

bool CliOption::has(const char& key) const
{
    const auto iter = std::find_if(option_list_.begin(), option_list_.end(), [&key](const Option& option) { return (key == option.short_name); });
    return iter != option_list_.end() && iter->found;
}

bool CliOption::has(const std::string& key) const
{
    const auto iter =std::find_if(option_list_.begin(), option_list_.end(), [&key](const Option& option) { return (key == option.long_name); });
    return iter != option_list_.end() && iter->found;
}

bool CliOption::assign(const std::string& str, bool& ret)
{
    if (str != "true" && str != "false" && str != "TRUE" && str != "FALSE" && str != "True" && str != "False")
    {
        return false;
    }

    ret = (str == "true" || str == "TRUE" || str == "True");
    return true;
}

bool CliOption::assign(const std::string& str, int& ret)
{
    char* endptr    = nullptr;
    ret             = std::strtol(str.c_str(), &endptr, 10);
    
    if (endptr == str.c_str())
        return false;

    return true;
}

bool CliOption::assign(const std::string& str, double& ret)
{
    char* endptr    = nullptr;
    ret             = std::strtod(str.c_str(), &endptr);
    
    if (endptr == str.c_str())
        return false;

    return true;
}

bool CliOption::assign(const std::string& str, std::string& ret)
{
    ret = str;
    return true;
}

bool CliOption::assign(const std::vector<std::string>& str_list, std::vector<bool>& ret)
{
    ret.resize(str_list.size());
    bool r;
    for (size_t i = 0; i < str_list.size(); i++)
    {
        if (!assign(str_list[i], r))
            return false;
        ret[i] = r;
    }
    return true;
}

bool CliOption::assign(const std::vector<std::string>& str_list, std::vector<int>& ret)
{
    ret.resize(str_list.size());
    for (size_t i = 0; i < str_list.size(); i++)
    {
        if (!assign(str_list[i], ret[i]))
        {
            return false;
        }
    }
    return true;
}

bool CliOption::assign(const std::vector<std::string>& str_list, std::vector<double>& ret)
{
    ret.resize(str_list.size());
    for (size_t i = 0; i < str_list.size(); i++)
    {
        if (!assign(str_list[i], ret[i]))
        {
            return false;
        }
    }
    return true;
}

bool CliOption::assign(const std::vector<std::string>& str_list, std::vector<std::string>& ret)
{
    ret = str_list;
    return true;
}