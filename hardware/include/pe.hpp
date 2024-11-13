#ifndef _PE_H
#define _PE_H

#include "accelerator.hpp"
#include "defines.hpp"
#include "types.hpp"

#include <systemc>

SC_MODULE(PE) {
    sc_core::sc_in<float> weight_i[PE_ACCELERATED_THREADS][ACCELERATOR_THREADS]; // from TU+
    sc_core::sc_in<float> data_i[ACCELERATOR_THREADS];                           // from TU+
    sc_core::sc_in<bool> start_i;                                                // from TU+
    sc_core::sc_in<bool> last_iter_i;                                            // from TU+
    sc_core::sc_in<bool> data_ready_i;                                           // from ACC+

    sc_core::sc_out<float> value_o[PE_ACCELERATED_THREADS]; // to TU+
    sc_core::sc_out<bool> result_ready_o;                   // to TU+

    SC_HAS_PROCESS(PE);

    PE(sc_core::sc_module_name name);
    ~PE();

    /* PE to ACCELERATOR */
    sc_core::sc_out<float> value_i[PE_ACCELERATED_THREADS]; // from ACCELERATOR+
    ACCELERATOR* accelerator_threads[PE_ACCELERATED_THREADS];
    void load_array_result();
};

#endif /*PE.hpp*/