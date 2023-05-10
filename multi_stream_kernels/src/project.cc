#include <adf.h>
#include <iostream>
#include <string>
#include <fstream>

#define GOLD_FILE_ADR "./data/golden.txt"
#define IN_FILE_ADR "data/input.txt"
#define OUT_FILE_ADR "data/output.txt"
#define X86_OUT_FILE_ADR "./x86simulator_output/data/output.txt"

void aie_read(input_stream_int32* in, output_stream_int32* out);
void aie_write(input_stream_int32* in, output_stream_int32* out);

class simpleGraph : public adf::graph {
private:
    adf::kernel read;
    adf::kernel write;
public:
    adf::input_plio in;
    adf::output_plio out;

    simpleGraph() {
        in = adf::input_plio::create(adf::plio_32_bits, IN_FILE_ADR);
        out = adf::output_plio::create(adf::plio_32_bits, OUT_FILE_ADR);

        read = adf::kernel::create(aie_read);
        write = adf::kernel::create(aie_write);

	adf::connect<adf::stream> net0(in.out[0], read.in[0]);
	adf::connect<adf::stream> net1(read.out[0], write.in[0]);
	adf::connect<adf::stream> net2(write.out[0], out.in[0]);

	adf::source(read) = "kernels.cc";
	adf::source(write) = "kernels.cc";

	adf::runtime<ratio>(read)  = 1;
	adf::runtime<ratio>(write) = 1;
    }
};

simpleGraph mygraph;

int main(void) {
    adf::return_code ret;

    std::ifstream input_file(IN_FILE_ADR);
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
    std::ifstream output_file(X86_OUT_FILE_ADR);
    std::ifstream gold_file(GOLD_FILE_ADR);

    if(output_file.is_open()) {
        long int sample_num;
        while(std::getline(output_file, out_line)) {
            sample_num++;
            if (gold_file.is_open()){
                std::getline(gold_file, gold_line);
                out_line.erase(std::remove(out_line.begin(), out_line.end(), ' '), out_line.end());
                gold_line.erase(std::remove(gold_line.begin(), gold_line.end(), ' '), gold_line.end());
                if(out_line != gold_line) {
                    std::cout <<"sample #"<< sample_num << " : " << out_line << "!=" << gold_line << std::endl;
                }
            } else {
                std::cerr << "cannot open output.txt!";
            }
        }

        output_file.close();
        gold_file.close();
    } else {
        std::cerr << "cannot open output.txt!";
    }
    return 0;
}
