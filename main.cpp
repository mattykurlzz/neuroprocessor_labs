#include <iostream>
#include <systemc>

#include "top.hpp"
#include "types.hpp"
#include "vector"

int
sc_main(int, char*[]) {
    shared_memory_weight = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
    shared_memory_data = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    TENSOR_UNIT tensor_unit("tens_unit");

    // init_tensor_unit();
    tensor_unit.input_cnt_o.write(2);
    tensor_unit.hidden_layer_cnt_o.write(1);
    tensor_unit.output_cnt_o.write(1);
    tensor_unit.neurons_in_layer_o.write(2);
    // tensor_unit.start_o.write(1);
    sc_core::sc_start();

    while (tensor_unit.busy_i.read() != 0) {}
    // std::cout << shared_memory_result[0].value;
    return 0;
}
