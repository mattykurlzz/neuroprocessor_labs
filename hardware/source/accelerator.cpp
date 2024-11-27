#include "accelerator.hpp"
#include <systemc>

ACCELERATOR::ACCELERATOR(sc_core::sc_module_name name)
    : sc_core::sc_module(name), start_i("start_i"), last_iter_i("last_iter_i"), value_o("value_o"),
      data_ready_o("data_ready_o") {

    printf("accelerator constructor\n");

    SC_THREAD(main_thread);
    sensitive << start_i.pos();
    dont_initialize();
}

void
ACCELERATOR::main_thread() {
    float sum = 0;
    float result = 0;
    while (true) {
        printf("accelerator main thread\n");
        for (int i = 0; i < ACCELERATOR_THREADS; ++i) {
            sum += data_i[i].read() * weight_i[i].read();
            // printf("sum += %f * %f = %f\n", data_i[i].read(), weight_i[i].read(), sum);
        }
        printf("\taccelerator intermid. sum = %f\n", sum);
        if (last_iter_i.read() != 1) {
            wait(1, sc_core::SC_NS);
            intermid_ready_o.write(1);
            wait(1, sc_core::SC_NS);
            intermid_ready_o.write(0);
        } else {
            // break;
            value_o.write(1. / (1. + exp(-sum)));
            printf("\taccelerator last iter. sum = %f\n", sum);
            wait(1, sc_core::SC_NS);
            data_ready_o.write(1);
            wait(1, sc_core::SC_NS);
            data_ready_o.write(0);
            sum = 0;
        }
        wait();
    }
}