#include "accelerator.hpp"
#include <systemc>

ACCELERATOR::ACCELERATOR(sc_core::sc_module_name name) : sc_core::sc_module(name) {
    SC_THREAD(main_thread);
    sensitive << start_i.pos();
    dont_initialize();
}

void
ACCELERATOR::main_thread() {
    fixed32_t sum;
    fixed32_t result;
    while (true) {
        for (int i = 0; i < ACCELERATOR_THREADS; ++i) {
            sum += energy_i[i].read() * weight_i[i].read();
        }
        if (last_iter_i.read() != 1) {
            wait(); // sum untill last iter flag passed
        } else {
            // break;
            value_o.write(sum / (sum.abs() + 1));
        }
    }
}