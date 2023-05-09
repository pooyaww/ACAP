#include <adf.h>

void aie_read(input_stream_int32* in, output_stream_int32* out) {
    v4int32 a = readincr_v4(in);
    writeincr_v4(out, a);
}

void aie_write(input_stream_int32* in, output_stream_int32* out) {
    v4int32 a = readincr_v4(in);
    writeincr_v4(out, a);
}

