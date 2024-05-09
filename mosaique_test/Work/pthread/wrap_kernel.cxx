#include "adf.h"
#include "../../src/kernel.cxx"
void b1_kernel_wrapper(x86sim::stream_internal * arg0, x86sim::stream_internal * arg1)
{
  auto _arg0 = (input_stream_int32 *)(arg0);
  auto _arg1 = (output_stream_int32 *)(arg1);
  return _3309(_arg0, _arg1);
}
