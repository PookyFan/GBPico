cmake_minimum_required(VERSION 3.0.0)

file(GLOB GbEmuSrc "GbEmu/*.cpp")
add_library(GbEmu STATIC ${GbEmuSrc})
target_include_directories(GbEmu PUBLIC ${GBPico_SOURCE_DIR}/include)
