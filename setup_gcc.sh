
SETUP_DIR=$(pwd)

if [ ! -e ../gcc/bins ]
then
  mkdir -p ../gcc
  cd ../gcc
  if [ ! -e gcc-arm-none-eabi-6-2017-q1-update/pkg ]
  then
     if [ ! -e gcc-arm-none-eabi-6-2017-q1-update-src.tar.bz2 ]
     then
       #wget https://developer.arm.com/-/media/Files/downloads/gnu-rm/6_1-2017q1/gcc-arm-none-eabi-6-2017-q1-update-src.tar.bz2?product=GNU%20ARM%20Embedded%20Toolchain,Invariant,,Source,6-2017-q1-update
       wget https://developer.arm.com/-/media/Files/downloads/gnu-rm/6_1-2017q1/gcc-arm-none-eabi-6-2017-q1-update-src.tar.bz2
     fi
     tar -xjf gcc-arm-none-eabi-6-2017-q1-update-src.tar.bz2
     cd gcc-arm-none-eabi-6-2017-q1-update
     cp ${SETUP_DIR}/gcc_build_toolchain.sh build-toolchain.sh
     cd src
     find -name '*.tar.*' | xargs -I% tar -xf %
     cd ..
     ./build-prerequisites.sh --skip_steps=mingw32
     ./build-toolchain.sh --skip_steps=mingw32
     cd .. #should be at gcc
  fi
  cd gcc-arm-none-eabi-6-2017-q1-update/pkg
  tar -xjf gcc-arm-none-eabi-6-2017-q1-update-linux.tar.bz2
  mv gcc-arm-none-eabi-6-2017-q1-update ../../bins
  cd ${SETUP_DIR}
fi
