#pragma once

#include <ranges>
#include <algorithm>
#include <concepts>
#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <type_traits>
#include <array>
#include <shared_mutex>
#include <mutex>
#include <optional>

#ifdef ENABLE_DYNAMIC_CONTAINER
#include <vector>
#endif

namespace EAPP {
    // TODO: implement, with reusage of ids
    template<std::unsigned_integral auto MaxIds>
    struct IdPool { };

    // No reusage of ids
    template<std::unsigned_integral auto MaxIds>
    struct ForwardId {
        using ValueType = decltype(MaxIds);
        static inline constexpr ValueType InvalidId = MaxIds;

        static auto generate(ForwardId &id) {
            return id.generate();
        }

        auto operator()() {
            return generate();
        }

        private:

        auto generate() {
            if (last < MaxIds) {
                return last++;
            }

            return InvalidId;
        }

        ValueType last = 0;
    };

namespace Events {

    template<typename ContainerType>
    struct AddEvent {
        using ValueType = typename ContainerType::ValueType;
        using IdType = typename ContainerType::IdType;
        using IdValueType = typename ContainerType::IdValueType;
        using EventContext = typename ContainerType::EventContext;

        ValueType element;
        IdValueType toId = IdType::InvalidId;

        bool operator()(IdValueType &id, std::optional<ValueType> &element, EventContext &context) noexcept {
            if (id != IdType::InvalidId) {
                return false;
            }

            id = context.genId();
            toId = id;
            element = this->element;
            return true;
        }
    };

    /*
    template<typename T, typename IdType>
    struct ReadEvent { 
        T element;
        IdType id;

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

    template<typename T, typename IdType>
    struct SetEvent {
        T element;
        IdType id;
    };

    template<typename IdType>
    struct RemoveEvent { 
        IdType id;
    };
    */
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
    template<typename IdType, typename T, ContainerType CT, typename ... MoreArgs>
    struct UnderlyingContainer;

    #ifdef ENABLE_DYNAMIC_CONTAINER
    template<typename IdValueType, typename T, typename ... MoreArgs>
    struct UnderlyingContainer<IdValueType, T, ContainerType::Dynamic, MoreArgs ...> {
        using Type = std::vector<std::pair<IdValueType, std::optional<T>>, MoreArgs ...>;

        static auto outputIterator(Type &type) {
            return std::back_inserter(type);
        }

        static auto expandContainer(Type &type, IdValueType initIdValue) {
            // TODO: can potentially throw exceptions, handle them
            const auto sizeDifference = (type.size() * 2 + 1) - type.size();
            type.reserve(sizeDifference);
            std::fill_n(std::back_inserter(type), sizeDifference, std::make_pair(initIdValue, std::nullopt));

            return true;
        }
    };
    #endif

    template<typename IdValueType, typename T, std::unsigned_integral auto N>
    requires(N > 0)
    struct UnderlyingContainer<IdValueType, T, ContainerType::Fixed, ContainerSize<N>> {
        using Type = std::array<std::pair<IdValueType, std::optional<T>>, N>;

        static auto outputIterator(Type &type) { return type.begin(); }

        static auto expandContainer(Type &type, IdValueType) { return false; }
    };
}

template<typename IT, typename T, ContainerType CT, typename ... MoreArgs>
class EventAccessContainer {
    public:
    using IdType = IT;
    using IdValueType = typename IdType::ValueType;
    using UnderlyingContainerType = typename Details::UnderlyingContainer<IdValueType, T, CT, MoreArgs ...>;
    using ContainerType = typename UnderlyingContainerType::Type;
    using ValueType = typename ContainerType::value_type::second_type::value_type;

    struct EventContext {
        IdType &genId;
    };

    EventAccessContainer() {
        for (auto &[index, element] : mData) {
            index = IdType::InvalidId;
        }
    }

    // TODO: concept convertable to this type
    template<typename ThisValueType>
    EventAccessContainer(std::initializer_list<ThisValueType> init) {
        auto copyTo = UnderlyingContainerType::outputIterator(mData);
        for (const auto &currentValue : init) {
            *copyTo++ = std::make_pair(IdType::generate(idGen), std::make_optional(currentValue));
        }
    }

    ~EventAccessContainer() = default;

    EventAccessContainer& operator=(EventAccessContainer &other) = delete;
    EventAccessContainer& operator=(EventAccessContainer &&other) = delete;

    template<typename Event>
    requires(std::invocable<Event &,
        IdValueType &, std::optional<T> &, EventContext &>)
    void dispatch(Event &event) noexcept;

    template<typename Event>
    requires(std::invocable<Event &,
        const IdValueType &, const std::optional<T> &, const EventContext &>)
    void dispatch(Event &event) const noexcept;

    private:

    ContainerType mData;
    IdType idGen;
    mutable std::shared_mutex instanceMutex;
};

// TODO: concept to check if the event needs write access
template<typename IdType, typename T, ContainerType CT, typename ... MoreArgs>
template<typename Event>
requires(std::invocable<Event &, 
    typename IdType::ValueType &, std::optional<T> &,
    typename EventAccessContainer<IdType, T, CT, MoreArgs ...>::EventContext &>)
void EventAccessContainer<IdType, T, CT, MoreArgs ...>::dispatch(Event &event) noexcept {
    std::unique_lock<std::shared_mutex> instanceGuard{instanceMutex};

    EventContext context{ idGen };
    for (auto [index, currentElement] : std::ranges::enumerate_view(mData)) {
        if (event(currentElement.first, currentElement.second, context)) {
            return;
        }
    }

    size_t pastIndex = mData.size();
    bool newElements = UnderlyingContainerType::expandContainer(mData, IdType::InvalidId);

    // Couldn't add more space -> nothing to do
    if (!newElements) {
        return;
    }

    // Access first free element, after last element
    auto result = event(mData[pastIndex].first, mData[pastIndex].second, context);

}

template<typename IdType, typename T, ContainerType CT, typename ... MoreArgs>
template<typename Event>
requires(std::invocable<Event &, 
    const typename IdType::ValueType &, 
    const std::optional<T> &,
    const typename EventAccessContainer<IdType, T, CT, MoreArgs ...>::EventContext &>)
void EventAccessContainer<IdType, T, CT, MoreArgs ...>::dispatch(Event &event) const noexcept {
    std::shared_lock<std::shared_mutex> instanceGuard{instanceMutex};

    EventContext context{ idGen };
    for (const auto &currentElement : mData) {
        if (event(currentElement.first, currentElement.second, context)) {
            return;
        }
    }
}

}