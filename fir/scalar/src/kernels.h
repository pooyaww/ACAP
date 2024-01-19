#ifndef KERNEL_H
#define KERNEL_H

#define FIR_TAPS    8
#define NUM_SAMPLES 256


// Kernel prototype
void fir_asym_8t_16int_scalar(input_window_int16 * in, output_window_int16 * out);

#endif // KERNEL_H
