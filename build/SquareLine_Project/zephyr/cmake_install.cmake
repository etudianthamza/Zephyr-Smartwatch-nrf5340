# Install script for directory: /home/hamza/ncs/v3.2.1/zephyr

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
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
  set(CMAKE_OBJDUMP "/home/hamza/ncs/toolchains/43683a87ea/opt/zephyr-sdk/arm-zephyr-eabi/bin/arm-zephyr-eabi-objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/zephyr/arch/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/zephyr/lib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/zephyr/soc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/zephyr/boards/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/zephyr/subsys/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/zephyr/drivers/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/nrf/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/hostap/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/mcuboot/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/mbedtls/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/trusted-firmware-m/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/cjson/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/azure-sdk-for-c/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/cirrus-logic/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/openthread/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/memfault-firmware-sdk/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/canopennode/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/chre/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/lz4/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/zscilib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/cmsis/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/cmsis-dsp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/cmsis-nn/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/cmsis_6/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/fatfs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/hal_nordic/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/hal_st/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/hal_tdk/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/hal_wurthelektronik/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/liblc3/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/libmetal/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/littlefs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/loramac-node/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/lvgl/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/mipi-sys-t/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/nanopb/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/nrf_wifi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/open-amp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/percepio/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/picolibc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/segger/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/uoscore-uedhoc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/zcbor/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/nrfxlib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/nrf_hw_models/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/modules/connectedhomeip/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/zephyr/kernel/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/zephyr/cmake/flash/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/zephyr/cmake/usage/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/hamza/Projects_RTOS/projet_montre_connecte/neveau_dessin/SquareLine_Project/build/SquareLine_Project/zephyr/cmake/reports/cmake_install.cmake")
endif()

