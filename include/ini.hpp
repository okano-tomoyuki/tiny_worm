#ifndef INI_HPP
#define INI_HPP

#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <fstream>
#include <map>
#include <array>

class Ini final
{

public:

    Ini(const std::string& file_path = "", const char& delim = '=', const std::vector<std::string>& comment_prefixes = {"#", ";"});

    Ini& set_delim(const char& delim);

    Ini& set_comment_prefixes(const std::vector<std::string>& comment_prefixes);

    bool write_file(const std::string& file_path) const;

    bool read_file(const std::string& file_path);

    bool add_comment(const std::string& comment);
    
    bool add_comment(const std::string& section, const std::string& comment);
    
    bool add_comment(const std::string& section, const std::string& field, const std::string& comment);

    bool has(const std::string& section, const std::string& field) const;

    template<typename T>
    inline T get(const std::string& section, const std::string& field) const
    {
        auto str = std::string();
        if (!try_get_field(section, field, str))
        {
            throw std::runtime_error("Not found section '" + section + "' field '" + field + "'");
        }

        auto ret = T{};
        if (!try_assign(ret, str))
        {
            throw std::runtime_error("Failed to convert section '" + section   + "' field '" + field + "' to " + std::string(typeid(T).name()));
        }

        return ret;
    }

    template<typename T>
    inline T get(const std::string& section, const std::string& field, const T& alternative) const
    {
        auto str = std::string();
        if (try_get_field(section, field, str))
        {
            auto ret = alternative;
            return try_assign(ret, str) ? ret : alternative;
        }
        return alternative;
    }

    template<typename T>
    inline bool try_get(const std::string& section, const std::string& field, T& value) const
    {
        auto str = std::string();
        return try_get_field(section, field, str) ? try_assign(value, str) : false;
    }

    template<typename T>
    inline void set(const std::string& section, const std::string& field, const T& value)
    {
        set_field(section, field, to_str(value));
    }

private:
    enum Type
    {
        FIELD,
        COMMENT,
        IGNORE,
    };

    struct Line
    {
        Type            type;
        std::string     value;
        std::string     field;    
    };

    std::map<std::string, std::vector<Line>> sections_;
    char delim_;
    std::vector<std::string> comment_prefixes_;

    bool try_get_field(const std::string& section, const std::string& field, std::string& value) const;

    void set_field(const std::string& section, const std::string& field, const std::string& value);

    template<typename T>
    static inline bool try_assign(std::vector<T>& dst, const std::string& src)
    {
        if (src.size() >= 2 && src[0] == '[' && src[src.size() - 1] == ']')
        {
            auto ret = split(src.substr(1, src.size() - 2));
            dst.resize(ret.size());
            for (size_t i = 0; i < ret.size(); ++i)
            {
                if (!try_assign(dst[i], ret[i]))
                    return false;
            }
            return true;
        }
        return false;
    }

    static bool try_assign(std::vector<bool>& dst, const std::string& src);

    static bool try_assign(bool& dst, const std::string& src);

    static bool try_assign(int& dst, const std::string& src);

    static bool try_assign(double& dst, const std::string& src);

    static bool try_assign(std::string& dst, const std::string& src);

    template<typename T>
    static inline std::string to_str(const T& v)
    {
        auto ret = std::string("[");
        for (const auto& c : v)
        {
            ret += to_str(c) + ", ";
        }
        ret = (v.empty()) ? ret : ret.substr(0, ret.size() - 2);
        ret += "]";
        return ret;
    }
   
    static std::string to_str(const bool& v);

    static std::string to_str(const int& v);

    static std::string to_str(const double& v);

    static std::string to_str(const std::string& v);

    static std::string to_str(const char* v);

    static void trim(std::string& str);

    static std::vector<std::string> split(const std::string& str);

};

#endif // INI_HPP