#pragma once

#include <vector>
#include <cstring>
#include <cassert>
#include <stdexcept>

//#define NDEBUG // removes assertions
#define MAX_PACKET_LENGTH 32768

namespace bango { namespace network {

    class packet
    {
        char* m_buffer;

        char* m_header;
        char* m_begin;
        char* m_end;

        void reset_size()
        {
            unsigned short size = (unsigned short)(m_end - m_begin) + 3;
            (m_begin-3)[0] = ((char*)&size)[0];
            (m_begin-3)[1] = ((char*)&size)[1];
            (m_begin-3)[2] = m_header[2];
            m_header = m_begin-3;
        }

    public:
        packet(unsigned char type) : packet()
        {
            m_header[2] = type;
        }

        packet()
        {
            // That's hell lot of data to send just 1 byte!
            // TODO: Come up with solution
            m_buffer = new char[MAX_PACKET_LENGTH];
            m_header = m_buffer;
            m_end = m_begin = m_buffer+3;
            m_header[0]=3;
            m_header[1]=0;
            m_header[2]=0;
        }

        // This constructor copies data. Maybe work on vector data?
        packet(const std::vector<char>& buffer) : packet()
        {
            std::memcpy(m_buffer, buffer.data(), buffer.size());
            m_end = m_buffer+buffer.size();
            assert(size() == buffer.size());
        }

        ~packet()
        {
            delete[] m_buffer;
        }

        unsigned short size() const 
        {
            assert(((unsigned short*)m_header)[0] == m_end-m_header);
            return ((unsigned short*)m_header)[0]; 
        }

        unsigned char type() const { return (unsigned char)(m_header[2]);}
        bool empty() const { return m_begin == m_end; }

        char* header() const { return m_header; }
        char* begin() const { return m_begin; }
        char* end() const { return m_end; }

        const std::vector<char> buffer() const { return std::vector<char>(header(), end()); }

        packet& change_type(unsigned char type) { m_header[2] = (char)type; return *this; }

        template<typename T>
        void push(const T& value);

        template<typename T>
        T pop();

        void push_str(const std::string& str) { push_str(str.c_str()); }
        void push_str(const char* str);
        std::string pop_str();

        template<typename T>
        friend packet& operator>> (packet& lhs, T& rhs) { rhs = lhs.pop<T>(); return lhs; }
        friend packet& operator>> (packet& lhs, std::string& rhs) { rhs = lhs.pop_str(); return lhs; }

        friend packet& operator<< (packet& lhs, std::string& rhs) { lhs.push_str(rhs); return lhs; }
        friend packet& operator<< (packet& lhs, const char* rhs) { lhs.push_str(rhs); return lhs; }

        void merge(const packet& p);
        friend packet& operator<< (packet& lhs, const packet& rhs) { lhs.merge(rhs); return lhs; }
    };

    template<typename T>
    inline void packet::push(const T& value)
    {
        if ((m_end-m_buffer) + sizeof(T) > MAX_PACKET_LENGTH)
            throw std::runtime_error("packet overflow");

        memcpy(m_end, (char*)&value, sizeof(T));
        m_end += sizeof(T);

        reset_size();
    }

    template<typename T>
    inline T packet::pop()
    {
        if (m_end-m_begin < sizeof(T))
            throw std::runtime_error("packet no more data");

        T result;
        memcpy(&result, m_begin, sizeof(T));
        m_begin += sizeof(T);

        reset_size();

        return result;
    }
    
}}