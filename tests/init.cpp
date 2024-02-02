#include "event_access_array.hpp"

#include <gtest/gtest.h>

TEST(InitTests, Dynamic) {
    using namespace EAPP;
    using EventArrayType = EventAccessContainer<ForwardId<256u>, int, ContainerType::Dynamic>;

    EventArrayType defaultInit{};
    EventArrayType initWithData{1, 5, 6};
}

TEST(InitTests, Fixed) {
    using namespace EAPP;

    using EventArrayType = EventAccessContainer<ForwardId<256u>, int, ContainerType::Fixed, ContainerSize<15>>;

    EventArrayType defaultInit{};
    EventArrayType initWithData{1, 5, 6};
}