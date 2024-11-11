#ifndef _ACCELERATOR_H
#define _ACCELERATOR_H

#include "defines.hpp"
#include "types.hpp"

#include <systemc>

SC_MODULE(ACCELERATOR) {
    sc_core::sc_in<fixed32_t> energy_i[ACCELERATOR_THREADS];
    sc_core::sc_in<fixed32_t> weight_i[ACCELERATOR_THREADS];
    sc_core::sc_in<bool> start_i;
    sc_core::sc_in<bool> last_iter_i;
    sc_core::sc_out<fixed32_t> value_o;

    SC_HAS_PROCESS(ACCELERATOR);

    ACCELERATOR(sc_core::sc_module_name name);

    void main_thread();
};

#endif /*_ACCELERATOR_H*/