#include <rapidfuzz/fuzz.hpp>
#include <string>
#include <iostream>

int main()
{
    std::string a = "aaaa";
    std::string b = "abab";
    std::cout << rapidfuzz::fuzz::ratio(a, b) << std::endl;
}