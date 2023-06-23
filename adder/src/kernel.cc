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

//TODO: move lane num and type tnd othe configs to a common config file
#include <adf.h>
#include "aie_api/aie.hpp"
#include <aie_api/aie_adf.hpp>
#include <aie_api/utils.hpp>
#define LANE_SIZE 4
#define WINDOW_SIZE 2048 //sample size, one iteration of graph is enough
using TYPE = int32_t;

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

