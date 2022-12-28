#pragma once
#include <stdint.h>
#include <stddef.h>
#include "bitmap.h"
#include "mem.h"
#include "util.h"

namespace memory
{
    class page_alloc_t
    {
    private:
        page_alloc_t(uint8_t *bitmap_buffer, size_t cap);
        friend void init_page_alloc();
        phy::bitmap_t bit_map;
        void reserve_page(void *addr);
        void unreserve_page(void *addr);
        void reserve_pages(void *addr, uint64_t num_pages);
        void unreserve_pages(void *addr, uint64_t num_pages);

    public:
        void free_page(void *addr);
        void lock_page(void *addr);
        void lock_pages(void *addr, uint64_t num_pages);
        void free_pages(void *addr, uint64_t num_pages);
        void *get_free_page();
    };
} // namespace memory
