# Install script for directory: D:/cm2025_src/zephyr/drivers

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Zephyr-Kernel")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/Actions/ZEPHYR_SDK/1.02/gcc-arm-none-eabi-9-2020-q2-update-win32/bin/arm-none-eabi-objdump.exe")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/drivers/console/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/drivers/cfg_drv/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/drivers/serial/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/drivers/i2c/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/drivers/gpio/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/drivers/dma/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/drivers/flash/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/drivers/ipmsg/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/drivers/misc/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/drivers/bluetooth/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/drivers/nvram/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/drivers/audio/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/drivers/dsp/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/drivers/adc/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/drivers/input/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/drivers/power_supply/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/drivers/usb/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/drivers/watchdog/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/drivers/timer/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/drivers/test/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/drivers/pwm/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/drivers/led/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/drivers/mpu/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/drivers/compensation/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/drivers/dvfs/cmake_install.cmake")

endif()

