#ifndef _DEFINES_H
#define _DEFINES_H

#define ACCELERATOR_THREADS    16 /* количество потоков внутри акселератора */
#define PE_ACCELERATED_THREADS 16 /* количество акселераторов в одном PE */
#define PE_THREADS             6  /* количество PE в одном акселераторе */

#define INPUT_DATA_SIZE        256 /* количество связей, приходящих в нейрон (максимум без свопа) */
#define LAYER_LINKS_SIZE       128 /* максимальное количество связей в слое, записываемое в памяти */

#endif /*_DEFINES_H*/