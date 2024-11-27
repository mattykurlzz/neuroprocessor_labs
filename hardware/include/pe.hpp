#ifndef _PE_H
#define _PE_H

#include "accelerator.hpp"
#include "defines.hpp"
#include "types.hpp"

#include <systemc>

using namespace sc_core;

SC_MODULE(PE) {
    sc_out<bool> calc_ready_o; // to TU+

    SC_HAS_PROCESS(PE);

    PE(sc_module_name name);
    ~PE();

    /* PE to ACCELERATOR */
    ACCELERATOR* accelerator_threads[PE_ACCELERATED_THREADS];
    void load_array_result();
};

#endif /*PE.hpp*/