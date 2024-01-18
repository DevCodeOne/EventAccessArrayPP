#include "event_access_array.hpp"

#include <gtest/gtest.h>

template<typename T>
struct EleType {
    size_t id;
    T elem;
};

template<typename T>
bool operator==(const EleType<T> &lhs, const EleType<T> &rhs) {
    return lhs.id == rhs.id && lhs.elem == rhs.elem;
}

TEST(BasicEvents, Dynamic) {
    using namespace EAPP;
    using EventArrayType = EventAccessContainer<EleType<const char *>, ContainerType::Dynamic>;

    EventArrayType::ContainerType data{
        EleType<const char *>{0, "First"}, 
        EleType<const char *>{0, "Second"},
        EleType<const char *>{2, "Third"}
    };
    EventArrayType initWithData{data};

    Events::ReadEvent<EleType<const char *>> readFirst{.id = 0};
    initWithData.dispatch(readFirst);

    // First element
    EXPECT_EQ((readFirst.element == EleType<const char *>{0, "First"}), true);
    // Still first element
    EXPECT_EQ((readFirst.element == EleType<const char *>{0, "Second"}), false);

    Events::ReadEvent<EleType<const char *>> readThird{.id = 2};
    initWithData.dispatch(readThird);
    // Third element
    EXPECT_EQ((readThird.element == EleType<const char *>{2, "Third"}), true);
}

TEST(BasicEvents, Fixed) {

}