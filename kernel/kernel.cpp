#include <stdint.h>
#include <stddef.h>
#include <limine.h>

#include "term.h"
#include "mem.h"

extern "C" void _start(void)
{
	init_term();
	memory::init_memmap_info();
	memory::print_mem_info(memory::get_memory_map_entry());
	memory::init_page_alloc();

	uint8_t *ptr0 = (uint8_t *)memory::get_global_alloc().get_free_page();
	uint8_t *ptr1 = (uint8_t *)memory::get_global_alloc().get_free_page();
	uint8_t *ptr2 = (uint8_t *)memory::get_global_alloc().get_free_page();
	uint8_t *ptr3 = (uint8_t *)memory::get_global_alloc().get_free_page();
	uint8_t *ptr4 = (uint8_t *)memory::get_global_alloc().get_free_page();

	printf("ptr = :%i", (uint64_t)ptr4 - (uint64_t)ptr0);
	hlt();
}
