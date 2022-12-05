#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "DataStream.h"
#include "Serializable.h"
using namespace yazi::serialize;


class A : public Serializable
{
public:
    A() {}
    A(const string & name, int age) : m_name(name), m_age(age) {}
    ~A() {}

    void show()
    {
        std::cout << m_name << "," << m_age << std::endl;
    }

    SERIALIZE(m_name, m_age)

private:
    string m_name;
    int m_age;
};

int main()
{
    A a("kitty", 18);

    DataStream ds;
    std::set<int, std::greater<int>> s {5, 100, 0, 40, 25};
    ds << a << s;
    ds.save("a.out");

    for (const auto& i : s) {
        std::cout << i << " ";
    }
    
    return 0;
}