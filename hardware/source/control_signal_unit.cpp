#include "control_signal_unit.hpp"
#include "defines.hpp"

#include <systemc>

CONTROL_SIGNAL_UNIT::CONTROL_SIGNAL_UNIT(sc_core::sc_module_name name)
    : sc_core::sc_module(name), state(IDLE), count_hidden_layers_calced(0), start_i("start_i") {
    SC_METHOD(switch_state);
    dont_initialize();
    sensitive << start_i << last_iter_o;

    SC_METHOD(trigger_load_data);
    dont_initialize();
    sensitive << start_i << last_iter_o;

    SC_THREAD(trigger_start_calc);
    dont_initialize();
    sensitive << data_ready_i;

    count_hidden_layers_calced = -1;
}

void
CONTROL_SIGNAL_UNIT::switch_state() {
    while (true) {
        switch (state) {
            case IDLE:
                state = start_i.read() == 1 ? CALCULATE_HIDDEN_LAYER : IDLE;
                finish_o.write(0);
                break;
            case CALCULATE_HIDDEN_LAYER:
                count_hidden_layers_calced++;
                state = count_hidden_layers_calced == hidden_layer_cnt_i.read() ? CALCULATE_OUTPUT_LAYER
                                                                                : CALCULATE_HIDDEN_LAYER;
                break;
            case CALCULATE_OUTPUT_LAYER:
                state = IDLE;
                count_hidden_layers_calced = -1;
                finish_o.write(1);
                break;

            default: break;
        }
        wait();
    }
}

void
CONTROL_SIGNAL_UNIT::trigger_load_data() {
    while (true) {
        load_data_o.write(0);
        wait(0);
        load_data_o.write((load_data_o.event() && state != CALCULATE_OUTPUT_LAYER) ? 1 : 0);
        wait();
    }
}

void
CONTROL_SIGNAL_UNIT::trigger_start_calc() {
    // метод запускает рассчет столько раз, сколько (входных сигналов / потоков акселератора) есть //todo
    int count_num;
    while (true) {
        if (state == CALCULATE_HIDDEN_LAYER || state == CALCULATE_OUTPUT_LAYER) {
            if (count_hidden_layers_calced == 0) {
                count_num = std::ceil(input_cnt_i.read() / ACCELERATOR_THREADS);
            } else {
                count_num = std::ceil(hidden_layer_cnt_i.read() / ACCELERATOR_THREADS);
            }
        }
        for (int i = 0; i < count_num; i++) {
            if (i == count_num - 1) {
                last_iter_o.write(1);
            }
            start_calc_o.write(1);
            wait(1, sc_core::SC_NS);
            start_calc_o.write(0);
        }
        wait();
    }
}