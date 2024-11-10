#pragma once

#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "Global.h"
#include "Date.h"
#include "Macros.h"

namespace minirisk {

// streaming separator
const char separator = ';';

//
// Overload classes
//

struct my_ofstream
{
    my_ofstream(const string& fn)
        : m_of(fn)
    {
    }
    void endl() { m_of << std::endl; }
    void close() { m_of.close(); }
    std::ofstream m_of;
};

struct my_ifstream
{
    my_ifstream(const string& fn)
        : m_if(fn)
    {
        MYASSERT(!m_if.fail(), "Could not open file " << fn);
    }

    bool read_line()
    {
        std::getline(m_if, m_line);  // read a line and store it in m_line
        m_line_stream.str(m_line);   // associate a string stream with m_line
        return m_line.length() > 0;
    }

    inline string read_token()
    {
        string tmp;
        std::getline(m_line_stream, tmp, separator);
        return tmp;
    }

private:
    string m_line;
    std::istringstream m_line_stream;
    std::ifstream m_if;
};

//
// Generic file streamer
//

template <typename T>
inline my_ifstream& operator>>(my_ifstream& is, T& v)
{
    string tmp = is.read_token();
    std::istringstream(tmp) >> v;
    return is;
}

template <typename T>
inline my_ofstream& operator<<(my_ofstream& os, const T& v)
{
    os.m_of << v << separator;
    return os;
}

//
// Double streamer overloads
//


// when saving a double to a file in text format, use the maximum possible precision
// re-interpreted as a 64 bits integer and use base 16 (hexadecimal)
inline my_ofstream& operator<<(my_ofstream& os, double v)
{
    union
    {
        double d;
        uint64_t u;
    } tmp;
    tmp.d = v;

    // os.m_of << std::hex << tmp.u << separator;
    os.m_of << std::hex << std::setfill('0') << std::setw(16) << tmp.u << separator;

    return os;
}

// when reading a double from a file in text format, read the hexadecimal format integer number
// and re-interpret it as a double
inline my_ifstream& operator>>(my_ifstream& is, double& v)
{
    // // read the hexadecimal string and then convert the string from hexadecimal to uint64_t
    // string hex_string = is.read_token(); 
    // uint64_t int_format;
    // std::istringstream(hex_string) >> std::hex >> int_format;

    std::string hex_string;
    is >> hex_string; // read hexadecimal string from stream

    // re-interpret uint64_t as a double
    union
    {
        double d;
        uint64_t u;
    } tmp;

    // convert hexadecimal string into uint64_t
    std::istringstream hex_stream(hex_string);
    hex_stream >> std::hex >> tmp.u;

    // assign double value to v
    v = tmp.d;
    return is;
}


//
// Vector streamer overloads
//

template <typename T, typename A>
inline std::ostream& operator<<(std::ostream& os, const std::vector<T, A>& v)
{
    for (const T& i : v)
        os << i << " ";
    return os;
}

template <typename T>
inline my_ofstream& operator<<(my_ofstream& os, const std::vector<T>& v)
{
    os << v.size();
    for (const T& i : v)
        os << i;
    return os;
}


template <typename T, typename A>
inline my_ifstream& operator>>(my_ifstream& is, std::vector<T, A>& v)
{
    size_t sz;
    is >> sz; // read size (this will call the general overload for >>)
    v.resize(sz);
    for (size_t i = 0; i < sz; ++i)
        is >> v[i];  // read i-th value
    return is;
}


//
// Date streamer overloads
//

inline std::ostream& operator<<(std::ostream& os, const Date& d)
{
    os << d.to_string(true);
    return os;
}

inline my_ofstream& operator<<(my_ofstream& os, const Date& d)
{
    os << d.to_string(false);
    return os;
}

inline my_ifstream& operator>>(my_ifstream& is, Date& v)
{
    string tmp;
    is >> tmp;
    unsigned y = std::atoi(tmp.substr(0, 4).c_str());
    unsigned m = std::atoi(tmp.substr(4, 2).c_str());
    unsigned d = std::atoi(tmp.substr(6, 2).c_str());
    v.init(y, m, d);
    return is;
}

} // namespace minirisk

