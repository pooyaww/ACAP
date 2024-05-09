#include "systemc.h"
#include <hls_stream_thread_safe.h>
#include <ap_axi_sdata.h>
#include <map>
#include <thread>
#include "adf/new_frontend/adf.h"
#include "adf/x86sim/x86sim.h"
#include "adf/buffer_port/buffer_port.h"
#include "adf/adf_api/X86SimConfig.h"

////// Kernel function Wrapper declarations //////
void b1_kernel_wrapper(x86sim::stream_internal *, x86sim::stream_internal *);

////// Class kernel dtor wrapper extern declaration //////

////// Class kernel ctor wrapper extern declaration //////

////// Kernel Inits extern declaration //////

namespace {
} // namespace
namespace x86sim
{

////// Kernel Classes //////

class Kernel_b1__3309 : public IMEKernel
{
public: 
    Kernel_b1__3309(ISimulator *sim, std::string label)
    : IMEKernel(sim, label)
    {
    }

protected: 
    virtual void invokeKernel() override
    {
        b1_kernel_wrapper(
          ((IStreamConnector*) input(0)) -> stream(),
          ((IStreamConnector*) output(0)) -> stream()
        );
    }
};

////// Set Initial Value for input async RTP //////

static void initValue(IRtpConnector* rtp, int8_t* val, size_t bytes)
{
    updateRtp(val, bytes,rtp);
}

ISimulator *createSimulator(ISimulator::Kind kind)
{
    ISimulatorConfig simConfig = {};
    simConfig._enableProgress = true;
    simConfig._enableEventTrace = std::getenv("X86SIM_EVENT_TRACE_ON");
    simConfig._enableEventTracePrint = std::getenv("X86SIM_EVENT_TRACE_PRINT_ON");
    simConfig._enableSnapshots = std::getenv("X86SIM_SNAPSHOTS_ON");
    simConfig._simTimeout = 0;
    simConfig._plWaitTime = 0;
    simConfig._meKernelIters = 0;

    if (ISimulator::Kind::kBasic == kind)
    {
        auto simParams = createSimParams();
        simParams -> meKernelIters = -1 /* aiecompiler argument value */ ;
        if (!simParams -> populate())
            std::exit(EXIT_FAILURE);
        simConfig._simTimeout = simParams->simTimeout;
        simConfig._plWaitTime = simParams->plWaitTime;
        simConfig._meKernelIters = simParams->meKernelIters;
        simConfig._enableStopOnDeadlock = std::getenv("X86SIM_STOP_ON_DEADLOCK");
        simConfig._enableHandShakeWithExtTb = std::getenv("X86SIM_HANDSHAKE_EXT_TB");
        simConfig._socketIO = false;
        if (auto env = std::getenv("PACKAGEDIR"))
            simConfig._optionsFile = env + std::string("/options/x86sim.options");
    }
    if (ISimulator::Kind::kSwemu == kind)
    {
        simConfig._meKernelIters = -1;
        if (auto optionFilePath = std::getenv("X86SIM_OPTIONSPATH"))
            simConfig._optionsFile = optionFilePath ;
    }
    if (simConfig._simTimeout == 0)
        if (auto val = std::getenv("X86SIM_TIMEOUT"))
            simConfig._simTimeout = std::atoi(val);

    if (!(simConfig._optionsFile).empty())
        ISimulatorOptions::processFile(simConfig);

    if (auto inputDir = std::getenv("INPUTDIR"))
        simConfig._inputDir = inputDir;
    if (auto outputDir = std::getenv("OUTPUTDIR"))
        simConfig._outputDir = outputDir;
    if ((simConfig._inputDir).empty())
        simConfig._inputDir = ".";
    if ((simConfig._outputDir).empty())
        simConfig._outputDir = "./x86simulator_output";

    ISimulator *sim = SimulatorFactory::simulator(simConfig, kind);
    IBasicSimulator *basicSim = dynamic_cast<IBasicSimulator*>(sim);
    ISwemuSimulator *swemuSim = dynamic_cast<ISwemuSimulator*>(sim);
    IXmcSimulator *xmcSim = dynamic_cast<IXmcSimulator*>(sim);
    IStreamConnector *scon_i0_po0_i1_pi0
        = ConnectorFactory::streamConnector(sim, "scon_i0_po0_i1_pi0");
    sim -> addConnectorMetaData(
        {scon_i0_po0_i1_pi0, "stream", "int32", "cgra_dataflow._5040.out[0]", "cgra_dataflow.k_3309.in[0]", "out", "in", 1, 1});
    IStreamConnector *scon_i1_po0_i2_pi0
        = ConnectorFactory::streamConnector(sim, "scon_i1_po0_i2_pi0");
    sim -> addConnectorMetaData(
        {scon_i1_po0_i2_pi0, "stream", "int32", "cgra_dataflow.k_3309.out[0]", "cgra_dataflow._5041.in[0]", "out", "in", 1, 1});
    // Graph configs
    // {id, name, test-iterations, x86SimPtr}
    DFGraph *gr_cgra_dataflow = new DFGraph(sim);
    sim->addGraphConfig({ 0, "cgra_dataflow", -1, gr_cgra_dataflow});
    Kernel_b1__3309 *ker_i1
        = new Kernel_b1__3309(sim, "ker_i1");
    gr_cgra_dataflow -> addKernel(ker_i1);
    ker_i1 -> addInput(scon_i0_po0_i1_pi0);
    ker_i1 -> addOutput(scon_i1_po0_i2_pi0);
    sim -> addNodeMetaData({ker_i1, "cgra_dataflow.k_3309", {
        {scon_i0_po0_i1_pi0, {"in[0]"}}
        , {scon_i1_po0_i2_pi0, {"out[0]"}}
    }});

    if (basicSim)
    {
        INode *platformIn_i0
            = NodeFactory::fileReader
            (sim, "platformIn_i0",
            simConfig._inputDir + "/in.txt", 
            false, INT32, 4, false);
        platformIn_i0 -> addOutput(scon_i0_po0_i1_pi0);

        INode *platformOut_i2
            = NodeFactory::fileWriter
            (sim, "platformOut_i2",
            simConfig._outputDir + "/out.txt", 
            false, INT32, 4, false);
        platformOut_i2 -> addInput(scon_i1_po0_i2_pi0);


        auto configs = basicSim->getConfig();
        adf::initializeX86SimConfigurations(
            std::get<0>(configs),
            std::get<1>(configs),
            std::get<2>(configs),
            std::get<3>(configs),
            std::get<4>(configs),
            std::get<5>(configs),
            std::get<6>(configs),
            std::get<7>(configs),
            std::get<8>(configs),
            std::get<9>(configs));
    }
    if (swemuSim)
    {
        IPlatformStreamNode *platformIn_i0
            = NodeFactory::streamReader(sim, "platformIn_i0", false);
        swemuSim->registerHlsStreamNode(platformIn_i0, "dd");
        platformIn_i0 -> addOutput(scon_i0_po0_i1_pi0);

        IPlatformStreamNode *platformOut_i2
            = NodeFactory::streamWriter(sim, "platformOut_i2", false);
        swemuSim->registerHlsStreamNode(platformOut_i2, "ss");
        platformOut_i2 -> addInput(scon_i1_po0_i2_pi0);

    }
    if (xmcSim)
    {
        IPlatformBuffer * bufplatformIn_i0
            = PlatformBufferFactory::inputBuffer(sim, "bufplatformIn_i0", 32768);
        INode *platformIn_i0
            = NodeFactory::bufferReader(sim, "platformIn_i0", bufplatformIn_i0, 4, 4);
        platformIn_i0 -> addOutput(scon_i0_po0_i1_pi0);

        IPlatformBuffer * bufplatformOut_i2
            = PlatformBufferFactory::outputBuffer(sim, "bufplatformOut_i2", 32768);
        INode *platformOut_i2
            = NodeFactory::bufferWriter(sim, "platformOut_i2", bufplatformOut_i2, 4, 4);
        platformOut_i2 -> addInput(scon_i1_po0_i2_pi0);

    }
    if ( !xmcSim)
    {
        sim->start();
    }
    return sim;
}

void *createBasicSimulatorInstance()
{
    static auto  g_sim = std::unique_ptr<ISimulator>
        (createSimulator(ISimulator::Kind::kBasic));
    return g_sim.get();
}

void *createSwemuSimulatorInstance()
{
    static auto  g_sim = std::unique_ptr<ISimulator>
        (createSimulator(ISimulator::Kind::kSwemu));
    return dynamic_cast<x86sim::ISwemuSimulator*>(g_sim.get());
}

void *createXmcSimulator()
{
    auto sim = createSimulator(ISimulator::Kind::kXmcGraph);
    return dynamic_cast<x86sim::IXmcSimulator*>(sim);
}

}  //end of x86Sim namespace


#include "adf/x86sim/symbolVisibility.h"
#include "adf/x86sim/x86simSwemuIfc.hpp"

