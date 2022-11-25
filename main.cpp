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
    ds << a;
    ds.save("a.out");

    return 0;
}