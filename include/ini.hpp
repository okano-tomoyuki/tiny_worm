/**
 * @file ini.hpp
 * @author okano tomoyuki (tomoyuki.okano@tsuneishi.com)
 * @brief ini ファイルのパースを行う @ref Utility::Ini クラスの定義ヘッダー
 * @version 0.1
 * @date 2024-01-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _UTILITY_INI_HPP_
#define _UTILITY_INI_HPP_

#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <fstream>
#include <map>

class Ini final
{

public:

    Ini(const std::string &file_path)
        : file_path_(std::string(file_path))
        , delim_('=')
        , comment_prefix_list_({"#", ";"})
    {}

    Ini& set_delim(const char& delim)
    {
        delim_ = delim;
        return *this;
    }

    Ini& set_comment_prefix_list(const std::vector<std::string>& comment_prefix_list)
    {
        comment_prefix_list_ = comment_prefix_list;
        return *this;
    }

    bool write_file(const std::string& file_path) const
    {
        std::ofstream ofs(file_path);
        if (!ofs)
            return false;

        auto ss = std::stringstream();
        for (const auto& line : lines_)
        {
            if (line.type == Type::SECTION)
            {
                ss << "[" << line.value << "]" << std::endl;
            }
            else if (line.type == Type::FIELD)
            {
                ss << line.field << delim_ << line.value << std::endl;
            }
            else
            {
                if (!comment_prefix_list_.empty())
                {
                    ss << comment_prefix_list_[0] << " " << line.value << std::endl;
                }
                else
                {
                    ss << line.value << std::endl;
                }
            }
        }

        ofs << ss.str();
        return true;
    }

    bool read_file(const std::string& file_path)
    {
        lines_.clear();

        const auto trim = [](std::string& str)
        {
            const char *whitespaces = " \t\n\r\f\v";
            auto lascurrent_tpos = str.find_last_not_of(whitespaces);
            if (lascurrent_tpos == std::string::npos)
            {
                str.clear();
                return;
            }
            str.erase(lascurrent_tpos + 1);
            str.erase(0, str.find_first_not_of(whitespaces));
        };

        const auto try_read_section = [](const std::string& line, std::string& section)
        {
            if (line[0] != '[')
                return false;
            
            const auto pos = line.find("]");
            if (pos == std::string::npos)
                return false;
            
            if (pos == 1)
                return false;
            
            section = line.substr(1, pos - 1);
            return true;
        };

        const auto try_read_field = [](const std::string& line, std::string& field, std::string& value)
        {
            const auto pos = line.find("=");
            if (pos == std::string::npos)
                return false;
            
            field  = line.substr(0, pos);
            value  = line.substr(pos + 1);
            return true;
        };

        auto ifs        = std::ifstream(file_path);

        if (!ifs.is_open())
            return false;

        auto lines          = std::vector<Line>();
        auto str_line       = std::string();
        auto found_section  = std::string();
        auto section        = std::string();
        auto field          = std::string();
        auto value          = std::string();

        while (std::getline(ifs, str_line, '\n'))
        {
            trim(str_line);

            if (str_line.empty())
                continue;
            
            // section
            if (try_read_section(str_line, found_section))
            {
                for (auto& line : lines)
                {
                    line.section = section;
                    lines_.push_back(line);
                }

                lines.clear();
                section = found_section;
                continue;
            }

            // field
            if (try_read_field(str_line, field, value))
            {
                lines.push_back(Line{Type::FIELD, field, value});
                continue;
            }

            // ignore
            lines.push_back(Line{Type::IGNORE, str_line});
        }

        // last section

        return true;
    }

    template<typename T>
    T get(const std::string& section, const std::string& field, const T& alternative) const
    {
        const auto iter = std::find_if(lines_.begin(), lines_.end(), [&](const Line& line){
                return line.section == section && line.field == field;
            }
        );
        
        if (iter == lines_.end())
            return alternative;
    
        auto ret = alternative;
        if (try_assign(iter->value, ret))
            return ret;

        return alternative;
    }

    template<typename T>
    inline void set(const std::string& section, const std::string& field, const T& value)
    {
        const auto section_iter = std::find_if(lines_.begin(), lines_.end(), [&](const Line& line){
                return line.type == Type::SECTION && line.section == section;
            }
        );

        const auto field_iter = std::find_if(lines_.begin(), lines_.end(), [&](const Line& line){
                return line.type == Type::FIELD && line.section == section && line.field == field;
            }
        );

        if (section_iter == lines_.end())
        {
            lines_.push_back(Line(Type::SECTION, section));
            lines_.push_back(Line(Type::FIELD, field, value));
        }
        else
        {
            auto index = std::distance(lines_.begin(), section_iter) + 1;
            for (index; index < lines_.size(); index++)
            {
                if (lines_[index].type == Type::SECTION)
                {
                    break;
                }
                else if (lines_[index].type == Type::FIELD && lines_[index].field == field)
                {
                    lines_[index].value = value;
                    return;
                }
            }
            lines_.insert(lines_.begin() + index, Line(Type::FIELD, field, value));
        }
    }

    bool read_bool(const std::string& section, const std::string& field, const bool& alternative)
    {
        if (!read_file(file_path_))
            return alternative;

        return get<bool>(section, field, alternative);
    }

    int read_int(const std::string& section, const std::string& field, const int& alternative)
    {
        if (!read_file(file_path_))
            return alternative;

        return get<int>(section, field, alternative);
    }

    double read_double(const std::string& section, const std::string& field, const double& alternative)
    {
        if (!read_file(file_path_))
            return alternative;

        return get<double>(section, field, alternative);
    }

    std::string read_str(const std::string& section, const std::string& field, const std::string& alternative)
    {
        if (!read_file(file_path_))
            return alternative;

        return get<std::string>(section, field, alternative);
    }

    bool write_bool(const std::string& section, const std::string& field, const bool &value) const
    {
        // set<bool>(section, field, value);
        
        return write_file(file_path_);
    }

    bool write_int(const std::string& section, const std::string& field, const int& value) const
    {
        std::stringstream ss;
        ss << value;
        return write_file(file_path_);
    }

    bool write_double(const std::string& section, const std::string& field, const double& value) const
    {
        std::stringstream ss;
        ss << value;
        return write_file(file_path_);
    }

    bool write_str(const std::string& section, const std::string& field, const std::string& value) const
    {
        return write_file(file_path_);
    }

private:
    enum struct Type
    {
        SECTION,
        FIELD,
        COMMENT,
        IGNORE,
    };

    struct Line
    {
        Type            type;
        std::string     value;
        std::string     section;
        std::string     field;
    
        Line(Type type, const std::string& value, const std::string& section = "", const std::string& field = "")
            : type(type)
            , value(value)
            , section(section)
            , field(field)
        {}
    };

    std::vector<Line> lines_;
    std::string file_path_;
    char delim_;
    std::vector<std::string> comment_prefix_list_;

    bool try_assign(bool& dst, const std::string& src) const
    {
        if (src == "true" || src == "TRUE" || src == "1")
        {
            dst = true;
            return true;
        }

        if (src == "false" || src == "FALSE" || src == "0")
        {
            dst = false;
            return true;
        }
        
        return false;
    }

    bool try_assign(int& dst, const std::string& src) const
    {
        char *endptr = nullptr;
        dst = std::strtol(src.c_str(), &endptr, 10);        
        if (*endptr == '\0')
            return true;
        dst = std::strtol(src.c_str(), &endptr, 8);
        if (*endptr == '\0')
            return true;
        dst = std::strtol(src.c_str(), &endptr, 16);
        if (*endptr == '\0')
            return true;
        return false;
    }

    bool try_assign(double& dst, const std::string& src) const
    {
        char *endptr = nullptr;
        dst = std::strtod(src.c_str(), &endptr);
        if (*endptr == '\0')
            return true;
        return false;
    }

    bool try_assign(std::string& dst, const std::string& src) const
    {
        dst = src;
        return true;
    }

};

#endif