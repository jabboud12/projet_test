#include <stdint.h>

#include "component.h"
#include "bit.h"
#include "cpu.h"
#include "bus.h"

#include "timer.h"

int timer_init(gbtimer_t* timer, cpu_t* cpu)
{
    if (timer == NULL || cpu == NULL) {
        return ERR_BAD_PARAMETER;
    }
    int err = cpu_init(cpu);
    if (err != ERR_NONE) {
        return err;
    }
    *(timer->cpu) = *cpu;
    timer->counter = (uint16_t) 0;
    return ERR_NONE;
}

bit_t timer_state(gbtimer_t* timer)
{
    if (timer == NULL) {
        return timer;
    }
    // TAC[2] & [TAC[1 downto 0]]
    // timer->cpu->bus[]
    data_t tac = cpu_read_at_id(timer->cpu, REG_TAC);
    bit_t x = bit_get((uint8_t) tac, 2);
    data_t div = cpu_read_at_id(timer->cpu, REG_DIV);
    bit_t y = 0;

    //fixme bigtime
    switch(tac & 0x11) {
    case 0b00:
        //9
        y = bit_get((uint8_t) div, 1);
        break;
    case 0b01:
        //3
        y = bit_get((uint8_t) timer->counter, 3);
        break;
    case 0b10:
        //5
        y = bit_get((uint8_t) timer->counter, 5);
        break;
    case 0b11:
        //7
        y = bit_get((uint8_t) timer->counter, 7);
        break;
    default:
        break;
    }
    return x & y;



}

int timer_incr_if_state_change(gbtimer_t* timer, bit_t old_state)
{
    if (timer == NULL) {
        return timer;
    }

    data_t tima = cpu_read_at_id(timer->cpu, REG_TIMA);
    if (old_state && !timer_state(timer)) { // &?
        ++tima;
    }
    if (tima > 0xFF) {
        //todo raise timer interrupt
        //???
        tima = cpu_read_at_idx(timer->cpu, REG_TMA);
    }
    int err = cpu_write_at_idx(timer->cpu, REG_TIMA, tima);
    return err;


}

int timer_cycle(gbtimer_t* timer)
{
    if (timer == NULL) {
        return ERR_BAD_PARAMETER;
    }
    // Get current state of principal timer
    bit_t current_state = timer_state(timer);

    int err = timer_incr_if_state_change(timer, current_state);//twice??
    if (err != ERR_NONE) {
        return err;
    }

    // Increment counter by 4 (A cycle is 4 clock ticks)
    timer->counter += 4;

    // copy 8 MSB from timer principal counter to DIV register
    err = cpu_write_at_idx(timer->cpu, REG_DIV, msb8(timer->counter));
    if (err != ERR_NONE) {
        return err;
    }

    current_state = timer_state(timer);
    return timer_incr_if_state_change(timer, current_state); // twice??
}

int timer_bus_listener(gbtimer_t* timer, addr_t addr)
{
    if (timer == NULL) {
        return ERR_BAD_PARAMETER;
    }

    bit_t current_state = timer_state(timer);

    switch(addr) {
    case REG_DIV:
        timer_incr_if_state_change(timer, current_state);
        timer->counter = 0; // fixme or at bus
        current_state = timer_state(timer);
        timer_incr_if_state_change(timer, current_state);
        break;
    case REG_TAC:
        timer_incr_if_state_change(timer, current_state);
        break;
    default :
        return ERR_BAD_PARAMETER;
        break;
    }
}
