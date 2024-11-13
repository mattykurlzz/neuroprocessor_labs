#ifndef _CONTROL_SIGNAL_UNIT_HPP
#define _CONTROL_SIGNAL_UNIT_HPP

#include <systemc>

enum CONTROL_SIGNAL_STATE { IDLE, CALCULATE_HIDDEN_LAYER, CALCULATE_OUTPUT_LAYER };

SC_MODULE(CONTROL_SIGNAL_UNIT) {
    /*control unit*/
    sc_core::sc_in<bool> start_i;           // from TU+
    sc_core::sc_in<int> input_cnt_i;        // from TU+
    sc_core::sc_in<int> hidden_layer_cnt_i; // from TU+
    sc_core::sc_in<int> output_cnt_i;       // from TU+
    sc_core::sc_in<int> neurons_in_layer_i; // from TU+

    /*tensor_unit*/
    sc_core::sc_in<bool> data_ready_i; // from TU+

    sc_core::sc_out<bool> busy_o;       // to TU+ to CU
    sc_core::sc_out<bool> start_calc_o; // to ACCELERATOR+
    sc_core::sc_out<bool> load_data_o;  // to TU
    sc_core::sc_out<bool> last_iter_o;  // to ACCELERATOR+
    sc_core::sc_out<bool> finish_o;     // to TU

    enum CONTROL_SIGNAL_STATE state;
    int count_hidden_layers_calced;

    SC_HAS_PROCESS(CONTROL_SIGNAL_UNIT);

    CONTROL_SIGNAL_UNIT(sc_core::sc_module_name name);

    void switch_state();
    void trigger_load_data();
    void trigger_start_calc();
};

#endif
