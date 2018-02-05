#include <kernel/memory.h>
#include <string.h>

struct {
    mem_page_t *pages;
    mem_page_t *pg_free;
    uintptr_t  end;

    size_t pages_amt;
} mem;

void mem_init(uint32_t *mp, void *phys_base, void *phys_free) {
    static size_t npage_mem = 0;
    mem.end = MEM_KERN_BASE+(uintptr_t)phys_free;

    struct smap_ {
        uint64_t base, length;
        uint32_t type;
    }__attribute__((packed)) *smap;

    while (mp[0] != 0x9001)
        mp += mp[1]+2;
    
    for (smap = (struct smap_ *)(mp+1); smap < (struct smap_ *)((char *)mp+mp[1]+2*4); ++smap) {
        if (smap->type == 1 && smap->length != 0) {
            if (smap->base == 0) {
                
            }
        }
    }
}
