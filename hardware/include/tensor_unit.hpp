#ifndef _TENSOR_UNIT_HPP
#define _TENSOR_UNIT_HPP

#include "accelerator.hpp"
#include "control_signal_unit.hpp"
#include "defines.hpp"
#include "pe.hpp"
#include "types.hpp"

#include <systemc>
using namespace sc_core;

SC_MODULE(TENSOR_UNIT) {
    std::vector<float> shared_memory_data;
    std::vector<std::vector<std::vector<float>>> shared_memory_weight;
    std::vector<float> shared_memory_result;

    int input_cnt, hidden_layer_cnt, output_cnt, neurons_in_layer;
    sc_event int_data_ready;

    /* internals */
    sc_out<bool> start_o;
    sc_out<int> input_cnt_o;        // from TU+`
    sc_out<int> hidden_layer_cnt_o; // from TU+
    sc_out<int> output_cnt_o;       // from TU+
    sc_out<int> neurons_in_layer_o; // from TU+

    sc_out<bool> data_ready_o;
    sc_out<float> pe_weights_o[PE_THREADS][PE_ACCELERATED_THREADS][ACCELERATOR_THREADS];
    sc_out<float> pe_data_o[ACCELERATOR_THREADS];

    sc_in<bool> load_data_i;
    sc_in<bool> busy_i;
    sc_in<float> pe_value_i[PE_THREADS][PE_ACCELERATED_THREADS];

    float local_input_data[INPUT_DATA_SIZE];
    float local_next_input_data[INPUT_DATA_SIZE];

    int weights_ready_flag;

    float local_weight_memory[PE_THREADS][PE_ACCELERATED_THREADS][INPUT_DATA_SIZE];

    PE* processing_elements[PE_THREADS];
    CONTROL_SIGNAL_UNIT* control_signal_unit;

    SC_HAS_PROCESS(TENSOR_UNIT);

    TENSOR_UNIT(sc_module_name name, int input_cnt, int hidden_layers_cnt, int outputs_cnt, int neurons_in_layer_cnt);
    ~TENSOR_UNIT();

    void load_local_input_data();
    void load_local_weight_data();
    void transfer_locals_to_pe();
    void transfer_pe_to_next();
    void load_result();

    void init();
    void set_shared_memory(std::vector<float> & shared_memory_data,
                           std::vector<std::vector<std::vector<float>>> & shared_memory_weight,
                           std::vector<float> & shared_memory_result);
};

#endif