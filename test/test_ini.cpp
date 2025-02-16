#include "ini.hpp"

#include <iostream>

int main(int argc, char* argv[])
{
    auto ini = Ini();
    ini.set("section1", "field1", 123);
    ini.set("section1", "field2", 123.456);
    ini.set("section1", "field3", "abc");
    ini.set("section1", "field4", false);

    ini.set("section2", "field1", std::vector<int>{456, 789});
    ini.set("section2", "field2", std::vector<double>{456.789, 123.456});
    ini.set("section2", "field3", std::vector<std::string>{"abc", "def", "ghi", "jkl"});
    ini.set("section2", "field4", std::vector<bool>{true, false, false});

    ini.write_file("test.ini");

    ini.read_file("test.ini");

    auto v1_1 = ini.get<int>("section1", "field1");
    auto v1_2 = ini.get<double>("section1", "field2");
    auto v1_3 = ini.get<std::string>("section1", "field3");
    auto v1_4 = ini.get<bool>("section1", "field4");

    std::cout << v1_1 << std::endl;
    std::cout << v1_2 << std::endl;
    std::cout << v1_3 << std::endl;
    std::cout << v1_4 << std::endl;

    auto v2_1 = ini.get<std::vector<int>>("section2", "field1");
    auto v2_2 = ini.get<std::vector<double>>("section2", "field2");
    auto v2_3 = ini.get<std::vector<std::string>>("section2", "field3");
    auto v2_4 = ini.get<std::vector<bool>>("section2", "field4");

    for (const auto& v : v2_1)
    {
        std::cout << v << ' ';
    }
    std::cout << std::endl;

    for (const auto& v : v2_2)
    {
        std::cout << v << ' ';
    }
    std::cout << std::endl;
    
    for (const auto& v : v2_3)
    {
        std::cout << v << ' ';
    }
    std::cout << std::endl;
    
    for (const auto& v : v2_4)
    {
        std::cout << v << ' ';
    }
    std::cout << std::endl;

    ini.set("section1", "field1", 789);
    ini.set("section1", "field2", 789.123);
    ini.set("section1", "field3", "hij");
    ini.set("section1", "field4", false);
    ini.set("section1", "field5", 123);
    ini.set("section1", "field6", 456.789);
    ini.set("section1", "field7", "klm");
    ini.set("section1", "field8", true);
    ini.set("section1", "field9", std::vector<int>{10, 20, 30});
    ini.set("section1", "field10", std::vector<double>{1.11, 2.22, 3.33});

    ini.add_comment("this is comment");
    ini.add_comment("section1", "this is comment1 for section1");
    ini.add_comment("section1", "this is comment2 for section1");
    ini.add_comment("section1", "field1", "this is comment for field1");
    ini.add_comment("section1", "field2", "this is comment for field2");
    ini.add_comment("section1", "field3", "this is comment for field3");
    ini.write_file("test2.ini");
}