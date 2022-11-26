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

#include "Serializable.h"

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
    ~DataStream() {}

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
 
    template <typename T>
    void write(const std::vector<T> & value);

    template <typename T>
    void write(const std::list<T> & value);

    template <typename K, typename V>
    void write(const std::map<K, V> & value);

    template <typename T>
    void write(const std::set<T> & value);

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

    template <typename T>
    bool read(std::vector<T> & value);

    template <typename T>
    bool read(std::list<T> & value);

    template <typename K, typename V>
    bool read(std::map<K, V> & value);

    template <typename T>
    bool read(std::set<T> & value);

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

    template <typename T>
    DataStream & operator << (const std::vector<T> & value);

    template <typename T>
    DataStream & operator << (const std::list<T> & value);

    template <typename K, typename V>
    DataStream & operator << (const std::map<K, V> & value);

    template <typename T>
    DataStream & operator << (const std::set<T> & value);

    DataStream & operator >> (bool & value);
    DataStream & operator >> (char & value);
    DataStream & operator >> (int32_t & value);
    DataStream & operator >> (int64_t & value);
    DataStream & operator >> (float & value);
    DataStream & operator >> (double & value);
    DataStream & operator >> (string & value);
    DataStream & operator >> (Serializable & value);

    template <typename T>
    DataStream & operator >> (std::vector<T> & value);

    template <typename T>
    DataStream & operator >> (std::list<T> & value);

    template <typename K, typename V>
    DataStream & operator >> (std::map<K, V> & value);

    template <typename T>
    DataStream & operator >> (std::set<T> & value);

private:
    void reserve(int len);
    ByteOrder byteorder();

private:
    std::vector<char> m_buf;
    int m_pos;
    ByteOrder m_byteorder;
};

DataStream::DataStream() : m_pos(0)
{
    m_byteorder = byteorder();
}

DataStream::DataStream(const string & str) : m_pos(0)
{
    m_byteorder = byteorder();
    m_buf.clear();
    reserve(str.size());
    write(str.data(), str.size());
}

void DataStream::reserve(int len)
{
    int size = m_buf.size();
    int cap = m_buf.capacity();
    if (size + len > cap)
    {
        while (size + len > cap)
        {
            if (cap == 0)
            {
                cap = 1;
            }
            else
            {
                cap *= 2;
            }
        }
        m_buf.reserve(cap);
    }
}

DataStream::ByteOrder DataStream::byteorder()
{
    int n = 0x12345678;
    char str[4];
    memcpy(str, &n, sizeof(int));
    if (str[0] == 0x12)
    {
        return ByteOrder::BigEndian;
    }
    else
    {
        return ByteOrder::LittleEndian;
    }
}

void DataStream::show() const
{
    int size = m_buf.size();
    std::cout << "data size = " << size << std::endl;
    int i = 0;
    while (i < size)
    {
        switch ((DataType)m_buf[i])
        {
        case DataType::BOOL:
            if ((int)m_buf[++i] == 0)
            {
                std::cout << "false";
            }
            else
            {
                std::cout << "true";
            }
            ++i;
            break;
        case DataType::CHAR:
            std::cout << m_buf[++i];
            ++i;
            break;
        case DataType::INT32:
            std::cout << *((int32_t *)(&m_buf[++i]));
            i += 4;
            break;
        case DataType::INT64:
            std::cout << *((int64_t *)(&m_buf[++i]));
            i += 8;
            break;
        case DataType::FLOAT:
            std::cout << *((float *)(&m_buf[++i]));
            i += 4;
            break;
        case DataType::DOUBLE:
            std::cout << *((double *)(&m_buf[++i]));
            i += 8;
            break;
        case DataType::STRING:
            if ((DataType)m_buf[++i] == DataType::INT32)
            {
                int len = *((int *)(&m_buf[++i]));
                i += 4;
                std::cout << string(&m_buf[i], len);
                i += len;
            }
            else
            {
                throw std::logic_error("parse string error");
            }
            break;
        case DataType::VECTOR:
            if ((DataType)m_buf[++i] == DataType::INT32)
            {
                int len = *((int *)(&m_buf[++i]));
                i += 4;
            }
            else
            {
                throw std::logic_error("parse vector error");
            }
            break;
        case DataType::MAP:
            if ((DataType)m_buf[++i] == DataType::INT32)
            {
                int len = *((int *)(&m_buf[++i]));
                i += 4;
            }
            else
            {
                throw std::logic_error("parse map error");
            }
            break;
        case DataType::SET:
            if ((DataType)m_buf[++i] == DataType::INT32)
            {
                int len = *((int *)(&m_buf[++i]));
                i += 4;
            }
            else
            {
                throw std::logic_error("parse set error");
            }
            break;
        case DataType::CUSTOM:
            break;
        default:
            break;
        }
    }
    std::cout << std::endl;
}

void DataStream::write(const char * data, int len)
{
    reserve(len);
    int size = m_buf.size();
    m_buf.resize(m_buf.size() + len);
    std::memcpy(&m_buf[size], data, len);
}

void DataStream::write(bool value)
{
    char type = DataType::BOOL;
    write((char *)&type, sizeof(char));
    write((char *)&value, sizeof(char));
}

void DataStream::write(char value)
{
    char type = DataType::CHAR;
    write((char *)&type, sizeof(char));
    write((char *)&value, sizeof(char));
}

void DataStream::write(int32_t value)
{
    char type = DataType::INT32;
    write((char *)&type, sizeof(char));
    if (m_byteorder == ByteOrder::BigEndian)
    {
        char * first = (char *)&value;
        char * last = first + sizeof(int32_t);
        std::reverse(first, last);
    }
    write((char *)&value, sizeof(int32_t));
}

void DataStream::write(int64_t value)
{
    char type = DataType::INT64;
    write((char *)&type, sizeof(char));
    if (m_byteorder == ByteOrder::BigEndian)
    {
        char * first = (char *)&value;
        char * last = first + sizeof(int64_t);
        std::reverse(first, last);
    }
    write((char *)&value, sizeof(int64_t));
}

void DataStream::write(float value)
{
    char type = DataType::FLOAT;
    write((char *)&type, sizeof(char));
    if (m_byteorder == ByteOrder::BigEndian)
    {
        char * first = (char *)&value;
        char * last = first + sizeof(float);
        std::reverse(first, last);
    }
    write((char *)&value, sizeof(float));
}

void DataStream::write(double value)
{
    char type = DataType::DOUBLE;
    write((char *)&type, sizeof(char));
    if (m_byteorder == ByteOrder::BigEndian)
    {
        char * first = (char *)&value;
        char * last = first + sizeof(double);
        std::reverse(first, last);
    }
    write((char *)&value, sizeof(double));
}

void DataStream::write(const char * value)
{
    char type = DataType::STRING;
    write((char *)&type, sizeof(char));
    int len = strlen(value);
    write(len);
    write(value, len);
}

void DataStream::write(const string & value)
{
    char type = DataType::STRING;
    write((char *)&type, sizeof(char));
    int len = value.size();
    write(len);
    write(value.data(), len);
}

void DataStream::write(const Serializable & value)
{
    value.serialize(*this);
}

template <typename T>
void DataStream::write(const std::vector<T> & value)
{
    char type = DataType::VECTOR;
    write((char *)&type, sizeof(char));
    int len = value.size();
    write(len);
    for (int i = 0; i < len; i++)
    {
        write(value[i]);
    }
}

template <typename T>
void DataStream::write(const std::list<T> & value)
{
    char type = DataType::LIST;
    write((char *)&type, sizeof(char));
    int len = value.size();
    write(len);
    for (auto it = value.begin(); it != value.end(); it++)
    {
        write((*it));
    }
}

template <typename K, typename V>
void DataStream::write(const std::map<K, V> & value)
{
    char type = DataType::MAP;
    write((char *)&type, sizeof(char));
    int len = value.size();
    write(len);
    for (auto it = value.begin(); it != value.end(); it++)
    {
        write(it->first);
        write(it->second);
    }
}

template <typename T>
void DataStream::write(const std::set<T> & value)
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

void DataStream::write_args()
{
}

bool DataStream::read(char * data, int len)
{
    std::memcpy(data, (char *)&m_buf[m_pos], len);
    m_pos += len;
    return true;
}

bool DataStream::read(bool & value)
{
    if (m_buf[m_pos] != DataType::BOOL)
    {
        return false;
    }
    ++m_pos;
    value = m_buf[m_pos];
    ++m_pos;
    return true;
}

bool DataStream::read(char & value)
{
    if (m_buf[m_pos] != DataType::CHAR)
    {
        return false;
    }
    ++m_pos;
    value = m_buf[m_pos];
    ++m_pos;
    return true;
}

bool DataStream::read(int32_t & value)
{
    if (m_buf[m_pos] != DataType::INT32)
    {
        return false;
    }
    ++m_pos;
    value = *((int32_t *)(&m_buf[m_pos]));
    if (m_byteorder == ByteOrder::BigEndian)
    {
        char * first = (char *)&value;
        char * last = first + sizeof(int32_t);
        std::reverse(first, last);
    }
    m_pos += 4;
    return true;
}

bool DataStream::read(int64_t & value)
{
    if (m_buf[m_pos] != DataType::INT64)
    {
        return false;
    }
    ++m_pos;
    value = *((int64_t *)(&m_buf[m_pos]));
    if (m_byteorder == ByteOrder::BigEndian)
    {
        char * first = (char *)&value;
        char * last = first + sizeof(int64_t);
        std::reverse(first, last);
    }
    m_pos += 8;
    return true;
}

bool DataStream::read(float & value)
{
    if (m_buf[m_pos] != DataType::FLOAT)
    {
        return false;
    }
    ++m_pos;
    value = *((float *)(&m_buf[m_pos]));
    if (m_byteorder == ByteOrder::BigEndian)
    {
        char * first = (char *)&value;
        char * last = first + sizeof(float);
        std::reverse(first, last);
    }
    m_pos += 4;
    return true;
}

bool DataStream::read(double & value)
{
    if (m_buf[m_pos] != DataType::DOUBLE)
    {
        return false;
    }
    ++m_pos;
    value = *((double *)(&m_buf[m_pos]));
    if (m_byteorder == ByteOrder::BigEndian)
    {
        char * first = (char *)&value;
        char * last = first + sizeof(double);
        std::reverse(first, last);
    }
    m_pos += 8;
    return true;
}

bool DataStream::read(string & value)
{
    if (m_buf[m_pos] != DataType::STRING)
    {
        return false;
    }
    ++m_pos;
    int len;
    read(len);
    if (len < 0)
    {
        return false;
    }
    value.assign((char *)&(m_buf[m_pos]), len);
    m_pos += len;
    return true;
}

bool DataStream::read(Serializable & value)
{
    return value.unserialize(*this);
}

template <typename T>
bool DataStream::read(std::vector<T> & value)
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

template <typename T>
bool DataStream::read(std::list<T> & value)
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

template <typename K, typename V>
bool DataStream::read(std::map<K, V> & value)
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

template <typename T>
bool DataStream::read(std::set<T> & value)
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
        T v;
        read(v);
        value.insert(v);
    }
    return true;
}

template <typename T, typename ...Args>
bool DataStream::read_args(T & head, Args&... args)
{
    read(head);
    return read_args(args...);
}

bool DataStream::read_args()
{
    return true;
}

const char * DataStream::data() const
{
    return m_buf.data();
}

int DataStream::size() const
{
    return m_buf.size();
}

void DataStream::clear()
{
    m_buf.clear();
}

void DataStream::reset()
{
    m_pos = 0;
}

void DataStream::save(const string & filename)
{
    ofstream fout(filename);
    fout.write(data(), size());
    fout.flush();
    fout.close();
}

void DataStream::load(const string & filename)
{
    ifstream fin(filename);
    stringstream ss;
    ss << fin.rdbuf();
    const string & str = ss.str();
    m_buf.clear();
    reserve(str.size());
    write(str.data(), str.size());
}

DataStream & DataStream::operator << (bool value)
{
    write(value);
    return *this;
}

DataStream & DataStream::operator << (char value)
{
    write(value);
    return *this;
}

DataStream & DataStream::operator << (int32_t value)
{
    write(value);
    return *this;
}

DataStream & DataStream::operator << (int64_t value)
{
    write(value);
    return *this;
}

DataStream & DataStream::operator << (float value)
{
    write(value);
    return *this;
}

DataStream & DataStream::operator << (double value)
{
    write(value);
    return *this;
}

DataStream & DataStream::operator << (const char * value)
{
    write(value);
    return *this;
}

DataStream & DataStream::operator << (const string & value)
{
    write(value);
    return *this;
}

DataStream & DataStream::operator << (const Serializable & value)
{
    write(value);
    return *this;
}

template <typename T>
DataStream & DataStream::operator << (const std::vector<T> & value)
{
    write(value);
    return *this;
}

template <typename K, typename V>
DataStream & DataStream::operator << (const std::map<K, V> & value)
{
    write(value);
    return *this;
}

template <typename T>
DataStream & DataStream::operator << (const std::set<T> & value)
{
    write(value);
    return *this;
}

DataStream & DataStream::operator >> (bool & value)
{
    read(value);
    return *this;
}

DataStream & DataStream::operator >> (char & value)
{
    read(value);
    return *this;
}

DataStream & DataStream::operator >> (int32_t & value)
{
    read(value);
    return *this;
}

DataStream & DataStream::operator >> (int64_t & value)
{
    read(value);
    return *this;
}

DataStream & DataStream::operator >> (float & value)
{
    read(value);
    return *this;
}

DataStream & DataStream::operator >> (double & value)
{
    read(value);
    return *this; 
}

DataStream & DataStream::operator >> (string & value)
{
    read(value);
    return *this; 
}

DataStream & DataStream::operator >> (Serializable & value)
{
    read(value);
    return *this;
}

template <typename T>
DataStream & DataStream::operator >> (std::vector<T> & value)
{
    read(value);
    return *this;
}

template <typename T>
DataStream & DataStream::operator >> (std::list<T> & value)
{
    read(value);
    return *this;
}

template <typename K, typename V>
DataStream & DataStream::operator >> (std::map<K, V> & value)
{
    read(value);
    return *this;
}

template <typename T>
DataStream & DataStream::operator >> (std::set<T> & value)
{
    read(value);
    return *this;
}

}
}