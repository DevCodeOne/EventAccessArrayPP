#pragma once

#include <cstdint>
#include <cstdlib>
#include <type_traits>
#include <array>

#ifdef ENABLE_DYNAMIC_CONTAINER
#include <vector>
#endif

namespace EAPP {

namespace Events {
    struct AddEvent { };

    struct ReadEvent { };

    struct SetEvent { };

    struct RemoveEvent { };
}

enum struct ContainerType {
    #ifdef ENABLE_DYNAMIC_CONTAINER
    Dynamic, 
    #endif
    Fixed
};

template<size_t N>
using ContainerSize = std::integral_constant<size_t, N>;

namespace Details {
    template<typename T, ContainerType CT, typename ... MoreArgs>
    struct UnderlyingContainer;

    #ifdef ENABLE_DYNAMIC_CONTAINER
    template<typename T, typename ... MoreArgs>
    struct UnderlyingContainer<T, ContainerType::Dynamic, MoreArgs ...> {
        using Type = std::vector<T, MoreArgs ...>;
    };
    #endif

    template<typename T, size_t N>
    struct UnderlyingContainer<T, ContainerType::Fixed, ContainerSize<N>> { 
        using Type = std::array<T, N>;
    };
}

template<typename T, ContainerType CT, typename ... MoreArgs>
class EventAccessContainer {
    public:
    using ContainerType = typename Details::UnderlyingContainer<T, CT, MoreArgs ...>::Type;

    EventAccessContainer() = default;
    EventAccessContainer(ContainerType init) : mData(init) {}
    ~EventAccessContainer() = default;

    EventAccessContainer& operator=(EventAccessContainer &other);
    EventAccessContainer& operator=(EventAccessContainer &&other);

    private:
    ContainerType mData;
};

}