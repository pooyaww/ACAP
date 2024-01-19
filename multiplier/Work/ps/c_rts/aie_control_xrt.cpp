#include <iostream>
#include "adf/new_frontend/adf.h"
#include "adf/adf_api/X86SimConfig.h"


/************************** Graph Configurations  *****************************/

  adf::X86SimGraphConfig GraphConfigurations[] = {
  // {id, name, runForIter, x86SimPtr}
    {0, "mygraph", -1, nullptr},
  };
  const int NUM_GRAPH = 1;

/************************** PLIO Configurations  *****************************/

  adf::X86SimPLIOConfig PLIOConfigurations[] = {
  //{id, name, loginal_name}
    {0, "mygraph.pl_in0", "DataIn0"},
    {1, "mygraph.pl_in1", "DataIn1"},
    {2, "mygraph.pl_out", "DataOut"},
  };
  const int NUM_PLIO = 3;


/************************** ADF API initializer *****************************/

  class InitializeAIEControlXRT
  {
  public:
    InitializeAIEControlXRT()
    {
      std::cout<<"Initializing ADF API..."<<std::endl;
      adf::initializeX86SimConfigurations(GraphConfigurations, NUM_GRAPH,
                                    nullptr, 0,
                                    nullptr, 0,
                                    PLIOConfigurations, NUM_PLIO,
                                    nullptr, 0);
    }
  } initAIEControlXRT;



#if !defined(__CDO__)

// Kernel Stub Definition
  void aie_multiplier_stream(input_stream<int> *,input_stream<int> *,output_stream<int> *) { /* Stub */ } 
#endif
