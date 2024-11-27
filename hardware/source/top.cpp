#include "top.hpp"

using namespace sc_core;

void
init_tensor_unit() {
    // tensor_unit.control_signal_unit->start_i(start_o);
    // tensor_unit.control_signal_unit->input_cnt_i(input_cnt_o);
    // tensor_unit.control_signal_unit->hidden_layer_cnt_i(hidden_layer_cnt_o);
    // tensor_unit.control_signal_unit->output_cnt_i(output_cnt_o);
    // tensor_unit.control_signal_unit->neurons_in_layer_i(neurons_in_layer_o);
    // busy_i(tensor_unit.control_signal_unit->busy_o);
    // sc_core::sc_start();
}

std::vector<float> shared_memory_data;
std::vector<std::vector<std::vector<float>>> shared_memory_weight;
std::vector<float> shared_memory_result;

/* accelerator_output */
sc_signal<bool> acc_pe_data_ready_signal[PE_THREADS][PE_ACCELERATED_THREADS];     //AB
sc_signal<bool> acc_pe_intermid_ready_signal[PE_THREADS][PE_ACCELERATED_THREADS]; //AB
sc_signal<float> acc_pe_value_signal[PE_THREADS][PE_ACCELERATED_THREADS];         //A

/* pe_output */
sc_signal<bool> pe_tu_data_ready_signal[PE_THREADS]; //A

/* tensorUnit_outputs */
sc_signal<float> tu_pe_data_energy_signal[ACCELERATOR_THREADS];                                     // B
sc_signal<float> tu_pe_data_weight_signal[PE_THREADS][PE_ACCELERATED_THREADS][ACCELERATOR_THREADS]; // B
sc_signal<float> pe_tu_result_signal[PE_THREADS][PE_ACCELERATED_THREADS];

/* csu_inputs */
sc_signal<bool> csu_data_ready_signal; // b
/* csu_outputs */
sc_signal<bool> busy_signal;       // A
sc_signal<bool> start_calc_signal; // AB
sc_signal<bool> load_data_signal;  // A
sc_signal<bool> last_iter_signal;  // AB
sc_signal<bool> finish_signal;     // A

/* user inputs*/
sc_signal<bool> tu_csu_start_signal;           // внешний старт, который триггерится пользователем
sc_signal<int> tu_csu_input_cnt_signal;        // from CU to SCU
sc_signal<int> tu_csu_hidden_layer_cnt_signal; // from CU to SCU
sc_signal<int> tu_csu_output_cnt_signal;       // from CU to SCU
sc_signal<int> tu_csu_neurons_in_layer_signal; // from CU to SCU

void
bind_tu(TENSOR_UNIT& tu) {
    for (int i = 0; i < PE_THREADS; i++) {
        for (int j = 0; j < PE_ACCELERATED_THREADS; j++) {
            /* accelerator init */
            tu.processing_elements[i]->accelerator_threads[j]->start_i(start_calc_signal);
            tu.processing_elements[i]->accelerator_threads[j]->last_iter_i(last_iter_signal);
            tu.processing_elements[i]->accelerator_threads[j]->data_ready_o(acc_pe_data_ready_signal[i][j]);
            tu.processing_elements[i]->accelerator_threads[j]->intermid_ready_o(acc_pe_intermid_ready_signal[i][j]);
            tu.processing_elements[i]->accelerator_threads[j]->value_o(pe_tu_result_signal[i][j]);

            for (int k = 0; k < ACCELERATOR_THREADS; k++) {
                tu.processing_elements[i]->accelerator_threads[j]->data_i[k](tu_pe_data_energy_signal[k]);
                tu.processing_elements[i]->accelerator_threads[j]->weight_i[k](tu_pe_data_weight_signal[i][j][k]);
            }
        }
        /* pe init */
        tu.processing_elements[i]->calc_ready_o(pe_tu_data_ready_signal[i]);
    }

    /* CSU init */
    tu.control_signal_unit->start_i(tu_csu_start_signal);
    tu.control_signal_unit->input_cnt_i(tu_csu_input_cnt_signal);
    tu.control_signal_unit->hidden_layer_cnt_i(tu_csu_hidden_layer_cnt_signal);
    tu.control_signal_unit->output_cnt_i(tu_csu_output_cnt_signal);
    tu.control_signal_unit->neurons_in_layer_i(tu_csu_neurons_in_layer_signal);
    tu.control_signal_unit->data_ready_i(csu_data_ready_signal);
    tu.control_signal_unit->intermid_ready_i(acc_pe_intermid_ready_signal[0][0]);
    tu.control_signal_unit->results_ready_i(pe_tu_data_ready_signal[0]);

    tu.control_signal_unit->busy_o(busy_signal);
    tu.control_signal_unit->start_calc_o(start_calc_signal);
    tu.control_signal_unit->load_data_o(load_data_signal);
    tu.control_signal_unit->last_iter_o(last_iter_signal);
    tu.control_signal_unit->finish_o(finish_signal);

    /* tensor unit init */
    tu.start_o(tu_csu_start_signal);
    tu.data_ready_o(csu_data_ready_signal);
    tu.load_data_i(load_data_signal);
    tu.busy_i(busy_signal);

    tu.input_cnt_o(tu_csu_input_cnt_signal);
    tu.hidden_layer_cnt_o(tu_csu_hidden_layer_cnt_signal);
    tu.output_cnt_o(tu_csu_output_cnt_signal);
    tu.neurons_in_layer_o(tu_csu_neurons_in_layer_signal);

    for (int i = 0; i < PE_THREADS; i++) {
        for (int j = 0; j < PE_ACCELERATED_THREADS; j++) {
            for (int k = 0; k < ACCELERATOR_THREADS; k++) {
                tu.pe_weights_o[i][j][k](tu_pe_data_weight_signal[i][j][k]);
            }
            tu.pe_value_i[i][j](pe_tu_result_signal[i][j]);
        }
    }
    for (int k = 0; k < ACCELERATOR_THREADS; k++) {
        tu.pe_data_o[k](tu_pe_data_energy_signal[k]);
    }
}
