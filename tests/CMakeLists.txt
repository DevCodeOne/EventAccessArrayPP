add_executable(
    init
    init.cpp
)

target_link_libraries(
    init
    EventAccessArrayPP
    GTest::gtest_main
)

add_executable(
    basic_events
    basic_events.cpp
)

target_link_libraries(
    basic_events
    EventAccessArrayPP
    GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(init)
gtest_discover_tests(basic_events)