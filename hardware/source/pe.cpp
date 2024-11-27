#include "pe.hpp"
#include <systemc>

PE::PE(sc_core::sc_module_name name) : sc_module(name) {
    printf("PE constructor\n");

    /* accelerators init */
    for (int i = 0; i < PE_ACCELERATED_THREADS; i++) {
        sc_core::sc_module_name str(std::string("ACCELERATOR" + std::to_string(i)).c_str());
        accelerator_threads[i] = new ACCELERATOR(str);
    }

    SC_THREAD(load_array_result);
    sensitive << accelerator_threads[0]->data_ready_o.pos();
    dont_initialize();
}

PE::~PE() {
    printf("pe destructor\n");
    for (int i = 0; i < PE_ACCELERATED_THREADS; i++) {
        delete accelerator_threads[i];
    }
}

void
PE::load_array_result() {
    while (true) {
        printf("PE load_array_result\n");
        wait(1, sc_core::SC_NS);
        calc_ready_o.write(1);
        wait(1, sc_core::SC_NS);
        calc_ready_o.write(0);
        wait();
    }
}