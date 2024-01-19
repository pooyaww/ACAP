#include "adf.h"
#include "../../src/kernel.cc"
void b0_kernel_wrapper(x86sim::stream_internal * arg0, x86sim::stream_internal * arg1, x86sim::stream_internal * arg2)
{
  auto _arg0 = (input_stream_int32 *)(arg0);
  auto _arg1 = (input_stream_int32 *)(arg1);
  auto _arg2 = (output_stream_int32 *)(arg2);
  return aie_multiplier_stream(_arg0, _arg1, _arg2);
}
