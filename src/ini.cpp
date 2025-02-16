#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <fstream>
#include <map>
#include <iostream>

#include "ini.hpp"

Ini::Ini(const std::string& file_path, const char& delim, const std::vector<std::string>& comment_prefixes)
    : delim_(delim)
    , comment_prefixes_(comment_prefixes)
{
    if (!file_path.empty())
        read_file(file_path);
}

Ini& Ini::set_delim(const char& delim)
{
    delim_ = delim;
    return *this;
}

Ini& Ini::set_comment_prefixes(const std::vector<std::string>& comment_prefixes)
{
    comment_prefixes_ = comment_prefixes;
    return *this;
}

bool Ini::write_file(const std::string& file_path) const
{
    std::ofstream ofs(file_path);
    
    if (!ofs.is_open())
        return false;

    auto max_field_length = 0;
    for (const auto& section : sections_)
    {
        for (const auto& line : section.second)
        {
            if (line.type == Type::FIELD)
            {
                max_field_length = std::max(max_field_length, static_cast<int>(line.field.size()));
            }
        }
    }
    max_field_length += (4 - (max_field_length % 4));

    auto ss = std::stringstream();

    auto add_lines = [&](std::stringstream& ss, const std::vector<Line>& lines)
    {
        for (const auto& line : lines)
        {
            if (line.type == Type::FIELD)
            {
                ss << line.field << std::string(max_field_length - line.field.size(), ' ') << delim_ << ' ' << line.value << std::endl;
            }
            else if (line.type == Type::COMMENT)
            {
                if (!comment_prefixes_.empty())
                {
                    ss << comment_prefixes_[0] << " " << line.value << std::endl;
                }
                else
                {
                    ss << line.value << std::endl;
                }
            }
            else // IGNORE
            {
                ss << line.value << std::endl;
            }
        }
    };

    if (sections_.find("") != sections_.end())
        add_lines(ss, sections_.at(""));
    
    for (const auto& section : sections_)
    {
        if (section.first == "")
            continue;

        ss << "[" << section.first << "]" << std::endl;
        add_lines(ss, section.second);
        ss << std::endl;
    }

    ofs << ss.str();
    return true;
}

bool Ini::read_file(const std::string& file_path)
{
    sections_.clear();
    auto ifs            = std::ifstream(file_path);

    if (!ifs.is_open())
        return false;

    auto lines          = std::vector<Line>();
    auto line           = std::string("");
    auto section        = std::string("");
    auto field          = std::string("");
    auto value          = std::string("");

    while (std::getline(ifs, line, '\n'))
    {
        trim(line);

        if (line.empty())
        {
            continue;
        }
        
        // comment
        for (const auto& comment_prefix : comment_prefixes_)
        {
            if (line.find(comment_prefix) == 0)
            {
                lines.push_back(Line{Type::COMMENT, line.substr(comment_prefix.size()), ""});;
                continue;
            }
        }

        // section
        const auto end_pos = line.find_first_of(']');
        if (line[0] == '[' && end_pos != std::string::npos && end_pos != 1)
        {
            sections_[section] = lines;
            lines.clear();
            section = line.substr(1, end_pos - 1);
            continue;
        }

        // field
        const auto delim_pos = line.find_first_of(delim_);
        if (delim_pos != std::string::npos)
        {
            field  = line.substr(0, delim_pos);
            value  = line.substr(delim_pos + 1);
            trim(field);
            trim(value);
            lines.push_back(Line{Type::FIELD, value, field});
            continue;
        }

        // ignore
        lines.push_back(Line{Type::IGNORE, line, ""});
    }

    // last section
    sections_[section] = lines;

    return true;
}

bool Ini::add_comment(const std::string& comment)
{
    auto  s = sections_.find("");
    
    if (s == sections_.end())
    {
        sections_[""].push_back(Line{Type::COMMENT, comment, ""});
        return true;
    }

    auto f = std::find_if(s->second.begin(), s->second.end(), [](const Line& l){
            return l.type != Type::COMMENT;
        }
    );

    if (f == s->second.end())
    {
        s->second.push_back(Line{Type::COMMENT, comment, ""});
        return true;
    }
    else
    {
        s->second.insert(f, Line{Type::COMMENT, comment, ""});
        return true;
    }
}

bool Ini::add_comment(const std::string& section, const std::string& comment)
{
    auto s = sections_.find(section);
    if (s == sections_.end())
    {
        return false;
    }

    auto f = std::find_if(s->second.begin(), s->second.end(), [](const Line& l){
            return l.type != Type::COMMENT;
        }
    );

    if (f == s->second.end())
    {
        s->second.push_back(Line{Type::COMMENT, comment, ""});
        return true;
    }
    else
    {
        s->second.insert(f, Line{Type::COMMENT, comment, ""});
        return true;
    }
}

bool Ini::add_comment(const std::string& section, const std::string& field, const std::string& comment)
{
    auto s = sections_.find(section);
    if (s == sections_.end())
    {
        return false;
    }

    auto f = std::find_if(s->second.begin(), s->second.end(), [&](const Line& l){
            return l.type == Type::FIELD && l.field == field;
        }
    );

    if (f == s->second.end())
    {
        return false;
    }
    else
    {
        s->second.insert(f, Line{Type::COMMENT, comment, ""});
        return true;
    }
}

bool Ini::has(const std::string& section, const std::string& field) const
{
    const auto s = sections_.find(section);
    if (s == sections_.end())
        return false;
    
    const auto f = std::find_if(s->second.begin(), s->second.end(), [&](const Line& l){
            return l.type == Type::FIELD && l.field == field;
        }
    );
    return f != s->second.end();
}

bool Ini::try_get_field(const std::string& section, const std::string& field, std::string& value) const
{
    const auto s = sections_.find(section);
    
    if (s == sections_.end())
        return false;

    const auto f = std::find_if(s->second.begin(), s->second.end(), [&](const Line& l){
            return l.type == Type::FIELD && l.field == field;
        }
    );

    if (f == s->second.end())
        return false;

    value = f->value;
    return true;
}

void Ini::set_field(const std::string& section, const std::string& field, const std::string& value)
{
    auto s = sections_.find(section);

    if (s == sections_.end())
    {
        sections_[section] = std::vector<Line>{Line{Type::FIELD, value, field}};
    }
    else
    {
        auto f = std::find_if(s->second.begin(), s->second.end(), [&](const Line& l){
                return l.type == Type::FIELD && l.field == field;
            }
        );
        
        if (f == s->second.end())
        {
            s->second.push_back(Line{Type::FIELD, value, field});
        }
        else
        {
            f->value = value;
        }
    }
}

bool Ini::try_assign(std::vector<bool>& dst, const std::string& src)
{
    if (src.size() >= 2 && src[0] == '[' && src[src.size() - 1] == ']')
    {
        auto ret = split(src.substr(1, src.size() - 2));
        dst.resize(ret.size());
        for (size_t i = 0; i < ret.size(); ++i)
        {
            auto ref = bool{};
            if (!try_assign(ref, ret[i]))
                return false;
            dst[i] = ref;
        }
        return true;
    }
    return false;
}

bool Ini::try_assign(bool& dst, const std::string& src)
{
    if (src == "true" || src == "TRUE" || src == "True" || src == "1")
    {
        dst = true;
        return true;
    }

    if (src == "false" || src == "FALSE" || src == "False" || src == "0")
    {
        dst = false;
        return true;
    }
    
    return false;
}

bool Ini::try_assign(int& dst, const std::string& src)
{
    if (src.empty())
        return false;

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

bool Ini::try_assign(double& dst, const std::string& src)
{
    if (src.empty())
        return false;
    
    char *endptr = nullptr;
    dst = std::strtod(src.c_str(), &endptr);
    if (*endptr == '\0')
        return true;
    return false;
}

bool Ini::try_assign(std::string& dst, const std::string& src)
{
    if (src.size() >= 2 && src[0] == '\"' && src[src.size() - 1] == '\"')
    {
        dst = src.substr(1, src.size() - 2);
        return true;
    }
    else
    {
        dst = src;
        return true;
    }
}

std::string Ini::to_str(const bool& v)
{
    return v ? "true" : "false";
}

std::string Ini::to_str(const int& v)
{
    return std::to_string(v);
}

std::string Ini::to_str(const double& v)
{
    return std::to_string(v);
}

std::string Ini::to_str(const std::string& v)
{
    return '\"' + v + '\"';
}

std::string Ini::to_str(const char* v)
{
    return '\"' + std::string(v) + '\"';
}

void Ini::trim(std::string& str)
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
}

std::vector<std::string> Ini::split(const std::string& str)
{
    auto ret        = std::vector<std::string>();
    auto pos        = std::string::size_type(0);
    auto prev_pos   = std::string::size_type(0);
    auto elem       = std::string();
    auto escape     = false;

    while ((pos = str.find_first_of("\",", prev_pos)) != std::string::npos)
    {
        if (str[pos] == '\"')
        {
            prev_pos = pos;
            pos = str.find_first_of('\"', pos + 1);
            if (pos == std::string::npos)
                pos = str.size();
            ret.push_back(str.substr(prev_pos, pos - prev_pos + 1));
            pos = str.find_first_of(',', pos + 1);
            if (pos == std::string::npos)
                pos = str.size();
            prev_pos = pos + 1;
            escape = true;
        }
        else // ','
        {
            elem = str.substr(prev_pos, pos - prev_pos);
            trim(elem);
            ret.push_back(elem);
            prev_pos = pos + 1;
            escape = false;
        }
    }

    if (!escape)
    {
        elem = str.substr(prev_pos);
        trim(elem);
        ret.push_back(elem);
    }

    return ret;
}