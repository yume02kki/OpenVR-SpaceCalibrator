cmake_minimum_required(VERSION 3.15)
project(OpenVR-SpaceCalibrator LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)

# Vendored includes
include_directories(lib)
include_directories(SYSTEM lib/Eigen)
include_directories(lib/openvr)
include_directories(lib/gl3w/include)
include_directories(lib/imgui)

# Find system libraries
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

# GUI executable
add_executable(openvr-spacecalibrator
    OpenVR-SpaceCalibrator/Calibration.cpp
    OpenVR-SpaceCalibrator/Configuration.cpp
    OpenVR-SpaceCalibrator/EmbeddedFiles.cpp
    OpenVR-SpaceCalibrator/IPCClient.cpp
    OpenVR-SpaceCalibrator/OpenVR-SpaceCalibrator.cpp
    OpenVR-SpaceCalibrator/UserInterface.cpp
    lib/imgui/imgui.cpp
    lib/imgui/imgui_demo.cpp
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_impl_glfw.cpp
    lib/imgui/imgui_impl_opengl3.cpp
    lib/gl3w/src/gl3w.c
)
target_include_directories(openvr-spacecalibrator PRIVATE OpenVR-SpaceCalibrator)
target_compile_options(openvr-spacecalibrator PRIVATE -Wno-template-body)
target_link_libraries(openvr-spacecalibrator glfw OpenGL::GL openvr_api Threads::Threads dl)

# Driver shared library
add_library(driver_spacecalibrator SHARED
    OpenVR-SpaceCalibratorDriver/dllmain.cpp
    OpenVR-SpaceCalibratorDriver/Hooking.cpp
    OpenVR-SpaceCalibratorDriver/InterfaceHookInjector.cpp
    OpenVR-SpaceCalibratorDriver/IPCServer.cpp
    OpenVR-SpaceCalibratorDriver/Logging.cpp
    OpenVR-SpaceCalibratorDriver/OpenVR-SpaceCalibratorDriver.cpp
    OpenVR-SpaceCalibratorDriver/ServerTrackedDeviceProvider.cpp
)
target_include_directories(driver_spacecalibrator PRIVATE OpenVR-SpaceCalibratorDriver)
target_link_libraries(driver_spacecalibrator openvr_api funchook Threads::Threads)
target_compile_options(driver_spacecalibrator PRIVATE -fvisibility=hidden)
