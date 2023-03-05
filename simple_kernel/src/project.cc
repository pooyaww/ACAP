#include <adf.h>

void aie_save(input_stream_int32* in);

class simpleGraph : public adf::graph {
private:
    adf::kernel save;
public:
    adf::input_plio in;

    simpleGraph() {
        in = adf::input_plio::create(adf::plio_32_bits, "data/input0.txt");
        save = adf::kernel::create(aie_save);

	adf::connect<adf::stream> net0(in.out[0], save.in[0]);

	//adf::source(save) = "project.cc";

	adf::source(save) = "kernels.cc";

	adf::runtime<ratio>(save) = 1;
    }
};

simpleGraph mygraph;

int main(void) {
  mygraph.init();
  mygraph.run(4);
  mygraph.end();
  return 0;
}

//void aie_save(input_stream_int32* in) {
//    v4int32 a = readincr_v4(in);
//}
