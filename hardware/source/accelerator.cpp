#include "accelerator.hpp"
#include <systemc>

ACCELERATOR::ACCELERATOR(sc_core::sc_module_name name) : sc_core::sc_module(name) {
    SC_THREAD(main_thread);
    sensitive << start_i;
    dont_initialize();
}

void
ACCELERATOR::main_thread() {
    float sum;
    float result;
    while (true) {
        data_ready_o.write(0);

        for (int i = 0; i < ACCELERATOR_THREADS; ++i) {
            sum += data_i[i].read() * weight_i[i].read();
        }
        if (last_iter_i.read() != 1) {
        } else {
            // break;
            value_o.write(sum / (abs(sum) + 1));
            data_ready_o.write(1);
        }
        wait();
    }
}