#include "mem.h"

static volatile limine_memmap_request mem_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0};
static volatile limine_hhdm_request hhdm_request{
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0};
namespace memory
{

    static mem_info_t mem_info;
    static uint64_t total_mem;
    static uint64_t total_usable_mem;
    static uint64_t total_reserved_mem;
    static frame_buffer_t frame_buffer;
    static page_alloc_t *global_alloc_ptr;
    void init_memmap_info()
    {
        // Ensure we got a meminfo
        if (mem_request.response == NULL)
        {
            print("no info on memory found!\n");
            hlt();
        }
        if (hhdm_request.response == NULL)
        {
            print("no info on hhdm found!\n");
            hlt();
        }
        auto response = mem_request.response;
        auto entries = response->entries;
        auto entry_count = response->entry_count;
        mem_info.entry_count = entry_count;
        mem_info.hhdm_offset = hhdm_request.response->offset;
        for (uint64_t i = 0; i < entry_count; i++)
        {
            auto entry = *entries[i];
            mem_info.entries[i].length = entry.length;
            mem_info.entries[i].base = entry.base;
            mem_info.entries[i].type = entry.type;
            total_mem += entry.length;
            switch (entry.type)
            {
            case LIMINE_MEMMAP_USABLE:
                total_usable_mem += entry.length;
                break;
            case LIMINE_MEMMAP_RESERVED:
                total_reserved_mem += entry.length;
                break;
            case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
                break;
            case LIMINE_MEMMAP_ACPI_NVS:
                break;
            case LIMINE_MEMMAP_BAD_MEMORY:
                break;
            case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
                break;
            case LIMINE_MEMMAP_KERNEL_AND_MODULES:
                break;
            case LIMINE_MEMMAP_FRAMEBUFFER:
                frame_buffer.buffer = (void *)entry.base;
                frame_buffer.size = entry.length;
                break;
            default:
                break;
            }
        }
    }
    const mem_info_t &get_memory_map_entry()
    {
        return mem_info;
    }
    uint64_t get_total_usable_mem()
    {
        return total_usable_mem;
    }
    uint64_t get_total_reserved_mem()
    {
        return total_reserved_mem;
    }
    uint64_t get_total_mem()
    {

        return total_mem;
    }
    frame_buffer_t get_frame_buffer()
    {
        return frame_buffer;
    }
    void print_mem_info(const mem_info_t &info_ptr)
    {
        auto &info = info_ptr;
        printf("meminfo:\n entry count = %i total mem = %i", info.entry_count, memory::get_total_mem());
        uint64_t total_usable = 0;
        for (uint64_t i = 0; i < info.entry_count; i++)
        {

            auto entry = info.entries[i];
            auto length = entry.length;
            auto type = entry.type;
            auto base = entry.base;
            if (type == LIMINE_MEMMAP_KERNEL_AND_MODULES)
                base += info_ptr.hhdm_offset;
            print("\n");

            switch (type)
            {
            case LIMINE_MEMMAP_USABLE:
                total_usable += length;
                print("type: LIMINE_MEMMAP_USABLE ");
                break;
            case LIMINE_MEMMAP_RESERVED:
                print("type: LIMINE_MEMMAP_RESERVED ");
                break;
            case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
                print("type: LIMINE_MEMMAP_ACPI_RECLAIMABLE ");
                break;
            case LIMINE_MEMMAP_ACPI_NVS:
                print("type: LIMINE_MEMMAP_ACPI_NVS ");

                break;
            case LIMINE_MEMMAP_BAD_MEMORY:
                print("type: LIMINE_MEMMAP_BAD_MEMORY ");

                break;
            case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
                print("type: LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE ");

                break;
            case LIMINE_MEMMAP_KERNEL_AND_MODULES:
                print("type: LIMINE_MEMMAP_KERNEL_AND_MODULES ");

                break;
            case LIMINE_MEMMAP_FRAMEBUFFER:
                print("type: LIMINE_MEMMAP_FRAMEBUFFER ");

                break;
            default:
                break;
            }
            print("base: ");
            print((void *)base);
            print(" length:");

            print(length / 1000);
            print("Kb\n");
        }
    }

    page_alloc_t &get_global_alloc()
    {
        return (*global_alloc_ptr);
    }

    void init_page_alloc()
    {
        auto entries = memory::get_memory_map_entry();

        // determine size of bitmap
        auto total_size = memory::get_total_mem();
        uint64_t bitmap_capacity = (total_size / PAGE_SIZE) + 1;
        // find memory segment for bitmap
        uint8_t *buffer = nullptr;
        for (size_t i = 0; i < entries.entry_count; i++)
        {
            auto entry = entries.entries[i];
            if ((entry.type == LIMINE_MEMMAP_USABLE) && (entry.length > bitmap_capacity))
            {
                buffer = (uint8_t *)entry.base;
            }
        }
        if (buffer == nullptr)
        {
            printf("memory segment not found for bitmap!!\n");
            hlt();
        }
        static page_alloc_t global_allocator(buffer, bitmap_capacity);
        global_allocator.lock_page((void *)nullptr);
        for (size_t i = 0; i < entries.entry_count; i++)
        {
            auto entry = entries.entries[i];
            if (entry.type == LIMINE_MEMMAP_USABLE)
                global_allocator.free_pages((void *)entry.base, entry.length / PAGE_SIZE);

            if ((entry.type == LIMINE_MEMMAP_KERNEL_AND_MODULES) ||
                (entry.type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE) ||
                (entry.type == LIMINE_MEMMAP_BAD_MEMORY) ||
                (entry.type == LIMINE_MEMMAP_ACPI_NVS) ||
                (entry.type == LIMINE_MEMMAP_ACPI_RECLAIMABLE) ||
                (entry.type == LIMINE_MEMMAP_FRAMEBUFFER))
                global_allocator.lock_pages((void *)entry.base, entry.length / PAGE_SIZE);

            if (entry.type == LIMINE_MEMMAP_RESERVED)
                global_allocator.reserve_pages((void *)entry.base, entry.length / PAGE_SIZE);
        }

        // lock pages of bitmap
        global_allocator.lock_pages(buffer, (bitmap_capacity / PAGE_SIZE) + 1);
        global_alloc_ptr = &global_allocator;
    }
}
// namespace memory

namespace memory
{
    page_alloc_t::page_alloc_t(uint8_t *bitmap_buffer, size_t cap) : bit_map(bitmap_buffer, cap)
    {
        memset(bit_map.data(), bit_map.size(), FREE);
    }
    void page_alloc_t::free_page(void *addr)
    {
        uint64_t index = (uint64_t)addr / PAGE_SIZE;
        bit_map.set_bit(index, FREE);
        total_usable_mem += PAGE_SIZE;
    }

    void page_alloc_t::lock_page(void *addr)
    {
        uint64_t index = (uint64_t)addr / PAGE_SIZE;
        bit_map.set_bit(index, NOT_FREE);
        total_usable_mem -= PAGE_SIZE;
    }
    void page_alloc_t::reserve_page(void *addr)
    {
        uint64_t index = (uint64_t)addr / PAGE_SIZE;
        bit_map.set_bit(index, NOT_FREE);
        total_reserved_mem += PAGE_SIZE;
    }

    void page_alloc_t::unreserve_page(void *addr)
    {
        uint64_t index = (uint64_t)addr / PAGE_SIZE;
        bit_map.set_bit(index, FREE);
        total_reserved_mem -= PAGE_SIZE;
    }

    void page_alloc_t::free_pages(void *addr, uint64_t num_pages)
    {
        uint8_t *baddr = (uint8_t *)addr;
        for (size_t i = 0; i < num_pages; i++)
        {
            free_page((void *)baddr);
            baddr += PAGE_SIZE;
        }
    }
    void *page_alloc_t::get_free_page()
    {
        for (size_t i = 0; i < bit_map.size() * 8; i++)
        {
            if (bit_map[i] == FREE)
            {
                lock_page((void *)(i * PAGE_SIZE));
                return (void *)(i * PAGE_SIZE);
            }
        }
        return nullptr;
    }
    void page_alloc_t::lock_pages(void *addr, uint64_t num_pages)
    {
        uint8_t *baddr = (uint8_t *)addr;
        for (size_t i = 0; i < num_pages; i++)
        {
            lock_page((void *)baddr);
            baddr += PAGE_SIZE;
        }
    }
    void page_alloc_t::reserve_pages(void *addr, uint64_t num_pages)
    {
        uint8_t *baddr = (uint8_t *)addr;
        for (size_t i = 0; i < num_pages; i++)
        {
            reserve_page((void *)baddr);
            baddr += PAGE_SIZE;
        }
    }
    void page_alloc_t::unreserve_pages(void *addr, uint64_t num_pages)
    {
        uint8_t *baddr = (uint8_t *)addr;
        for (size_t i = 0; i < num_pages; i++)
        {
            unreserve_page((void *)baddr);
            baddr += PAGE_SIZE;
        }
    }

}
