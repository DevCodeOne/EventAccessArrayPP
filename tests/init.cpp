#include "event_access_array.hpp"

#include <gtest/gtest.h>

TEST(InitTests, Dynamic) {
    using namespace EAPP;
    using EventArrayType = EventAccessContainer<int, ContainerType::Dynamic>;

    EventArrayType defaultInit{};

    EventArrayType::ContainerType data{1, 5, 6};
    EventArrayType initWithData{data};
}

TEST(InitTests, Fixed) {
    using namespace EAPP;

    using EventArrayType = EventAccessContainer<int, ContainerType::Fixed, ContainerSize<15>>;

    EventArrayType defaultInit{};
    EventArrayType::ContainerType data{1, 5, 6};
    EventArrayType initWithData{data};
}