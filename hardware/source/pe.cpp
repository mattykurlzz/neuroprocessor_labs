#include "pe.hpp"
#include <systemc>

PE::PE(sc_core::sc_module_name name) : sc_module(name) {
    for (int i = 0; i < PE_ACCELERATED_THREADS; i++) {
        sc_core::sc_module_name str(std::string("ACCELERATOR" + std::to_string(i)).c_str());
        accelerator_threads[i] = new ACCELERATOR(str);

        for (int j = 0; j < ACCELERATOR_THREADS; j++) {
            accelerator_threads[i]->weight_i[j](weight_i[i][j]);
            accelerator_threads[i]->data_i[j](data_i[j]);
        }

        accelerator_threads[i]->start_i(start_i);
        accelerator_threads[i]->last_iter_i(last_iter_i);

        value_i[i](accelerator_threads[i]->value_o);
    }
    data_ready_i(accelerator_threads[0]->data_ready_o);

    SC_THREAD(load_array_result);
    sensitive << data_ready_i;
    dont_initialize();
}

PE::~PE() {
    for (int i = 0; i < PE_ACCELERATED_THREADS; i++) {
        delete accelerator_threads[i];
    }
}

void
PE::load_array_result() {
    while (true) {
        for (int i = 0; i < PE_ACCELERATED_THREADS; i++) {
            // value_o[i].write(value_i[i].read());
            value_o[i].write(value_i[i].read());
        }
        result_ready_o.write(1);
        wait();
        result_ready_o.write(0);
    }
}