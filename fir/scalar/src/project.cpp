#include <adf.h>
#include "kernels.h"
#include "project.h"

using namespace adf;

fir_asym_8t_16int_graph mygraph;
//simulation::platform<1,1> platform("../data/input.txt", "data/output.txt");
//connect<> net0(platform.src[0], mygraph.in);
//connect<> net1(mygraph.out, platform.sink[0]);

int main(void) {
  mygraph.init();
  mygraph.run(1);
  mygraph.end();
  return 0;
}
