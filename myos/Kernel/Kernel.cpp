#include <stdint.h>
#include "Print.h"
#include "multibootdef.h"
#include "Wrapper.h"
#include "PageAlloc.h"
extern void *multiboot_info_ptr;

void InitMemoryMap(MemMapInfo &meminfo)
{

    static bool init = false;

    uint32_t totalSize = *((uint32_t *)multiboot_info_ptr);
    multiboot_tag *tag = (multiboot_tag *)(multiboot_info_ptr + 8);
    Console c;
    if (init)
    {
        c.log("already initialized!\n\n");
        return;
    }

    while (tag->type != MULTIBOOT_TAG_TYPE_END)
    {
        if (tag->type == MULTIBOOT_TAG_TYPE_MMAP)
        {
            c.log("Initialising Physical Mememory Map \n");
            multiboot_tag_mmap *mmaptag = (multiboot_tag_mmap *)tag;
            auto mmap = mmaptag->entries;
            uint32_t counter = 0;
            for (; ((uintptr_t)mmap) < ((uintptr_t)tag + tag->size); mmap = (multiboot_mmap_entry *)(((uintptr_t)mmap) + mmaptag->entry_size))
            {
                meminfo.entries[counter].addr = mmap->addr;
                meminfo.entries[counter].len = mmap->len;
                meminfo.entries[counter].type = mmap->type;
                meminfo.m_totalSize += mmap->len;
                if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE)
                {
                    meminfo.m_usableSize += mmap->len;
                }
                counter++;
            }
            meminfo.m_size = counter - 1;
        }

        tag = (struct multiboot_tag *)((uint8_t *)tag + ((tag->size + 7) & ~7));
    }
    init = true;
}
void PrintMemMap(MemMapInfo &meminfo)
{
    Console c;

    for (uint32_t i = 0; i < meminfo.size(); i++)
    {
        c.log("addr: ");
        c.log((void *)meminfo[i].addr);
        c.log(" \nlength: ");
        {
            if (meminfo[i].len > 1000000)
            {
                c.log((int)(meminfo[i].len / 1000000));
                c.log(" MB");
            }
            else if (meminfo[i].len > 1000)
            {
                c.log((int)(meminfo[i].len / 1000));
                c.log(" KB");
            }
            else
            {
                c.log((int)meminfo[i].len);
            }
        }
        c.log("\ntype: ");
        switch (meminfo[i].type)
        {
        case MULTIBOOT_MEMORY_AVAILABLE:
            c.log("Available");
            break;
        case MULTIBOOT_MEMORY_RESERVED:
            c.log("Reserved");
            break;
        case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE:
            c.log("ACPI Reclaimable");
            break;
        case MULTIBOOT_MEMORY_BADRAM:
            c.log("Bad Ram");
            break;
        default:
            break;
        }
        c.log("\n\n");
    }
}

extern "C" void cpp_kstart()
{

    Console c;
    c.clear();
    c.log("control to c++ kernel!!\n");
    MemMapInfo meminfo;
    InitMemoryMap(meminfo);

    PageAllocator pageAlloc(meminfo);
    //  c.logf("free ram:%i \n used ram:%i \n reserved ram:%i", pageAlloc.GetFreeRam(), pageAlloc.GetUsedRam(), pageAlloc.GetReservedRam());
    while (true)
    {
        continue;
    }
}
