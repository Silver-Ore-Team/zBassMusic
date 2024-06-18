find_package(GTest CONFIG REQUIRED)

add_executable(tests)
target_sources(tests PRIVATE "src/example.cpp")
target_include_directories(tests PRIVATE "src")
target_compile_options(tests PRIVATE /W1)
target_link_libraries(tests PRIVATE GTest::gtest GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)