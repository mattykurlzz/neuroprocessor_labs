#ifndef _TOP_HPP
#define _TOP_HPP

#include "tensor_unit.hpp"

#include <vector>

extern std::vector<float> shared_memory_data;
extern std::vector<std::vector<std::vector<float>>> shared_memory_weight;
extern std::vector<float> shared_memory_result;

void init_tensor_unit();

void bind_tu(TENSOR_UNIT&);

#endif
