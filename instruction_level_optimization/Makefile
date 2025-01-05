compiler = g++ 
flags = -ggdb -march=native -DDEBUG -I./include -I${MKLROOT}/include -L${MKLROOT}/lib
libs = -lmkl_intel_lp64 -lmkl_core -lmkl_intel_thread -liomp5 -lpthread -lm
obj_dir = build
bin_dir = bin

objects = arena_allocator.o \
	      debug.o \
		  machine_info.o \
		  timer.o \
		  matrix.o \
		  run_info.o \
		  cpu_gemm_runner.o \
		  custom_kernels.o \
		  mkl_kernels.o

binaries = cpu_gemm
test_binaries = cpu_gemm_tests

%.o: src/**/%.cpp
	$(compiler) $(flags) -c $< -o $(obj_dir)/$@ $(libs)

%.o: src/%.cpp
	$(compiler) $(flags) -c $< -o $(obj_dir)/$@ $(libs)


all: dirs $(binaries)


#################################################### bin and build directories

dirs:
	@mkdir -p $(obj_dir)
	@mkdir -p $(bin_dir)

$(bin_dir):
	@mkdir -p $(bin_dir)


##################################################### universal objects

arena_allocator.o: src/utils/arena_allocator.cpp
debug.o: src/utils/debug.cpp
machine_info.o: src/utils/machine_info.cpp
timer.o: src/utils/timer.cpp
matrix.o: src/matrix.cpp
run_info.o: src/run_info.cpp



##################################################### cpu_gemm 

cpu_gemm_runner.o: src/cpu_gemm/cpu_gemm_runner.cpp
custom_kernels.o: src/cpu_gemm/custom_kernels.cpp
mkl_kernels.o: src/cpu_gemm/mkl_kernels.cpp

cpu_gemm: $(objects)
	$(compiler) $(flags) src/cpu_gemm/main.cpp -o $(bin_dir)/cpu_gemm $(libs) $(addprefix $(obj_dir)/, $(objects))



########################## cpu_gemm tests

cpu_gemm_tests: src/cpu_gemm/tests/test_loaders.cpp
	$(compiler) $(flags) src/cpu_gemm/tests/test_loaders.cpp -o $(bin_dir)/cpu_gemm_tests $(libs) $(addprefix $(obj_dir)/, $(objects))

test: $(test_binaries)
	@for exec in $(test_binaries); do \
		echo "Running $$exec"; \
		./bin/$$exec; \
	done



.PHONY: clean
clean:
	rm -rf $(obj_dir)
	rm -rf $(bin_dir)