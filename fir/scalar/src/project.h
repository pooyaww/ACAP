#include <adf.h>
#include "kernels.h"

using namespace adf;

class fir_asym_8t_16int_graph : public adf::graph {
    private:
        kernel fir_asym_8t_k_1;
    public:
        input_plio in;
        output_plio out;
  
        fir_asym_8t_16int_graph(){
	in  = input_plio::create(plio_32_bits, "../data/input.txt");
	out = output_plio::create(plio_32_bits, "../data/output.txt");

        fir_asym_8t_k_1 = kernel::create(fir_asym_8t_16int_scalar);
	
	connect< window<NUM_SAMPLES*2> > net0 (in.out[0], fir_asym_8t_k_1.in[0]);
        connect< window<NUM_SAMPLES*2> > net1 (fir_asym_8t_k_1.out[0], out.in[0]);

        source(fir_asym_8t_k_1) = "kernels/fir_asym_8t_16int_scalar.cpp";

        runtime<ratio>(fir_asym_8t_k_1) = 0.99;

      }
};
