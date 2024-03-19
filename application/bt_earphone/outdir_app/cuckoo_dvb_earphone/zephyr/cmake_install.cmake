# Install script for directory: D:/cm2025_src/zephyr

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
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/arch/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/lib/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/soc/arm/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/boards/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/subsys/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/drivers/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/modules/cmsis/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/modules/fatfs/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/modules/segger/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/modules/tinycrypt/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/modules/littlefs/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/modules/base/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/modules/system/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/modules/audio/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/modules/media/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/modules/ota/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/modules/bluetooth/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/modules/display/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/modules/usb/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/kernel/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/cmake/flash/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/cmake/usage/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/cmake/reports/cmake_install.cmake")
  include("D:/cm2025_src/application/bt_earphone/outdir_app/cuckoo_dvb_earphone/zephyr/framework/cmake_install.cmake")

endif()

