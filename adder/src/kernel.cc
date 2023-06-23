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

void aie_adder(input_stream<int32_t>* in0, input_stream<int32_t>* in1, output_stream<int32_t>* out) {
    //v4int32 a = readincr_v<4>(in0);
    auto a = readincr_v4(in0);
    //v4int32 b = readincr_v4(in1);
    auto b = readincr_v4(in1);
    //v4int32 c = operator+(a, b);
    auto c = operator+(a, b);
    writeincr(out, c);
}
