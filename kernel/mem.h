#pragma once
#include <stdint.h>
#include <stddef.h>
#include <limine.h>

#include "term.h"
#include "bitmap.h"
#include "pagealloc.h"

#define PAGE_SIZE 4096
#define FREE false
#define NOT_FREE true

#define NUM_PAGES(x) x / PAGE_SIZE

namespace memory
{

    struct mem_info_t
    {
        uint64_t hhdm_offset;
        limine_memmap_entry entries[256];
        uint64_t entry_count;
    };
    struct frame_buffer_t
    {
        void *buffer;
        uint64_t size;
    };
    void init_memmap_info();
    const mem_info_t &get_memory_map_entry();
    uint64_t get_total_usable_mem();
    uint64_t get_total_reserved_mem();
    uint64_t get_total_mem();
    frame_buffer_t get_frame_buffer();
    void print_mem_info(const mem_info_t &info_ptr);

    page_alloc_t &get_global_alloc();
    void init_page_alloc();

}