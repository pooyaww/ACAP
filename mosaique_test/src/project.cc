
#include <adf.h>
#include <aie_api/aie_adf.hpp>
#include <aie_api/utils.hpp>
#include <iostream>
#include <string>
#include <fstream>
//#include <aie_api/operators.hpp>
//using namespace aie::operators;

typedef   int8_t  i8;
typedef  uint8_t  u8;
typedef  int16_t i16;
typedef uint16_t u16;
typedef  int32_t i32;
typedef uint32_t u32;
typedef  int64_t i64;
typedef uint64_t u64;
typedef    float f32;
typedef   double f64;


typedef i32 channel_i32_30;

void _3309(input_stream<channel_i32_30>*, output_stream<channel_i32_30>*);
//
//void _3309(input_stream<channel_i32_30>* _3309_5021, output_stream<channel_i32_30>* _3309_5022) {
//	aie::vector<i32, 4>   cont_5029;
//	aie::vector<i32, 4> p_cont_5029;
//	{
//		cont_5029 = adf::readincr_v<4>(_3309_5021);
//		p_cont_5029 = cont_5029;
//		goto cont_5027;
//	}
//cont_5027: {
//		   cont_5029 = p_cont_5029;
//		   adf::writeincr(_3309_5022, cont_5029);
//		   goto _3309_5043;
//	   }
//_3309_5043: {
//		    return;
//	    }
//}
//

class cgra_graph : public adf::graph {
	public:
		adf::input_plio _5040;
		adf::output_plio _5041;
	private:
		adf::kernel k_3309;

public:
		cgra_graph() {
			// Nodes
			//_5040 = adf::input_plio::create(adf::plio_32_bits, "in.txt");
			_5040 = adf::input_plio::create("dd", adf::plio_32_bits, "in.txt");
			//_5040 = adf::input_plio::create(adf::plio_32_bits);
			k_3309 = adf::kernel::create(_3309);
			//_5041 = adf::output_plio::create(adf::plio_32_bits, "out.txt");
			_5041 = adf::output_plio::create("ss", adf::plio_32_bits, "out.txt");
			//_5041 = adf::output_plio::create(adf::plio_32_bits);
			// Edges
			adf::connect<adf::stream> cgra_graph__3309(_5040.out[0], k_3309.in[0]);
			adf::connect<adf::stream> _3309_cgra_graph(k_3309.out[0], _5041.in[0]);
			// Constrains and Configurations
			adf::runtime<ratio>(k_3309) = 0.5;
			adf::location<adf::kernel>(k_3309) = adf::tile(1, 2);
			//adf::source(k_3309) = "channel_complete_sa.cgra";
			adf::source(k_3309) = "kernel.cxx";
		};
};

cgra_graph cgra_dataflow;

#if defined(__AIESIM__) || defined(__X86SIM__)

int main(void) {
	cgra_dataflow.init();
	const auto ITER_NUM = 5;
	cgra_dataflow.run(ITER_NUM);
	std::cout << "Graph executed " << ITER_NUM << " times" << std::endl;
	cgra_dataflow.end();
	std::cout << "Graph ended." << std::endl;
	return 0;
}
#endif
