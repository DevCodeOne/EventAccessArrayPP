add_library(EventAccessArrayPP INTERFACE)
target_include_directories(EventAccessArrayPP INTERFACE .)

if (ENABLE_DYNAMIC_CONTAINERS)
message("Enable dynamic containers")
target_compile_definitions(EventAccessArrayPP INTERFACE ENABLE_DYNAMIC_CONTAINER)
endif()