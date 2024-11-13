#ifndef _TOP_HPP
#define _TOP_HPP

#include "tensor_unit.hpp"

#include <vector>

static std::vector<float> shared_memory_data;
static std::vector<float> shared_memory_weight;
static std::vector<float> shared_memory_result;

// static sc_core::sc_out<bool> start_o;           // from TU+
// static sc_core::sc_out<int> input_cnt_o;        // from TU+
// static sc_core::sc_out<int> hidden_layer_cnt_o; // from TU+
// static sc_core::sc_out<int> output_cnt_o;       // from TU+
// static sc_core::sc_out<int> neurons_in_layer_o; // from TU+
// static sc_core::sc_in<bool> busy_i;             // from TU+

void init_tensor_unit();

#endif
