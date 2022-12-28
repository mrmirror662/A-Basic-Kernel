#pragma once
#include <stdint.h>
#include <stddef.h>
#include "util.h"
namespace phy
{
    class bitmap_t
    {
    private:
        size_t m_cap;
        uint8_t *m_ext_buffer = nullptr;

    public:
        uint8_t *data() { return m_ext_buffer; }
        size_t size() { return m_cap; }
        bitmap_t(uint8_t *buffer, size_t cap) : m_ext_buffer(buffer), m_cap(cap) {}
        bool operator[](size_t index)
        {
            auto byte_index = index / 8;
            auto bit_index = index % 8;
            uint8_t byte = 0b10000000 >> bit_index;
            return ((m_ext_buffer[byte_index] & byte) > 0) ? (true) : (false);
        }
        void set_bit(size_t index, bool value)
        {
            auto byte_index = index / 8;
            auto bit_index = index % 8;
            uint8_t byte = 0b10000000 >> bit_index;
            m_ext_buffer[byte_index] &= ~byte;
            if (value)
                m_ext_buffer[byte_index] |= byte;
        }
    };
}