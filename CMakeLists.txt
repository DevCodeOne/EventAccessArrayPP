cmake_minimum_required(VERSION 3.14)
project(EventAccessArrayPP CXX)

option(ENABLE_TESTING "Build Tests" ON)
option(ENABLE_DYNAMIC_CONTAINERS "Enable dynamic containers" ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(include)

if (ENABLE_TESTING)
    enable_testing()
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest
        GIT_TAG v1.14.0
    )

    set(gtest_force_shared_crt_o ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_subdirectory(tests)
endif()
