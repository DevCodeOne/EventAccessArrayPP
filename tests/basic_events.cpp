#include "event_access_array.hpp"

#include <gtest/gtest.h>

template<typename T>
struct EleType {
    T elem;
};

template<typename T>
bool operator==(const EleType<T> &lhs, const EleType<T> &rhs) {
    return lhs.elem == rhs.elem;
}

TEST(BasicEvents, Dynamic) {
    using namespace EAPP;
    using EventArrayType = EventAccessContainer<ForwardId<256u>, EleType<const char *>, ContainerType::Dynamic>;

    std::array<EleType<const char *>, 3> data{
        EleType<const char *>{"First"},
        EleType<const char *>{"Second"},
        EleType<const char *>{"Third"}
    };

    EventArrayType basic{};

    Events::AddEvent<EventArrayType> addFirst{.element = data[0]};
    basic.dispatch(addFirst);
    Events::AddEvent<EventArrayType> addSecond{.element = data[0]};
    basic.dispatch(addSecond);
    Events::AddEvent<EventArrayType> addThird{.element = data[1]};
    basic.dispatch(addThird);


    EXPECT_EQ(addFirst.toId, 0);
    EXPECT_EQ(addFirst.element, data[0]);
    EXPECT_EQ(addSecond.toId, 1);
    EXPECT_EQ(addSecond.element, data[0]);
    EXPECT_EQ(addThird.toId, 2);
    EXPECT_EQ(addThird.element, data[1]);

}

TEST(BasicEvents, Fixed) {
    using namespace EAPP;
    using EventArrayType = EventAccessContainer<ForwardId<256u>, EleType<const char *>, ContainerType::Fixed, ContainerSize<15>>;

    std::array<EleType<const char *>, 3> data{
        EleType<const char *>{"First"},
        EleType<const char *>{"Second"},
        EleType<const char *>{"Third"}
    };

    EventArrayType basic{};

    Events::AddEvent<EventArrayType> addFirst{.element = data[0]};
    basic.dispatch(addFirst);
    Events::AddEvent<EventArrayType> addSecond{.element = data[0]};
    basic.dispatch(addSecond);
    Events::AddEvent<EventArrayType> addThird{.element = data[1]};
    basic.dispatch(addThird);


    EXPECT_EQ(addFirst.toId, 0);
    EXPECT_EQ(addFirst.element, data[0]);
    EXPECT_EQ(addSecond.toId, 1);
    EXPECT_EQ(addSecond.element, data[0]);
    EXPECT_EQ(addThird.toId, 2);
    EXPECT_EQ(addThird.element, data[1]);

    // // First element
    // EXPECT_EQ((readFirst.element == EleType<const char *>{"First"}), true);
    // // Still first element
    // EXPECT_EQ((readFirst.element == EleType<const char *>{"Second"}), false);

    // Events::ReadEvent<EleType<const char *>> readThird{.id = 2};
    // initWithData.dispatch(readThird);
    // // Third element
    // EXPECT_EQ((readThird.element == EleType<const char *>{"Third"}), true);
}