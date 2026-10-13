
#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <cstddef> // size_t
#include <cassert>
#include <utility>
#include <vector>
#include <tuple>
#include <limits>
#include <type_traits>

#include "pack.h" // Pack from prep
#include "registry.h"

using EntityID = size_t;

template <typename... ComponentTypes>
class EntityManager {
private:
    static constexpr size_t invalidComponentIndex = std::numeric_limits<size_t>::max();

    std::tuple<Registry<ComponentTypes>...> registries;

    using ComponentIndex = std::tuple<std::conditional_t<true, size_t, ComponentTypes>...>;

    std::vector<ComponentIndex> entityComponents;
    std::vector<bool> activeEntity;
    std::vector<EntityID> inactiveIDs;

public:
    EntityID createEntity() {
        EntityID id;

        if (!inactiveIDs.empty()) {
            id = inactiveIDs.back();
            inactiveIDs.pop_back();

            entityComponents[id] = ComponentIndex{((void)sizeof(ComponentTypes), invalidComponentIndex)...};

            activeEntity[id] = true;
        } else {
            id = entityComponents.size();

            entityComponents.emplace_back(ComponentIndex{((void)sizeof(ComponentTypes), invalidComponentIndex)...});

            activeEntity.push_back(true);
        }

        return id;
    }

    bool validEntity(EntityID id) const {
        return id < activeEntity.size() && activeEntity[id];
    }

    template <typename ComponentType>
    bool hasComponent(EntityID id) const {
        assert(validEntity(id));

        static_assert(Pack<ComponentTypes...>::template index<ComponentType> >= 0); // component type not supported by entitymanager

        static constexpr auto COMPONENT_TYPE_INDEX = Pack<ComponentTypes...>::template index<ComponentType>;

        return std::get<COMPONENT_TYPE_INDEX>(entityComponents[id]) != invalidComponentIndex;
    }

    template <typename ComponentType>
    ComponentType& getComponent(EntityID id) {
        assert(validEntity(id));
        assert(hasComponent<ComponentType>(id));

        static constexpr auto COMPONENT_TYPE_INDEX = Pack<ComponentTypes...>::template index<ComponentType>;

        size_t componentID = std::get<COMPONENT_TYPE_INDEX>(entityComponents[id]);

        return std::get<COMPONENT_TYPE_INDEX>(registries)[componentID];
    }

    template <typename ComponentType>
    void addComponent(EntityID id, ComponentType&& component) {
        assert(validEntity(id));

        using T = std::remove_reference_t<ComponentType>;

        static_assert(
            Pack<ComponentTypes...>::template index<T> >= 0);

        assert(!hasComponent<T>(id));

        static constexpr auto COMPONENT_TYPE_INDEX = Pack<ComponentTypes...>::template index<T>;

        size_t componentID = std::get<COMPONENT_TYPE_INDEX>(registries).store(std::forward<ComponentType>(component));

        std::get<COMPONENT_TYPE_INDEX>(entityComponents[id]) = componentID;
    }
};

#endif  // ENTITY_MANAGER_H
