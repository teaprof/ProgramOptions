add_executable(unit_tests program_mode_options_test.cpp)
target_link_libraries(unit_tests GTest::gtest_main Boost::program_options)
target_include_directories(unit_tests PUBLIC GTEST_INCLUDE_DIRS)

