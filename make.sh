#! /bin/bash

set -e
set -u


# Compile host programs

hostcxx=g++
hostcxxflags="-Wall -O3 -g -std=c++0x"
ldflags=""

${hostcxx} ${hostcxxflags} -o png2background png2background.cc -lpng

./png2background title backgrounds/title.png backgrounds/title.cc


# Compile target programs

devkitARM_path="${PWD}/../devkitARM"

PATH="${devkitARM_path}/bin:${PATH}"
LD_LIBRARY_PATH="${devkitARM_path}/lib:${LD_LIBRARY_PATH}"

cxx=arm-eabi-g++
cxxflags="-Wall -O3 -g -marm -mthumb-interwork -mcpu=arm7tdmi -mlong-calls -std=gnu++0x"
ldflags="-specs=gba.specs"

${cxx} ${cxxflags} -c medici.cc
${cxx} ${ldflags} -o medici.elf medici.o

arm-eabi-objcopy -O binary -S medici.elf medici.bin
../gbafix medici.bin
ln -sf medici.bin medici.gba
