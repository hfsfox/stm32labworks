# stm32labworks
Lab works and experiments with STM32F401xx MCU

To build this you need installed:
```
git
CMake => 3.30
arm-none-eabi-gcc
```

For setup your build envirioment:
```
git clone https://github.com/ARM-software/CMSIS_5.git CMSIS
cd CMSIS
git checkout tags/5.9.0
mkdir -p Device/ST
git clone https://github.com/STMicroelectronics/cmsis_device_f4.git Device/ST/STM32F4
cd ..
```

For build:
```
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make
```
And now in build catalog you can find some .elf .bin and .hex files

If you using Proteus you should use hex files as firmware for project 
