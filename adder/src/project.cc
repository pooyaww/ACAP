#include <adf.h>
#include <iostream>
#include <string>
#include <fstream>


#define LANE_SIZE 4
#define GOLD_FILE_ADR "./data/golden.txt"
#define IN0_FILE_ADR "data/input0.txt"
#define IN1_FILE_ADR "data/input1.txt"
#define OUT_FILE_ADR "data/output.txt"

#if defined(__AIESIM__)
  #define SIM_OUT_FILE_ADR "./aiesimulator_output/data/output.txt"
#endif

#if defined(__X86SIM__)
    #define SIM_OUT_FILE_ADR "./x86simulator_output/data/output.txt"
#endif

using namespace adf;

void aie_adder(input_stream_int32* in0, input_stream_int32* in1, output_stream_int32* out);

class simpleGraph : public graph {
   private:
    kernel adder;

   public:
    input_plio pl_in0, pl_in1;
    output_plio pl_out;

    simpleGraph() {
        adder = kernel::create(aie_adder);

        pl_in0 = input_plio::create("DataIn0", adf::plio_32_bits, IN0_FILE_ADR);
        pl_in1 = input_plio::create("DataIn1", adf::plio_32_bits, IN1_FILE_ADR);
        pl_out = output_plio::create("DataOut", adf::plio_32_bits, OUT_FILE_ADR);

	connect<stream> net0(pl_in0.out[0], adder.in[0]);
        connect<stream> net1(pl_in1.out[0], adder.in[1]);
        connect<stream> net2(adder.out[0], pl_out.in[0]);

        source(adder) = "kernel.cc";

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

    long int iter_num = sample_size/4;

    mygraph.init();
    std::cout << "Graph initialized" <<std::endl;
    ret = mygraph.run(iter_num);
    if (ret != adf::ok) {
        std::cout << "Run failed\n";
	return ret;
    }
    std::cout << "Graph executed " << iter_num << " times"<< std::endl;
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
            if (gold_file.is_open()){
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
