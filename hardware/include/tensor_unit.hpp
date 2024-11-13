#ifndef _TENSOR_UNIT_HPP
#define _TENSOR_UNIT_HPP

#include "accelerator.hpp"
#include "control_signal_unit.hpp"
#include "defines.hpp"
#include "pe.hpp"
#include "types.hpp"

#include <systemc>

SC_MODULE(TENSOR_UNIT) {
    sc_core::sc_signal<bool> start_o;           // from CU to CSU
    sc_core::sc_signal<int> input_cnt_o;        // from CU to SCU
    sc_core::sc_signal<int> hidden_layer_cnt_o; // from CU to SCU
    sc_core::sc_signal<int> output_cnt_o;       // from CU to SCU
    sc_core::sc_signal<int> neurons_in_layer_o; // from CU to SCU

    /* internals */
    sc_core::sc_out<bool> data_ready_o; // from TU to CSU+
    sc_core::sc_in<bool> load_data_i;   // from CSU to PE+
    sc_core::sc_in<bool> finish_i;      // from CSU
    sc_core::sc_in<bool> busy_i;        // from CSU+ to CU

    sc_core::sc_out<float> pe_inputs_o[PE_THREADS][PE_ACCELERATED_THREADS][ACCELERATOR_THREADS]; // from TU+ to PE+
    sc_core::sc_out<float> pe_data_o[PE_THREADS][ACCELERATOR_THREADS];                           // from TU+ to PE+
    sc_core::sc_in<float> pe_value_i[PE_THREADS][ACCELERATOR_THREADS];                           // from PE+ to TU+

    PE* processing_elements[PE_THREADS];
    CONTROL_SIGNAL_UNIT* control_signal_unit;
    sc_core::sc_fifo<float> local_input_data;
    sc_core::sc_fifo<float> local_next_input_data;
    float local_weight_memory[PE_THREADS][PE_ACCELERATED_THREADS][INPUT_DATA_SIZE];
    float local_iter_weight_memory[PE_THREADS][PE_ACCELERATED_THREADS][ACCELERATOR_THREADS];

    int weights_ready_flag;

    SC_HAS_PROCESS(TENSOR_UNIT);

    TENSOR_UNIT(sc_core::sc_module_name name);
    ~TENSOR_UNIT();

    void load_local_input_data();
    void load_local_weight_data();
    void transfer_local_weights_to_pe();
    void transfer_pe_to_next();
    void load_result();
    void init();
};

#endif