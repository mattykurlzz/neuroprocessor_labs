#include "tensor_unit.hpp"
#include "accelerator.hpp"
#include "pe.hpp"
#include "top.hpp"

#include <systemc>

TENSOR_UNIT::TENSOR_UNIT(sc_core::sc_module_name name)
    : sc_core::sc_module(name), local_input_data(INPUT_DATA_SIZE * ACCELERATOR_THREADS),
      local_next_input_data(INPUT_DATA_SIZE * ACCELERATOR_THREADS), input_cnt_o("input_cnt_o") {
    control_signal_unit = new CONTROL_SIGNAL_UNIT("CSE");

    for (int i = 0; i < PE_THREADS; i++) {
        sc_core::sc_module_name name(std::string("PE" + std::to_string(i)).c_str());
        processing_elements[i] = new PE(name);

        for (int j = 0; j < PE_ACCELERATED_THREADS; j++) {
            for (int k = 0; k < ACCELERATOR_THREADS; k++) {
                processing_elements[i]->weight_i[j][k](pe_inputs_o[i][j][k]);
            }
            pe_value_i[i][j](processing_elements[i]->value_o[j]);
        }
        processing_elements[i]->start_i(control_signal_unit->start_calc_o);
        processing_elements[i]->last_iter_i(control_signal_unit->last_iter_o);
    }

    load_data_i(control_signal_unit->load_data_o);
    control_signal_unit->data_ready_i(data_ready_o);

    SC_THREAD(load_local_input_data);
    dont_initialize();
    sensitive << processing_elements[0]->result_ready_o << control_signal_unit->load_data_o;

    SC_THREAD(load_local_weight_data); //
    dont_initialize();
    sensitive << processing_elements[0]->result_ready_o << control_signal_unit->load_data_o;

    SC_THREAD(load_result); //
    dont_initialize();
    sensitive << processing_elements[0]->result_ready_o;

    SC_THREAD(transfer_pe_to_next); //
    dont_initialize();
    sensitive << processing_elements[0]->result_ready_o;

    SC_THREAD(init);

    weights_ready_flag = 0;

    control_signal_unit->start_i(start_o);
    control_signal_unit->input_cnt_i(input_cnt_o);
    control_signal_unit->hidden_layer_cnt_i(hidden_layer_cnt_o);
    control_signal_unit->output_cnt_i(output_cnt_o);
    control_signal_unit->neurons_in_layer_i(neurons_in_layer_o);
    busy_i(control_signal_unit->busy_o);
}

TENSOR_UNIT::~TENSOR_UNIT() {
    delete control_signal_unit;
    for (int i = 0; i < PE_THREADS; i++) {
        delete processing_elements[i];
    }
}

void
TENSOR_UNIT::load_local_input_data() {
    while (true) {
        if (control_signal_unit->start_i.read() == 1) {
            for (int i = 0; i < PE_ACCELERATED_THREADS * ACCELERATOR_THREADS; i++) {
                local_input_data.write(shared_memory_data[i]);
            }
        } else {
            for (int i = 0; i < PE_ACCELERATED_THREADS * ACCELERATOR_THREADS; i++) {
                local_input_data.write(local_next_input_data.read());
            }
        }

        data_ready_o.write(0);
        wait(0);
        if (weights_ready_flag == 1) {
            weights_ready_flag = 0;
            data_ready_o.write(1);
        }

        wait();
    }
}

void
TENSOR_UNIT::load_local_weight_data() {
    int int_counter = 0;
    while (true) {
        if (control_signal_unit->count_hidden_layers_calced == 0) {
            for (int i = 0;
                 i < control_signal_unit->input_cnt_i.read() * control_signal_unit->neurons_in_layer_i.read(); i++) {
                shared_memory_weight[int_counter];
                int_counter++;
            }
        } else if (control_signal_unit->count_hidden_layers_calced < control_signal_unit->hidden_layer_cnt_i.read()) {
            for (int i = 0;
                 i < control_signal_unit->neurons_in_layer_i.read() * control_signal_unit->neurons_in_layer_i.read();
                 i++) {
                shared_memory_weight[int_counter];
                int_counter++;
            }
        } else {
            for (int i = 0;
                 i < control_signal_unit->neurons_in_layer_i.read() * control_signal_unit->output_cnt_i.read(); i++) {
                shared_memory_weight[int_counter];
                int_counter++;
            }
            int_counter = 0;
        }
        weights_ready_flag = 1;
        wait();
    }
}

void
TENSOR_UNIT::transfer_local_weights_to_pe() {
    int iters_counter = -1;

    while (true) {
        iters_counter++;

        for (int i = 0; i < PE_THREADS; i++) {
            for (int j = 0; j < PE_ACCELERATED_THREADS; j++) {
                for (int k = 0; k < ACCELERATOR_THREADS; k++) {
                    pe_inputs_o[i][j][k].write(local_weight_memory[i][j][k + iters_counter * ACCELERATOR_THREADS]);
                }
            }
            for (int j = 0; j < ACCELERATOR_THREADS; j++) {
                pe_data_o[i][j].write(local_input_data.read());
            }
        }

        if (control_signal_unit->last_iter_o.read() == 1) {
            iters_counter = -1;
        }
        wait();
    }
}

void
TENSOR_UNIT::load_result() {
    while (true) {
        for (int i = 0; i < control_signal_unit->output_cnt_i * ACCELERATOR_THREADS; i++) {
            shared_memory_result.push_back(local_next_input_data.read());
        }
    }
}

void
TENSOR_UNIT::transfer_pe_to_next() {
    while (true) {
        for (int i = 0; i < PE_THREADS; i++) {
            for (int j = 0; j < PE_ACCELERATED_THREADS; j++) {
                local_next_input_data.write(processing_elements[i]->value_o[j]);
            }
        }
        wait();
    }
}

void
TENSOR_UNIT::init() {
    while (true) {
        start_o.write(1);
        wait();
        start_o.write(0);
        wait();
    }
}