#include <adf.h>
#define iter_num 4
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
        in = adf::input_plio::create(adf::plio_32_bits, "data/input.txt");
        out = adf::output_plio::create(adf::plio_32_bits, "data/output.txt");

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
	//TODO: increase the run size for all samples dynamically
	//TODO: use popen to diff between generated files
  mygraph.init();
  std::cout << "Graph initialized" <<std::endl;
  mygraph.run(iter_num);
  std::cout << "Graph executed " << iter_num << " times"<< std::endl;
  mygraph.end();
  std::cout << "Graph end." <<std::endl;
  return 0;
}
