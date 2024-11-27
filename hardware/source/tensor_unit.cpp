#include "tensor_unit.hpp"
#include "accelerator.hpp"
#include "pe.hpp"
#include "top.hpp"

#include <systemc>

TENSOR_UNIT::TENSOR_UNIT(sc_module_name name, int input_cnt, int hidden_layers_cnt, int outputs_cnt,
                         int neurons_in_layer_cnt)
    : sc_core::sc_module(name), input_cnt(input_cnt), hidden_layer_cnt(hidden_layers_cnt), output_cnt(outputs_cnt),
      neurons_in_layer(neurons_in_layer_cnt) {

    printf("TU constructor\n");

    control_signal_unit = new CONTROL_SIGNAL_UNIT("CSE");

    for (int i = 0; i < PE_THREADS; i++) {
        sc_core::sc_module_name name(std::string("PE" + std::to_string(i)).c_str());
        processing_elements[i] = new PE(name);
    }

    SC_THREAD(load_local_input_data);
    dont_initialize();
    sensitive << control_signal_unit->load_data_o.pos();

    SC_THREAD(load_local_weight_data); //
    dont_initialize();
    sensitive << control_signal_unit->load_data_o.pos();

    SC_THREAD(transfer_locals_to_pe); //
    dont_initialize();
    sensitive << int_data_ready << processing_elements[0]->accelerator_threads[0]->intermid_ready_o.pos();

    SC_THREAD(load_result); //
    dont_initialize();
    sensitive << control_signal_unit->finish_o.pos();

    SC_THREAD(transfer_pe_to_next); //
    dont_initialize();
    sensitive << processing_elements[0]->calc_ready_o.pos();

    SC_THREAD(init);

    weights_ready_flag = 0;
}

TENSOR_UNIT::~TENSOR_UNIT() {
    printf("TU destructor\n");
    delete control_signal_unit;
    for (int i = 0; i < PE_THREADS; i++) {
        delete processing_elements[i];
    }
}

void
TENSOR_UNIT::load_local_input_data() {
    while (true) {
        printf("TU load_local_input_data\n");

        if (control_signal_unit->count_hidden_layers_calced == 0) {
            for (int i = 0; i < INPUT_DATA_SIZE; i++) {
                try {
                    local_input_data[i] = this->shared_memory_data.at(i);
                } catch (const std::out_of_range) {
                    local_input_data[i] = 0;
                }
            }
        } else {
            for (int i = 0; i < INPUT_DATA_SIZE; i++) {
                local_input_data[i] = local_next_input_data[i];
            }
        }

        if (weights_ready_flag == 1) {
            weights_ready_flag = 0;
            int_data_ready.notify();
        }

        wait();
    }
}

void
TENSOR_UNIT::load_local_weight_data() {
    int int_counter = 0;
    while (true) {
        printf("TU load_local_weight_data\n");
        if (control_signal_unit->count_hidden_layers_calced
            <= control_signal_unit->hidden_layer_cnt_i.read()) { // PE = нейроны, ACC = 1 нейрон
            for (int i = 0; i < PE_THREADS; i++) {
                for (int j = 0; j < PE_ACCELERATED_THREADS; j++) {
                    for (int k = 0; k < INPUT_DATA_SIZE; k++) {
                        try {
                            local_weight_memory[i][j][k] = this->shared_memory_weight
                                                               .at(control_signal_unit->count_hidden_layers_calced)
                                                               .at(j + i * PE_THREADS)
                                                               .at(k);
                        } catch (const std::out_of_range) {
                            local_weight_memory[i][j][k] = 0;
                        }
                    }
                }
            }
            weights_ready_flag = 1;
        }
        wait();
    }
}

void
TENSOR_UNIT::transfer_locals_to_pe() {
    int iters_counter = 0;

    while (true) {
        printf("TU transfer_locals_to_pe\n");

        for (int i = 0; i < PE_THREADS; i++) {
            for (int j = 0; j < PE_ACCELERATED_THREADS; j++) {
                for (int k = 0; k < ACCELERATOR_THREADS; k++) {
                    pe_weights_o[i][j][k].write(local_weight_memory[i][j][k + iters_counter * ACCELERATOR_THREADS]);
                }
            }
        }
        for (int k = 0; k < ACCELERATOR_THREADS; k++) {
            pe_data_o[k].write(local_input_data[k + iters_counter * ACCELERATOR_THREADS]);
        }

        iters_counter++;
        if (control_signal_unit->count_num == iters_counter) {
            iters_counter = 0;
        }

        data_ready_o.write(0);
        wait(1, SC_NS);
        data_ready_o.write(1);
        wait();
    }
}

void
TENSOR_UNIT::load_result() {
    while (true) {
        printf("TU load_result\n");
        for (int i = 0; i < control_signal_unit->output_cnt_i; i++) {
            float tmp = local_next_input_data[i];
            this->shared_memory_result.push_back(tmp);
            printf("result %i = %f\n", i, tmp);
        }
        wait();
    }
    return;
}

void
TENSOR_UNIT::transfer_pe_to_next() {
    while (true) {
        printf("transfer pe to next\n");
        for (int i = 0; i < PE_THREADS; i++) {
            for (int j = 0; j < PE_ACCELERATED_THREADS; j++) {
                local_next_input_data[i * PE_THREADS + j] =
                    (processing_elements[i]->accelerator_threads[j]->value_o.read());
            }
        }
        wait();
    }
}

void
TENSOR_UNIT::init() {
    printf("tensor unit init\n");
    input_cnt_o.write(input_cnt);
    output_cnt_o.write(output_cnt);
    hidden_layer_cnt_o.write(hidden_layer_cnt);
    neurons_in_layer_o.write(neurons_in_layer);
    start_o.write(1);
    wait(1, SC_NS);
    start_o.write(0);
}

void
TENSOR_UNIT::set_shared_memory(std::vector<float>& shared_memory_data,
                               std::vector<std::vector<std::vector<float>>>& shared_memory_weight,
                               std::vector<float>& shared_memory_result) {
    this->shared_memory_data = shared_memory_data;
    this->shared_memory_weight = shared_memory_weight;
    this->shared_memory_result = shared_memory_result;
}