#pragma once

#include <cstdint>
#include <cstdlib>
#include <type_traits>
#include <array>
#include <shared_mutex>
#include <mutex>
#include <optional>

#ifdef ENABLE_DYNAMIC_CONTAINER
#include <vector>
#endif

namespace EAPP {

namespace Events {
    template<typename T>
    struct IdType {
        using Type = decltype(std::declval<T>().id);
    };

    template<typename T>
    struct AddEvent {
        T element;
        bool result = false;

        bool operator()(std::optional<T> &element) noexcept {
            if (element.has_value()) {
                return false;
            }

            element = this->element;
            return true;
        }
    };

    template<typename T>
    struct ReadEvent { 
        T element;
        typename IdType<T>::Type id;

        bool operator()(const std::optional<T> &element) noexcept {
            if (!element.has_value()) {
                return false;
            }

            if (element->id != id) {
                return false;
            }

            this->element = *element;
            return true;
        }
    };

    template<typename T, typename Id>
    struct SetEvent {
        T element;
        Id id;
    };

    template<typename Id>
    struct RemoveEvent { 
        Id id;
    };
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
        using Type = std::vector<std::optional<T>, MoreArgs ...>;
    };
    #endif

    template<typename T, size_t N>
    struct UnderlyingContainer<T, ContainerType::Fixed, ContainerSize<N>> { 
        using Type = std::array<std::optional<T>, N>;
    };
}

template<typename T, ContainerType CT, typename ... MoreArgs>
class EventAccessContainer {
    public:
    using ContainerType = typename Details::UnderlyingContainer<T, CT, MoreArgs ...>::Type;

    EventAccessContainer() = default;
    EventAccessContainer(ContainerType init) : mData(init) {}
    ~EventAccessContainer() = default;

    EventAccessContainer& operator=(EventAccessContainer &other) = delete;
    EventAccessContainer& operator=(EventAccessContainer &&other) = delete;

    template<typename Event>
    void dispatch(Event &event) noexcept;

    template<typename Event>
    void dispatch(Event &event) const noexcept;

    private:
    ContainerType mData;
    mutable std::shared_mutex instanceMutex;
};


template<typename T, ContainerType CT, typename ... MoreArgs>
template<typename Event>
void EventAccessContainer<T, CT, MoreArgs ...>::dispatch(Event &event) noexcept {
    std::unique_lock<std::shared_mutex> instanceGuard{instanceMutex};

    for (const auto &currentElement : mData) {
        if (event(currentElement)) {
            return;
        }
    }
}

template<typename T, ContainerType CT, typename ... MoreArgs>
template<typename Event>
void EventAccessContainer<T, CT, MoreArgs ...>::dispatch(Event &event) const noexcept {
    std::shared_lock<std::shared_mutex> instanceGuard{instanceMutex};

    for (const auto &currentElement : mData) {
        if (event(currentElement)) {
            return;
        }
    }
}

}