/**
* Copyright (C) 2019-2021 Xilinx, Inc
*
* Licensed under the Apache License, Version 2.0 (the "License"). You may
* not use this file except in compliance with the License. A copy of the
* License is located at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
* License for the specific language governing permissions and limitations
* under the License.
*/

#include <adf.h>
#include "aie_api/aie.hpp"
#include <aie_api/aie_adf.hpp>
#include <aie_api/utils.hpp>
#include "config.h"

void aie_adder_stream(input_stream<TYPE>* in0, input_stream<TYPE>* in1, output_stream<TYPE>* out) {
    //v4int32 a = readincr_v<4>(in0);
    //auto a = readincr_v4(in0);
    auto a = readincr_v<LANE_SIZE>(in0);
    //v4int32 b = readincr_v4(in1);
    //auto b = readincr_v4(in1);
    auto b = readincr_v<LANE_SIZE>(in1);
    //v4int32 c = operator+(a, b);
    auto c = operator+(a, b);
    //auto c = aie::add(a, b); // does not work on stream?
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
//        int32 c = aie::add(a + b);
//        *outIter++ = c; //writeincr, check for c.to_vector<int32>(0)
//    }
//}


// Free-running kernels
//only in stream method
//void free_running_aie(input_stream<TYPE>* in, output_stream<TYPE>* out) {
//    while(true){//for(;;) is acceptable for C++
//        int32 tmp = readincr(in);
//        chess_separator_scheduler();//make sure stream is flushed
//        writeincr(out, tmp+1);
//        chess_separator_scheduler();//make sure stream is flushed
//    }
//}
