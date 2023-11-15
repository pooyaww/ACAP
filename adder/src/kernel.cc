
#include <adf.h>
#include <aie_api/aie_adf.hpp>
#include <aie_api/operators.hpp>
#include <aie_api/utils.hpp>
#include "config.h"
using namespace aie::operators;

void aie_adder_stream(input_stream<TYPE>* in0, input_stream<TYPE>* in1, output_stream<TYPE>* out) {
    //v4int32 a = readincr_v<4>(in0);
    //auto a = readincr_v4(in0);
    auto a = readincr_v<LANE_SIZE>(in0);
    //v4int32 b = readincr_v4(in1);
    //auto b = readincr_v4(in1);
    auto b = readincr_v<LANE_SIZE>(in1);
    //v4int32 c = operator+(a, b);
    //auto c = operator+(a, b);
    auto c = aie::add(a, b); // does not work on stream?
    //auto c = a + b;
    //auto c = 1 + b;
    writeincr(out, c);
}

void aie_adder_window(input_window<TYPE>* in0, input_window<TYPE>* in1, output_window<TYPE>* out) {
    for (size_t i = 0; i < WINDOW_SIZE/LANE_SIZE; ++i) {
        auto a = window_readincr_v<LANE_SIZE>(in0);
        auto b = window_readincr_v<LANE_SIZE>(in1);
        auto res = aie::add(a, b);
        window_writeincr(out, res);
    }
}

//scalar free-running kernel
//only in stream method
void aie_adder_free_running_scalar(input_stream<TYPE>* in0, input_stream<TYPE>* in1, output_stream<TYPE>* out) {
    while(true){//for(;;) is acceptable for C++
        auto a = readincr(in0);
        auto b = readincr(in1);
        chess_separator_scheduler();//make sure stream is flushed
        //auto c = aie::add(a, b); // only works on vector types?
        //auto c = operator+(a, b);
        auto c = a + b;
        writeincr(out, c);
        chess_separator_scheduler();//make sure stream is flushed
    }
}


// from Vitis 2024.1
//void aie_adder_buffer_scalar(input_buffer<TYPE, adf::extents<BUFFER_SIZE>>& __restrict in0, input_buffer<TYPE, adf::extents<BUFFER_SIZE>>& __restrict in1, output_buffer<TYPE, adf::extents<BUFFER_SIZE>>& __restrict out){
//    auto inIter1 = aie::begin(in0);
//    auto inIter2 = aie::begin(in1);
//    auto outIter = aie::begin(out);
//    for(int i = 0; i < SAMPLE_SIZE ; ++i) {
//        int32 a = *inIter1++; //readincr
//        int32 b = *inIter2++;
//        int32 c = a + b;
//        *outIter++ = c; //writeincr
//    }
//}

//void aie_adder_buffer_vector(input_buffer<TYPE, adf::extents<BUFFER_SIZE>>& __restrict in0, input_buffer<TYPE, adf::extents<BUFFER_SIZE>>& __restrict in1, output_buffer<TYPE, adf::extents<BUFFER_SIZE>>& __restrict out){
//    auto inIter1 = aie::begin_vector<LANE_SIZE>(in0); //iterator for vector of LANE_SIZE elements
//    auto inIter2 = aie::begin_vector<LANE_SIZE>(in1);
//    auto outIter = aie::begin_vector<LANE_SIZE>(out);
//    for(int i = 0; i < BUFFER_SIZE/LANE_SIZE; ++i) chess_prepare_for_pipelining {
//        int32 a = *inIter1++; //readincr for vector of LANE_SIZE elements
//        int32 b = *inIter2++;
//        int32 c = aie::add(a, b);
//        *outIter++ = c; //writeincr, check for c.to_vector<TYPE>(0)
//    }
//}


//void aie_adder_circular_buffer_vector(input_circular_buffer<TYPE, adf::extents<BUFFER_SIZE>>& in0, input_circular_buffer<TYPE, adf::extents<BUFFER_SIZE>>& in1, output_circular_buffer<TYPE, adf::extents<BUFFER_SIZE>>& out) {
//    auto pIn0  = aie::begin_vector_random_circular<VECTOR_SIZE>(in0);
//    auto pIn1  = aie::begin_vector_random_circular<VECTOR_SIZE>(in1);
//    auto pOut  = aie::begin_vector_random_circular<VECTOR_SIZE>(out);
//
//    // Position the pointer at the end of the buffer
//    pIn0 += BUFFER_SIZE/VECTOR_SIZE;
//    pIn1 += BUFFER_SIZE/VECTOR_SIZE;
//
//    for (size_t i = 0; i < (BUFFER_SIZE/VECTOR_SIZE); ++i) {
//        auto a = *pIn0++;
//        auto b = *pIn1++;
//        auto c = aie::add(a, b)
//        *pOut++ = c;
//    }
//}
//



//void aie_copy_circular_buffer_vector(input_circular_buffer<cint16, adf::extents<BUFFER_SIZE>> & in, output_circular_buffer<cint16, adf::extents<BUFFER_SIZE>> & out) {
//    auto pIn  = aie::begin_vector_random_circular<VECTOR_SIZE>(in);
//    auto pOut = aie::begin_vector_random_circular<VECTOR_SIZE>(out);
//
//    // Position the pointer at the end of the buffer
//    pIn += BUFFER_SIZE/VECTOR_SIZE;
//    // Copies the input buffer onto the output buffer
//    for (unsigned i=0; i<(BUFFER_SIZE/VECTOR_SIZE); i++) {
//    *pOut++ = *pIn++;
//    }
//}
//

////Circular Input Buffer
//void k2(input_circular_buffer<int32, adf::extents<INPUT_SAMPLE_SIZE>, adf::margin<MARGIN_SIZE>> & in0, output_stream<int32> *out0) {
//    auto in0Iter = aie::begin_circular(in0);
//    for (int ind = 0; ind < (INPUT_SAMPLE_SIZE + MARGIN_SIZE); ++ind) {
//        writeincr(out0, *in0Iter++);
//    }
//}
//

////Circular Output Buffer
//void k1(input_stream<int32> *in0, output_circular_buffer<int32, adf::extents<OUTPUT_SAMPLE_SIZE>> & out0) {
//    auto out0Iter = aie::begin_circular(out0);
//    for (int ind = 0; ind < OUTPUT_SAMPLE_SIZE; ++ind) {
//        *out0Iter++ = readincr(in0);
//    }
//}

////Asynchronous Circular Buffer
//void k3(input_async_circular_buffer<int32, adf::extents<adf::inherited_extent>, adf::margin<MARGIN_SIZE>> &in0, output_stream<int32> *out0) {
//    in0.acquire();
//    auto in0Iter = aie::begin_circular(in0);
//    for (int ind = 0; ind < INPUT_SAMPLE_SIZE + MARGIN_SIZE; ++ind) {
//        writeincr(out0, *in0Iter++);
//    }
//
//    in0.release();
//}
