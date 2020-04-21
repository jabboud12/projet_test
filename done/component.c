#include <stdint.h>
#include <stdbool.h>

#include "memory.h"
#include "error.h"
#include "component.h"

int component_create(component_t* c, size_t mem_size) 
{   
    if (c == NULL || mem_size < 0) 
    {
        return ERR_BAD_PARAMETER;
    }

    c->start =0;
    c->end=0;
    if (mem_size == 0){
        c->mem = NULL;
        return ERR_NONE;
    }

    mem_create(c->mem, mem_size);
    return ERR_NONE;
}


void component_free(component_t* c)
{
    if ((c != NULL ) && (c->mem != NULL)){
        mem_free(c->mem);
        free(c->mem);

        c->mem = NULL;
        c->start = 0;
        c->end = 0;
    }
}