#pragma once

#include <stdint.h>
#include "multibootdef.h"
#include "Wrapper.h"
class bitmap
{
private:
    uint8_t *buffer;
    uint64_t m_size; // in bytes
    bitmap() {}

public:
    void set(uint64_t index, bool val)
    {
        auto buff_pos = index / 8;
        auto sub_pos = index % 8;
        auto &ele = buffer[buff_pos];
        ele |= (0x1 && val) << sub_pos;
    }
    bool get(uint64_t index)
    {
        auto buff_pos = index / (sizeof(uint8_t));
        auto sub_pos = index % 8;
        auto &ele = buffer[buff_pos];
        return ((0x1 << sub_pos) & ele);
    }
    friend class PageAllocator;
};