#include <iostream>
#include <rapidfuzz/fuzz.hpp>
#include <string>

using namespace std;

int main()
{
    string a, b;
    cout << "입력 스트링 1: ";
    getline(cin, a);
    cout << "입력 스트링 2: ";
    getline(cin, b);

    cout << "유사도 계산중 ..." << endl;
    cout << a << "와 " << b << "의 유사도: ";
    cout << rapidfuzz::fuzz::ratio(a, b) << endl;
}