#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;

#include <serialize/Serializable.h>

namespace yazi {
namespace serialize {

class DataStream
{
public:
    enum DataType
    {
        BOOL = 0,
        CHAR,
        INT32,
        INT64,
        FLOAT,
        DOUBLE,
        STRING,
        VECTOR,
        LIST,
        MAP,
        SET,
        CUSTOM
    };

    enum ByteOrder
    {
        BigEndian,
        LittleEndian
    };

    DataStream();
    DataStream(const string & data);
    ~DataStream();

    void show() const;
    void write(const char * data, int len);
    void write(bool value);
    void write(char value);
    void write(int32_t value);
    void write(int64_t value);
    void write(float value);
    void write(double value);
    void write(const char * value);
    void write(const string & value);
    void write(const Serializable & value);
 
    template<typename T, typename Alloc = std::allocator<T>>
    void write(const std::vector<T, Alloc>& val);

    template<typename T, typename Alloc = std::allocator<T>>
    void write(const std::list<T, Alloc>& val);

    template<typename K, typename V, typename Compare = std::less<K>, typename Alloc = std::allocator<std::pair<const K, V>>>
    void write(const std::map<K, V, Compare, Alloc>& val);

    template<typename K, typename Compare = std::less<K>, typename Alloc = std::allocator<K>>
    void write(const std::set<K, Compare, Alloc>& val);

    template <typename T, typename ...Args>
    void write_args(const T & head, const Args&... args);

    void write_args();

    bool read(char * data, int len);
    bool read(bool & value);
    bool read(char & value);
    bool read(int32_t & value);
    bool read(int64_t & value);
    bool read(float & value);
    bool read(double & value);
    bool read(string & value);
    bool read(Serializable & value);

    template<typename T, typename Alloc = std::allocator<T>>
    bool read(std::vector<T, Alloc>& val);

    template<typename T, typename Alloc = std::allocator<T>>
    bool read(std::list<T, Alloc>& val);

    template<typename K, typename V, typename Compare = std::less<K>, typename Alloc = std::allocator<std::pair<const K, V>>>
    bool read(std::map<K, V, Compare, Alloc>& val);

    template<typename K, typename Compare = std::less<K>, typename Alloc = std::allocator<K>>
    bool read(std::set<K, Compare, Alloc>& val);

    template <typename T, typename ...Args>
    bool read_args(T & head, Args&... args);

    bool read_args();

    const char * data() const;
    int size() const;
    void clear();
    void reset();
    void save(const string & filename);
    void load(const string & filename);

    DataStream & operator << (bool value);
    DataStream & operator << (char value);
    DataStream & operator << (int32_t value);
    DataStream & operator << (int64_t value);
    DataStream & operator << (float value);
    DataStream & operator << (double value);
    DataStream & operator << (const char * value);
    DataStream & operator << (const string & value);
    DataStream & operator << (const Serializable & value);

    template<typename T, typename Alloc = std::allocator<T>>
    DataStream & operator << (const std::vector<T, Alloc> & value);

    template<typename T, typename Alloc = std::allocator<T>>
    DataStream & operator << (const std::list<T, Alloc> & value);

    template<typename K, typename V, typename Compare = std::less<K>, typename Alloc = std::allocator<std::pair<const K, V>>>
    DataStream & operator << (const std::map<K, V, Compare, Alloc> & value);

    template<typename K, typename Compare = std::less<K>, typename Alloc = std::allocator<K>>
    DataStream & operator << (const std::set<K, Compare, Alloc> & value);

    DataStream & operator >> (bool & value);
    DataStream & operator >> (char & value);
    DataStream & operator >> (int32_t & value);
    DataStream & operator >> (int64_t & value);
    DataStream & operator >> (float & value);
    DataStream & operator >> (double & value);
    DataStream & operator >> (string & value);
    DataStream & operator >> (Serializable & value);

    template<typename T, typename Alloc = std::allocator<T>>
    DataStream & operator >> (std::vector<T, Alloc> & value);

    template<typename T, typename Alloc = std::allocator<T>>
    DataStream & operator >> (std::list<T, Alloc> & value);

    template<typename K, typename V, typename Compare = std::less<K>, typename Alloc = std::allocator<std::pair<const K, V>>>
    DataStream & operator >> (std::map<K, V, Compare, Alloc> & value);

    template<typename K, typename Compare = std::less<K>, typename Alloc = std::allocator<K>>
    DataStream & operator >> (std::set<K, Compare, Alloc> & value);

private:
    void reserve(int len);
    ByteOrder byteorder();

private:
    std::vector<char> m_buf;
    int m_pos;
    ByteOrder m_byteorder;
};

template<typename T, typename Alloc>
void DataStream::write(const std::vector<T, Alloc>& value) {
    char type = DataType::VECTOR;
    write(reinterpret_cast<char*>(&type), sizeof(char));
    int len = value.size();
    write(len);
    for (auto& item : value) {
        write(item);
    }
}

template<typename T, typename Alloc>
void DataStream::write(const std::list<T, Alloc>& value)
{
    char type = DataType::LIST;
    write(reinterpret_cast<char*>(&type), sizeof(char));
    int len = value.size();
    write(len);
    for (auto& item : value){ 
        write(item);
    }
}

template<typename K, typename V, typename Compare, typename Alloc>
void DataStream::write(const std::map<K, V, Compare, Alloc>& value)
{
    char type = DataType::MAP;
    write(reinterpret_cast<char*>(&type), sizeof(char));
    int len = value.size();
    write(len);
    for (auto it = value.begin(); it != value.end(); it++)
    {
        write(it->first);
        write(it->second);
    }
}

template<typename K, typename Compare, typename Alloc>
void DataStream::write(const std::set<K, Compare, Alloc>& value)
{
    char type = DataType::SET;
    write((char *)&type, sizeof(char));
    int len = value.size();
    write(len);
    for (auto it = value.begin(); it != value.end(); it++)
    {
        write(*it);
    }
}

template <typename T, typename ...Args>
void DataStream::write_args(const T & head, const Args&... args)
{
    write(head);
    write_args(args...);
}

template<typename T, typename Alloc>
bool DataStream::read(std::vector<T, Alloc>& value)
{
    value.clear();
    if (m_buf[m_pos] != DataType::VECTOR)
    {
        return false;
    }
    ++m_pos;
    int len;
    read(len);
    for (int i = 0; i < len; i++)
    {
        T v;
        read(v);
        value.push_back(v);
    }
    return true;
}

template<typename T, typename Alloc>
bool DataStream::read(std::list<T, Alloc>& value)
{
    value.clear();
    if (m_buf[m_pos] != DataType::LIST)
    {
        return false;
    }
    ++m_pos;
    int len;
    read(len);
    for (int i = 0; i < len; i++)
    {
        T v;
        read(v);
        value.push_back(v);
    }
    return true;
}

template<typename K, typename V, typename Compare, typename Alloc>
bool DataStream::read(std::map<K, V, Compare, Alloc>& value)
{
    value.clear();
    if (m_buf[m_pos] != DataType::MAP)
    {
        return false;
    }
    ++m_pos;
    int len;
    read(len);
    for (int i = 0; i < len; i++)
    {
        K k;
        read(k);

        V v;
        read(v);
        value[k] = v;
    }
    return true;
}

template<typename K, typename Compare, typename Alloc>
bool DataStream::read(std::set<K, Compare, Alloc>& value)
{
    value.clear();
    if (m_buf[m_pos] != DataType::SET)
    {
        return false;
    }
    ++m_pos;
    int len;
    read(len);
    for (int i = 0; i < len; i++)
    {
        K v;
        read(v);
        value.insert(v);
    }
    return true;
}

template <typename T, typename ...Args>
bool DataStream::DataStream::read_args(T & head, Args&... args)
{
    read(head);
    return read_args(args...);
}

template<typename T, typename Alloc>
DataStream & DataStream::operator << (const std::vector<T, Alloc> & value)
{
    write(value);
    return *this;
}

template<typename T, typename Alloc>
DataStream & DataStream::operator << (const std::list<T, Alloc> & value) {
    write(value);
    return *this;
}

template<typename K, typename V, typename Compare, typename Alloc>
DataStream & DataStream::operator << (const std::map<K, V, Compare, Alloc> & value)
{
    write(value);
    return *this;
}
template<typename K, typename Compare, typename Alloc>
DataStream & DataStream::operator << (const std::set<K, Compare, Alloc> & value)
{
    write(value);
    return *this;
}

template<typename T, typename Alloc>
DataStream & DataStream::operator >> (std::vector<T, Alloc> & value)
{
    read(value);
    return *this;
}

template<typename T, typename Alloc>
DataStream & DataStream::operator >> (std::list<T, Alloc> & value)
{
    read(value);
    return *this;
}

template<typename K, typename V, typename Compare, typename Alloc>
DataStream & DataStream::operator >> (std::map<K, V, Compare, Alloc> & value)
{
    read(value);
    return *this;
}

template<typename K, typename Compare, typename Alloc>
DataStream & DataStream::operator >> (std::set<K, Compare, Alloc> & value)
{
    read(value);
    return *this;
}

}
}