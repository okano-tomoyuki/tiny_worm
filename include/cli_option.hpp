#ifndef CLI_OPTION_HPP
#define CLI_OPTION_HPP
 
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <tuple>
 
class CliOption
{
 
public:

    enum ArgumentType
    {

    };

    enum PrintOption
    {
        PRINT_DESCRIPTION   = (1 << 0),
        PRINT_HEADER        = (1 << 1),
        PRINT_FOOTER        = (1 << 2),
        PRINT_COPYRIGHT     = (1 << 3),
    };

    std::string last_error;

    CliOption& add_option(const char& short_name, const std::string& long_name, const std::string& description);

    CliOption& add_option(const std::vector<std::tuple<char, std::string, std::string>>& list);

    CliOption& add_description(const std::string& description);

    CliOption& add_header(const std::string& header);

    CliOption& add_footer(const std::string& footer);

    CliOption& add_copyright(const std::string& copyright);

    bool parse(int argc, char** argv);
 
    void print(const int& print_option = 0) const;
 
    bool has(const char& key) const;
 
    bool has(const std::string& key) const;

    template<typename T>
    inline T get(const char& key) const
    {
        auto ret = T();
        get_to(key, ret);
        return ret;
    }

    template<typename T>
    inline T get(const std::string& key) const
    {
        auto ret = T();
        get_to(key, ret);
        return ret;
    }

    template<typename T>
    inline bool get_to(const char& key, T& value) const
    {
        const auto iter = std::find_if(option_list_.begin(), option_list_.end(), [&key](const Option& option) { return (key == option.short_name); });
        
        if (iter == option_list_.end())
        {
            return false;
        }

        if (iter->arg_list.size() != 1)
        {
            return false;
        }

        return assign(iter->arg_list[0], value);
    }

    template<typename T>
    inline bool get_to(const char& key, std::vector<T>& value) const
    {
        const auto iter = std::find_if(option_list_.begin(), option_list_.end(), [&key](const Option& option) { return (key == option.short_name); });
        
        if (iter == option_list_.end())
        {
            return false;
        }

        return assign(iter->arg_list, value);
    }    

    template<typename T>
    inline bool get_to(const std::string& key, T& value) const
    {
        const auto iter = std::find_if(option_list_.begin(), option_list_.end(), [&key](const Option& option) { return (key == option.long_name); });
        
        if (iter == option_list_.end())
        {
            return false;
        }

        if (iter->arg_list.size() != 1)
        {
            return false;
        }

        return assign(iter->arg_list[0], value);
    }

    template<typename T>
    inline bool get_to(const std::string& key, std::vector<T>& value) const
    {
        const auto iter = std::find_if(option_list_.begin(), option_list_.end(), [&key](const Option& option) { return (key == option.long_name); });
        
        if (iter == option_list_.end())
        {
            return false;
        }

        return assign(iter->arg_list, value);
    } 

private:
 
    struct Option
    {
        char                        short_name;
        std::string                 long_name;
        std::string                 description;
        
        bool                        found;
        std::vector<std::string>    arg_list;
 
        Option(const char& short_name, const std::string& long_name, const std::string& description)
            : short_name(short_name)
            , long_name(long_name)
            , description(description)
            , found(false)
        {}
    };

    std::vector<Option>   option_list_;

    std::vector<std::string> description_;
    std::vector<std::string> header_;
    std::vector<std::string> footer_;
    std::vector<std::string> copyright_;

    static bool assign(const std::string& str, bool& ret);

    static bool assign(const std::string& str, int& ret);

    static bool assign(const std::string& str, double& ret);

    static bool assign(const std::string& str, std::string& ret);

    static bool assign(const std::vector<std::string>& str_list, std::vector<bool>& ret);

    static bool assign(const std::vector<std::string>& str_list, std::vector<int>& ret);

    static bool assign(const std::vector<std::string>& str_list, std::vector<double>& ret);
    
    static bool assign(const std::vector<std::string>& str_list, std::vector<std::string>& ret);

};
 
#endif // CLI_OPTION_HPP