#include <stdint.h>
#include <stdio.h>

#include "component.h"
#include "bus.h"

#include "cartridge.h"

int cartridge_init_from_file(component_t* c, const char* filename)
{
    if (c == NULL || filename == NULL) {
        return ERR_BAD_PARAMETER;
    }
    FILE* input = NULL;
    input = fopen(filename, "r");

    if (input == NULL) {
        return ERR_IO;
    } else {
        // == 1??
        if( fread(c->mem->memory, sizeof(uint8_t), BANK_ROM_SIZE, input) != 32768) {
            fclose(filename);
            return ERR_IO;
        }
        c->mem->size = BANK_ROM_SIZE;
        if (c->mem->memory[CARTRIDGE_TYPE_ADDR] != 0) {
            return ERR_NOT_IMPLEMENTED;
        }
        fclose(filename);

    }
    return ERR_NONE;
}

int cartridge_init(cartridge_t* cartridge, const char* filename)
{
    if (cartridge == NULL || filename == NULL) {
        return ERR_NONE;
    }

    memset(cartridge, 0, sizeof(cartridge_t));

    int err = component_create(&cartridge->c, BANK_ROM_SIZE);
    if (err != ERR_NONE) {
        return err;
    }

    return cartridge_init_from_file(cartridge, filename);
}

int cartridge_plug(cartridge_t* ct, bus_t bus)
{
    // Plugs the cartridge to the bus
                                            //fixme
    int err = bus_forced_plug(bus, &ct->c, BANK_ROM0_START ,BANK_ROM0_START, BANK_ROM1_END);
    return err;
}

void cartridge_free(cartridge_t* ct)
{
    free(&ct->c);
}
