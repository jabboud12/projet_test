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

    // Check if the span of the zone is larger than the component's memory,
    // Or if the component's start address is bigger than its end
    if (end - start + offset >= mem.size)
    {
        return ERR_ADDRESS;
    }
    if (start > end || end == 0)
    {
        return ERR_BAD_PARAMETER;
    }

    for (int i = 0; i <= end - start; i++)
    {
        bus[start + i] = mem.memory[offset + i];
    }

    return ERR_NONE;
}

int bus_forced_plug(bus_t bus, component_t* c, addr_t start, addr_t end, addr_t offset)
{
    if (c == NULL)
    {
        return ERR_BAD_PARAMETER;
    }
    
    c->start = start;
    c->end = end;
    int err = bus_remap(bus, c, offset);

    if (err != ERR_NONE)
    {
        c->start = 0;
        c->end = 0;
        return err;
    }

    return ERR_NONE;
}

int bus_plug(bus_t bus, component_t* c, addr_t start, addr_t end)
{
    if (c == NULL)
    {
        return ERR_BAD_PARAMETER;
    }

    for (int i = start; i <= end; i++)
    {
        if (bus[i] != NULL)
        {
            return ERR_ADDRESS;
        }   
    }    

    return bus_forced_plug(bus, c, start, end, 0);
}
