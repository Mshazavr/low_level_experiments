# Research Dump 

## CPU Level optimization techniques 

Sources: 
- [Algorithmica](algorithmica.org)
- [Finnish uni course](https://ppc.cs.aalto.fi/ack/)
- [openBLAS](https://github.com/search?q=repo%3AOpenMathLib%2FOpenBLAS+matmul&type=code&p=1)
- [Intel Intrinsics](https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#)
- [SIMD listing](https://en.wikipedia.org/wiki/X86_SIMD_instruction_listings)
- [My CPU](https://www.intel.com/content/www/us/en/products/sku/230493/intel-core-i513600k-processor-24m-cache-up-to-5-10-ghz/specifications.html)
- [My CPU 2](https://www.techpowerup.com/cpu-specs/core-i5-13600k.c2829)

Learn how to check the C program assembly .

Regarding alignment:
- aligned_alloc 
- 64-byte aligned means the address (of the first element of an allocated array) is divisible by 64, 
    which means that it *starts at the beginning of a cache line* (cache lines are typically 64 bytes).
    **Note**: A memory address describes a byte, not a bit. 
- alignas(32) float a[n];

> The lack of fast gather and scatter instructions makes SIMD programming on CPUs 
very different from porper parallel computing environments that support independent memory 
access. You have to always engineer around it and employ various ways of organizing your 
data sequentially so that it can be loaded into registers ("Moving Data" section from [algorithmica](algorithmica.org))

Instruction tables include Latency AND Throughput (or RThroughput for the inverse of the latter). 
If an instruction has latency `x` and throughput `y`, then `x*y` accumulators are required to saturate it 
(assuming x*y logical registers are available). 
- Throughput can be bigger than one due to pipelining and superscalara CPUs (Instruction level Parallelism)
- CPI = cycles per instruction.
- [Intel CPU Instruction Table]()

SPMO
-OpenMP 

Regarding cache associativity: 
- Fully associative (all cache lines map to all memory addresses) 
- direct mapped (each cache lin mapped to a separate subset of addresses)
- set-associative (each set of cache lines ampped to a separate subset of addresses)
- Powers of 2 access/iteration can be pathological due to cache associativity and the way addresses are mapped to cache lines (via simple bit cutting) 
Basically you can end up skipping over a a solid chunk of cache lines always - effectively reducing your cache size. 

x86:
- x86 refers to 32-bit architecture, while x64 (a.k.a. x86-64) refers to 64-bit architecture
- `gcc` treats the source of a C program by default, while g++ treats it as a C++ program. both are part of GCC (GNU Compilier Collection).
GNU++17 is a GCC extension of the C++17 standard that includes additional features, such as, nested functions.
- the reason why its possble to make an add instruction that adds the value at address with size around 64 bits is thanks to addressing modes. 
    - For example, base-displacement addressing allows to specify the address as the value at a register + offset: `ADD R1, [R2 + 0x100000000]`



Debuging with assembly:
- `g++ -ggdb <flags> a.cpp` where (unrelated flags: `native=march --DDEBUG`)
- `gdb -tui ./a.out` 
- `run`
- `layout src` or `layour asm` or `layout split` 
- `disas` 
- `step`, `stepi` 
- `break <line>` or `break <func_name>`
- `continue` 
- `bt` / `bt full` for backtrace


Deep learning: 
- https://videolectures.net/videos/jul09_hinton_deeplearn