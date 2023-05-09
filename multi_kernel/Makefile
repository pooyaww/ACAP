##################################################
# Makefile to compile and simulate AIE Template. #
##################################################

# AIE compiler tools
AIE_CC = aiecompiler
AIE_GRAPH_FILES = ./src/project.cc 
AIE_OUT_DIR = ./Work
COMP_LOG_FILE_PATH = ./build.log
# AIE compiler settings
AIE_COMPILE_FLAGS = -v --stacksize=1024 --heapsize=1024 -Xchess=main:darts.xargs=-nb -include="$(XILINX_VITIS)/aietools/include" -include="$(XILINX_HLS)/include" -include="./" -include="./src" -include="./data" -target=hw   -platform=$(PLATFORM) -workdir=$(AIE_OUT_DIR)

#AIE Simulator tools
AIE_SIM = aiesimulator
AIE_SIM_FLAGS = --pkg-dir=$(AIE_OUT_DIR) --i=. --online -wdb
AIE_x86_FLAGS = --target=x86sim --include="src" --workdir=$(AIE_OUT_DIR)
AIE_SIM_LOG = ./sim.log

RM = rm -f
RMDIR = rm -rf

#
# primary build targets
#

all: clean ./libadf.a sim

.check_env_var :
ifndef XILINX_VITIS
	@echo "ERROR XILINX_VITIS environemnt variable not set."
	@exit 1
endif
ifndef XILINX_HLS
	@echo "ERROR XILINX_HLS environemnt variable not set."
	@exit 1
endif

./libadf.a: .check_env_var ./src/project.cc 
	-$(AIE_CC) $(AIE_COMPILE_FLAGS) $(AIE_GRAPH_FILES) | tee $(COMP_LOG_FILE_PATH)
	if [ -f $(COMP_LOG_FILE_PATH) ]; then echo; echo "INFO: Look at the log file ./build.log for more details."; else echo "Warning: Build log file $(COMP_LOG_FILE_PATH) doesn't exist."; fi; \

sim: .check_env_var ./libadf.a $(AIE_OUT_DIR)
	-$(AIE_SIM) $(AIE_SIM_FLAGS) | tee $(AIE_SIM_LOG)

x86: .check_env_var
	-$(AIE_CC) $(AIE_x86_FLAGS) $(AIE_GRAPH_FILES)  | tee $(AIE_SIM_LOG)
clean:
	$(RMDIR) $(AIE_OUT_DIR) ./aiesim* ./ess* ./profile* ./x86*
	$(RM) ./libadf.a *.log *.csv *.db pl_sample_counts *.wdb *.wcfg *.vcd 
	

incremental: ./libadf.a sim

.PHONY: clean .check_env_var ./libadf.a sim incremental all 
