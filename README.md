# EPOXY Embedded Privilege Overlays for X hardware with Y software

This is the compiler used in
"Protecting Bare-metal Embedded Systems With Privilege Overlays",
from IEEE Security and Privacy 2017 [Paper](http://hexhive.github.io/publications/files/17Oakland.pdf)

## Preqs

This tool has only been tested on Ubuntu 14.04, using Clang 3.9 to build LLVM
Other versions may work, but are untested.

Requires
* cmake, tested with 3.6.2
* ninja
* python 2.7
* llvm build preqs, see http://llvm.org/docs/GettingStarted.html
* gcc build preqs, see https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads

Building LLVM is memory intensive recommend >=16GB of memory.

## Setup
To setup clone this repo to a directory <YOUR_DIR>, this can be any name you choose
```
cd <YOUR_DIR>
git clone git@github.com:HexHive/EPOXY.git
```

Then cd into EPOXY and run the following scripts
```
./get_repos.sh  #this will clone and setup the llvm and clang repos
./cmake_config.sh  #this will run the properly cmake command for llvm
./setup_gcc.sh  #this will download a copy of arm-none-eabi gcc toolchain
		# and build it with the linker plugin support
		# EPOXY uses arm-none-eabi-ld and the standard libs from this build
```

This will build the arm-none-eabi toolchain with libraries, and
create the following directory structure.

```
<YOUR_DIR>
  |--> EPOXY (This Repo)
  |--> EPOXY-llvm   (The EPOXY-llvm repo)
  |--> EPOXY-clang  (The EPOXY-clang repo)
  |--> llvm_build   (The directory llvm will be built in)
  |--> llvm_bin     (The directory llvm's binaries will be installed)
  |--> gcc
        |-->bins    (Where the arm-none-eabi-gcc tool chain gets installed to)

```

Notes:

It appears that URL used to download GCC changes from time to time. This will
cause setup_gcc.sh to fail. EPOXY has been tested using the 6-2017-q1-update
release from https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads
Their build was modified to add support for the plugin with the linker by
modifying the build_toolchain.sh script to the options under the binutils
target.

```
    --enable-plugins \
    --enable-lto \
    --enable-gold \
```

## Building LLVM

After completing setup

```
cd <YOUR_DIR>/llvm_build
ninja install

```

Now build EPOXY's runtime
```
cd <YOUR_DIR>/EPOXY/EPOXY-rt
make
```
Any program using EPOXY should include the created rt_edivert.o in its final
linking step in order to use it runtime support code

## Building Beebs Benchmarks

After successfully compiling LLVM you can build the beebs benchmarks
for the STM32F4Discovery board.

```
cd <YOUR_DIR>/EPOXY/beebs/tools
python build_experiments.py -s #Sets up all the benchmarks for building
python build_experiments.py -m -n=1 #Builds all benchmarks with 1 variants of each
python build_experiments.py -c  #Cleans all the benchmarks

```

All binaries are placed in <YOUR_DIR>/EPOXY/beebs/bins

Individual benchmarks can be built by cd(ing) into the appropriate
benchmark directory (i.e. <YOUR_DIR>/EPOXY/beebs/src/<benchmark>) and running make
See build_experiments.py for options for make.

## Thanks
This repo includes a version of the BEEBs benchmarks, which are GPL licensed
Their repo can be found at https://github.com/mageec/beebs

It also includes a the STM32CubeF4 HAL which uses a BSD license.
http://www.st.com/en/embedded-software/stm32cubef4.html

## License
Our modifications and tools are distributed using license in License.md
