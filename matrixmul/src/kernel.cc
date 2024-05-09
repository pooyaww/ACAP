
#include <adf.h>
#include <aie_api/aie_adf.hpp>
#include <aie_api/operators.hpp>
#include <aie_api/utils.hpp>
#include "config.h"
using namespace aie::operators;

void aie_multiplier_stream(input_stream<TYPE>* in0, input_stream<TYPE>* in1, output_stream<TYPE>* out) {
    //v4int32 a = readincr_v<4>(in0);
    //auto a = readincr_v4(in0);
    auto a = readincr_v<LANE_SIZE>(in0);
    //v4int32 b = readincr_v4(in1);
    //auto b = readincr_v4(in1);
    auto b = readincr_v<LANE_SIZE>(in1);
    //v4int32 c = operator+(a, b);
    //auto c = operator+(a, b);
    auto c = aie::mul(a, b); // does not work on stream?
    auto c_vect = srs(c, 0);
    //auto c_vect = c.to_vector<TYPE>(0);
    // write on stream interface can have _v4 but not _v<4> and probably has no effect on performance 
    writeincr(out, c_vect);
}


template <unsigned M, unsigned K, unsigned N, typename T>
[[gnu::always_inline]]
static void mmul_blocked(
		unsigned rowA, unsigned colA, unsigned colB, const T *__restrict A, const T *__restrict B, T *__restrict C)
{
	using MMUL = aie::mmul<M, K, N, T, T>;

	for (unsigned z = 0; z < rowA; z++) {
		for (unsigned j = 0; j < colB; j++) {
			unsigned i = 0;
			const T *a_ptr = A + (z * colA + i) * MMUL::size_A;
			const T *b_ptr = B + (i * colB + j) * MMUL::size_B;
			T       *c_ptr = C + (z * colB + j) * MMUL::size_C;

			auto block_a = aie::load_v<MMUL::size_A>(a_ptr);
			auto block_b = aie::load_v<MMUL::size_B>(b_ptr);

			MMUL block_c;
			auto b_c = aie::mmul<M, K, N, T, T>::mul(block_a, block_b);
			//block_c.mul(block_a, block_b);

			for (i = 1; i < colA; ++i) {
				a_ptr = A + (z * colA + i) * MMUL::size_A;
				b_ptr = B + (i * colB + j) * MMUL::size_B;

				block_a = aie::load_v<MMUL::size_A>(a_ptr);
				block_b = aie::load_v<MMUL::size_B>(b_ptr);

				block_c.mac(block_a, block_b);
			}

			aie::store_v(c_ptr, block_c.template to_vector<T>());
		}
	}
}


//alignas(32) TYPE coeff_int[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
TYPE coeff_int[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
void aie_multiplier_window(input_window<TYPE>* in0, input_window<TYPE>* in1, output_window<TYPE>* out) {
    for (size_t i = 0; i < WINDOW_SIZE/LANE_SIZE; ++i) {
        auto a = window_readincr_v<LANE_SIZE>(in0);
        auto b = window_readincr_v<LANE_SIZE>(in1);
	//auto temp = aie::load_v<LANE_SIZE>(coeff_int);
	auto temp = aie::load_v<LANE_SIZE>(coeff_int);
	//auto temp2 = aie::load_v<8>(coeff_int);
	//auto temp = aie::load_v((int32_t*)in0);
	//auto temp = aie::load_v((int32_t*)in0);
	//auto temp = aie::vector::insert(2, b);
	//auto temp = a.insert(0, b);
        auto res = aie::mul(a, b);
        //auto res = aie::mul(a, temp);
        auto res_vect = srs(res, 0);
	//aie::store_v(out, res_vect); // it seems load with LANE_SIZE does not exist
        window_writeincr(out, res_vect);
    }
}

//scalar free-running kernel
//only in stream method
void aie_multiplier_free_running_scalar(input_stream<TYPE>* in0, input_stream<TYPE>* in1, output_stream<TYPE>* out) {
    while(true){//for(;;) is acceptable for C++
        auto a = readincr(in0);
        auto b = readincr(in1);
        chess_separator_scheduler();//make sure stream is flushed
        //auto c = aie::add(a, b); // only works on vector types?
        //auto c = operator+(a, b);
        auto c = a * b;
        writeincr(out, c);
        chess_separator_scheduler();//make sure stream is flushed
    }
}


// from Vitis 2024.1
//void aie_multiplier_buffer_scalar(input_buffer<TYPE, adf::extents<BUFFER_SIZE>>& __restrict in0, input_buffer<TYPE, adf::extents<BUFFER_SIZE>>& __restrict in1, output_buffer<TYPE, adf::extents<BUFFER_SIZE>>& __restrict out){
//    auto inIter1 = aie::begin(in0);
//    auto inIter2 = aie::begin(in1);
//    auto outIter = aie::begin(out);
//    for(int i = 0; i < SAMPLE_SIZE ; ++i) {
//        int32 a = *inIter1++; //readincr
//        int32 b = *inIter2++;
//        int32 c = a * b;
//        *outIter++ = c; //writeincr
//    }
//}

//void aie_multiplier_buffer_vector(input_buffer<TYPE, adf::extents<BUFFER_SIZE>>& __restrict in0, input_buffer<TYPE, adf::extents<BUFFER_SIZE>>& __restrict in1, output_buffer<TYPE, adf::extents<BUFFER_SIZE>>& __restrict out){
//    auto inIter1 = aie::begin_vector<LANE_SIZE>(in0); //iterator for vector of LANE_SIZE elements
//    auto inIter2 = aie::begin_vector<LANE_SIZE>(in1);
//    auto outIter = aie::begin_vector<LANE_SIZE>(out);
//    for(int i = 0; i < BUFFER_SIZE/LANE_SIZE; ++i) chess_prepare_for_pipelining {
//        int32 a = *inIter1++; //readincr for vector of LANE_SIZE elements
//        int32 b = *inIter2++;
//        auto c = aie::mull(a, b);
//        auto c_vect = srs(c, 0);
//        *outIter++ = c_vect; //writeincr, check for c.to_vector<TYPE>(0)
//    }
//}


//void aie_multiplier_circular_buffer_vector(input_circular_buffer<TYPE, adf::extents<BUFFER_SIZE>>& in0, input_circular_buffer<TYPE, adf::extents<BUFFER_SIZE>>& in1, output_circular_buffer<TYPE, adf::extents<BUFFER_SIZE>>& out) {
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
//        auto c = aie::mul(a, b)
//        auto c_vect = srs(c, 0);
//        *pOut++ = c_vect;
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
