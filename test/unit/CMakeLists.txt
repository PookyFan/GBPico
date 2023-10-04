cmake_minimum_required(VERSION 3.10.0)

find_package(GTest)

if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

include(GoogleTest)

file(GLOB GbEmuSrc "${GBPico_SOURCE_DIR}/src/GbEmu/*.cpp")
add_library(GbEmuTestable STATIC ${GbEmuSrc})
target_compile_definitions(GbEmuTestable PRIVATE TEST_MODE=1)
target_include_directories(GbEmuTestable PUBLIC ${GBPico_SOURCE_DIR}/include)


file(GLOB TestSrc "./*.cpp")
add_executable(UnitTestsExec ${TestSrc})
target_compile_definitions(UnitTestsExec PRIVATE TEST_MODE=1)
target_link_libraries(UnitTestsExec GbEmuTestable GTest::gmock_main)
gtest_add_tests(TARGET UnitTestsExec)
