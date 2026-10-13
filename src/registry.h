#ifndef REGISTRY_H
#define REGISTRY_H

#include <cassert>
#include <cstddef>
#include <utility>
#include <vector>

template <typename T>
class Registry {
private:
    size_t mSize = 0;
    std::vector<size_t> inactiveIDs;
    std::vector<bool> active;
    std::vector<T> mComponents;


public:
    size_t store(const T& component) {
        if (!inactiveIDs.empty()) {
            size_t id = inactiveIDs.back();
            inactiveIDs.pop_back();

            mComponents[id] = component;
            active[id] = true;
            ++mSize;
            return id;
        }
        mComponents.push_back(component);
        active.push_back(true);
        ++mSize;
        return mComponents.size() - 1;

    }
    
    size_t store(T&& component) {
        if (!inactiveIDs.empty()) {
            size_t id = inactiveIDs.back();
            inactiveIDs.pop_back();
            
            mComponents[id] = std::move(component);
            active[id] = true;
            ++mSize;
            return id;
        }
        mComponents.push_back(std::move(component));
        active.push_back(true);
        ++mSize;
        return mComponents.size() - 1;
    }
    
    void remove(size_t id) {
        assert(valid(id));
        mComponents[id] = T{};
        active[id] = false;
        inactiveIDs.push_back(id);
        --mSize;
    }
    T& operator[](size_t id) {
        assert(valid(id));
        return mComponents[id];
    }
    const T& operator[](size_t id) const {
        assert(valid(id));
        return mComponents[id];

    }
    // returns whether a slot both exist + active
    bool valid(size_t id) const {
        return id <active.size() && active[id];

    }
    // returns num of stored active objs
    size_t size() const {
        return mSize;
    }
    // personal implementation
    void reserve(size_t capacity) {
        mComponents.reserve(capacity);
        active.reserve(capacity);
        inactiveIDs.reserve(capacity);
    }

};

#endif  // REGISTRY_H
