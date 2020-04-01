#include <stdint.h>
#include <stdbool.h>

#include "memory.h" // addr_t and data_t
#include "component.h"
#include "bus.h"
#include "error.h"

int bus_remap(bus_t bus, component_t *c, addr_t offset)
{
    addr_t start = c->start;
    addr_t end = c->end;
    memory_t mem = c->mem;

    if (end - start + offset < mem.size)
    {
        return ERR_ADDRESS;
    }

    for (int i = 0; i <= end - start; i++)
    {
        bus[start + i] = mem.memory[offset + i];
    }

    return ERR_NONE;
}
