#ifndef _ACCELERATOR_H
#define _ACCELERATOR_H

#include "defines.hpp"
#include "types.hpp"

#include <systemc>

SC_MODULE(ACCELERATOR) {
    sc_core::sc_in<bool> start_i;                        // from PE+
    sc_core::sc_in<float> data_i[ACCELERATOR_THREADS];   // from PE+
    sc_core::sc_in<float> weight_i[ACCELERATOR_THREADS]; // from PE+
    sc_core::sc_in<bool> last_iter_i;                    // from PE+

    sc_core::sc_out<float> value_o;     // to PE+
    sc_core::sc_out<bool> data_ready_o; // to PE+

    SC_HAS_PROCESS(ACCELERATOR);

    ACCELERATOR(sc_core::sc_module_name name);

    void main_thread();
};

#endif /*_ACCELERATOR_H*/