#include <adf.h>
#include <iostream>
#include <string>
#include <fstream>
#include "config.h"

#define GOLD_FILE_ADR "./data/golden.txt"
#define IN0_FILE_ADR "data/input0.txt"
#define IN1_FILE_ADR "data/input1.txt"
#define OUT_FILE_ADR "data/output.txt"

#if defined (WINDOW)
  #define METHOD <window<WINDOW_SIZE * sizeof(TYPE)>>
#elif defined(STREAM)
  #define METHOD <stream>
#elif defined (FREE)
  #define METHOD <>
#else //BUFFER
  #define METHOD 
#endif

#if defined(__AIESIM__)
  #define SIM_OUT_FILE_ADR "./aiesimulator_output/data/output.txt"
#endif

#if defined(__X86SIM__)
    #define SIM_OUT_FILE_ADR "./x86simulator_output/data/output.txt"
#endif

using namespace adf;

void aie_adder_stream(input_stream<TYPE>*, input_stream<TYPE>*, output_stream<TYPE>*);
void aie_adder_window(input_window<TYPE>*, input_window<TYPE>*, output_window<TYPE>*);
void aie_adder_free_running_scalar(input_stream<TYPE>*, input_stream<TYPE>*, output_stream<TYPE>*);
//void aie_adder_buffer_scalar(input_buffer<TYPE>& __restrict in0, input_buffer<TYPE>& __restrict in1, output_buffer<TYPE>& __restrict out);

class simpleGraph : public graph {
   private:
    kernel adder;

   public:
    input_plio pl_in0, pl_in1;
    output_plio pl_out;

    simpleGraph() {


#if defined (WINDOW)
    	adder = kernel::create(aie_adder_window);
#elif defined (STREAM)
    	adder = kernel::create(aie_adder_stream);
#elif defined (FREE)
    	adder = kernel::create(aie_adder_free_running);
#elif defined (BUFFER_SCALAR) 
    	adder = kernel::create(aie_adder_buffer_scalar);
#elif defined (BUFFER_VECTORIZED)
    	adder = kernel::create(aie_adder_buffer_vector);
#endif
        source(adder) = "kernel.cc";

        pl_in0 = input_plio::create("DataIn0", adf::plio_32_bits, IN0_FILE_ADR);
        pl_in1 = input_plio::create("DataIn1", adf::plio_32_bits, IN1_FILE_ADR);
        pl_out = output_plio::create("DataOut", adf::plio_32_bits, OUT_FILE_ADR);

	connect METHOD net0(pl_in0.out[0], adder.in[0]);
        connect METHOD net1(pl_in1.out[0], adder.in[1]);
        connect METHOD net2(adder.out[0], pl_out.in[0]);


        runtime<ratio>(adder) = 0.1;
    };
};

simpleGraph mygraph;

#if defined(__AIESIM__) || defined(__X86SIM__)

int main(int argc, char** argv) {

    adf::return_code ret;

    std::ifstream input_file(IN1_FILE_ADR);
    std::string in_line;
    long int sample_size;
    if (input_file.is_open()) {
        for (sample_size = 0; std::getline(input_file, in_line); sample_size++);
    } else {
        std::cerr << "Can't open input file!";
    }
    input_file.close();

#if defined (WINDOW)
    long int iter_num = sample_size/WINDOW_SIZE;
#elif defined(BUFFER)
    long int iter_num = sample_size/BUFFER_SIZE;
#elif defined(FREE)
    long int iter_num = -1;
#else //STREAM
    long int iter_num = sample_size/LANE_SIZE;
#endif

    mygraph.init();
    std::cout << "Graph initialized" <<std::endl;
    ret = mygraph.run(iter_num);
    if (ret != adf::ok) {
        std::cout << "Run failed\n";
	return ret;
    }
    if (iter_num != -1)
        std::cout << "Graph executed " << iter_num << " times"<< std::endl;
    else
        std::cout << "Free running kernels. Graph executes for any number of samples"<< std::endl;
    ret = mygraph.end();
    if (ret != adf::ok) {
        std::cout << "End failed\n";
	return ret;
    }
    std::cout << "Graph end." <<std::endl;

    std::string out_line;
    std::string gold_line;
    std::ifstream output_file(SIM_OUT_FILE_ADR);
    std::ifstream gold_file(GOLD_FILE_ADR);

    if(output_file.is_open()) {
        long int sample_num;
        while(std::getline(output_file, out_line)) {
	    out_line.erase(std::remove(out_line.begin(), out_line.end(), ' '), out_line.end());
            if (gold_file.is_open()) {
                std::getline(gold_file, gold_line);
            } else {
                std::cerr << "cannot open gold.txt!";
            }
            sample_num++;
            gold_line.erase(std::remove(gold_line.begin(), gold_line.end(), ' '), gold_line.end());
            if(out_line != gold_line) {
                std::cout <<"sample #"<< sample_num << " : " << out_line << "!=" << gold_line << std::endl;
            }
        }

        output_file.close();
        gold_file.close();
    } else {
        std::cerr << "cannot open output.txt!";
    }
    return 0;
}
#endif
