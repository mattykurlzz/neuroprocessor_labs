#include <systemc>
#include "accelerator.hpp"

// Число uint32 содержит число с фиксированной точкой с границами от -128 до 128
// Разметка:  | 0 | 1 | ... | 7 | 8 | ... | 31 |
//              ^sn ^whole part   ^fractional part
// Точность числа: 5.960464832810452e-8
int
main(int, char*[]) {
    return 0;
}
