#include "control_signal_unit.hpp"
#include "defines.hpp"

#include <systemc>

CONTROL_SIGNAL_UNIT::CONTROL_SIGNAL_UNIT(sc_core::sc_module_name name)
    : sc_core::sc_module(name), state(IDLE), count_hidden_layers_calced(0), start_i("start_i") {

    printf("CSU construct\n");

    count_hidden_layers_calced = 0;
    SC_THREAD(switch_state);
    dont_initialize();
    sensitive << start_i.pos() << results_ready_i.pos();

    SC_THREAD(trigger_load_data);
    dont_initialize();
    sensitive << start_i.pos() << results_ready_i.pos();

    SC_THREAD(trigger_start_calc);
    dont_initialize();
    sensitive << data_ready_i.pos(); //<< intermid_ready_i.pos();
}

void
CONTROL_SIGNAL_UNIT::switch_state() {
    while (true) {
        printf("CSU switch state\n");
        switch (state) {
            case IDLE: state = start_i.read() == 1 ? CALCULATE_HIDDEN_LAYER : IDLE; break;
            case CALCULATE_HIDDEN_LAYER:
                count_hidden_layers_calced++;
                state = count_hidden_layers_calced == hidden_layer_cnt_i.read() ? CALCULATE_OUTPUT_LAYER
                                                                                : CALCULATE_HIDDEN_LAYER;
                break;
            case CALCULATE_OUTPUT_LAYER:
                state = IDLE;
                count_hidden_layers_calced = 0;
                wait(1, sc_core::SC_NS);
                finish_o.write(1);
                wait(1, sc_core::SC_NS);
                finish_o.write(0);
                break;

            default: break;
        }

        if (state == CALCULATE_HIDDEN_LAYER || state == CALCULATE_OUTPUT_LAYER) {
            if (count_hidden_layers_calced == 0) {
                count_num = std::ceil(float(input_cnt_i.read()) / ACCELERATOR_THREADS);
            } else {
                count_num = std::ceil(float(neurons_in_layer_i.read()) / ACCELERATOR_THREADS);
            }
        }
        wait();
    }
}

void
CONTROL_SIGNAL_UNIT::trigger_load_data() {
    while (true) {
        printf("CSU trigger load data\n");

        load_data_o.write(0);
        wait(1, sc_core::SC_NS);
        load_data_o.write((state != IDLE) ? 1 : 0);
        std::cout << "load_data_o = " << ((state != IDLE) ? 1 : 0) << "\n";
        wait();
    }
}

void
CONTROL_SIGNAL_UNIT::trigger_start_calc() {
    // метод запускает рассчет столько раз, сколько (входных сигналов / потоков акселератора) есть
    while (true) {
        printf("CSU trigger_start_calc\n");
        for (int i = 0; i < count_num; i++) {
            wait(1, sc_core::SC_NS);
            if (i == count_num - 1) {
                last_iter_o.write(1);
            }
            start_calc_o.write(1);
            wait(1, sc_core::SC_NS);
            start_calc_o.write(0);
            last_iter_o.write(0);
            wait();
        }
    }
}