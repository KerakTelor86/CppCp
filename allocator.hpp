#ifndef CPPCP_ALLOCATOR
#define CPPCP_ALLOCATOR

#include <array>

#include "debug.hpp"
#include "types.hpp"

namespace CppCp {

template <typename Allocator, typename Node, typename... Args>
concept is_node_allocator = requires(
    Allocator obj, Node* node, const Args... args
) {
    { obj.alloc(args...) } -> std::same_as<Node*>;
    { obj.dealloc(node) } -> std::same_as<void>;
};

template <typename Node> class DynamicAllocator {
public:
    template <typename... T> Node* alloc(const T&... args) {
        return new Node(args...);
    }

    void dealloc(const Node* node) {
        delete node;
    }
};

template <typename Node, usize Len> class StaticAllocator {
public:
    StaticAllocator() : usable(Len) {
        for (usize i = 0; i < Len; ++i) {
            usable[i] = &buffer[i];
        }
    }

    template <typename... T> Node* alloc(const T&... args) {
        debug_assert(
            std::size(usable) > 0, "static allocator ran out of usable nodes"
        );
        const auto ptr = usable.back();
        usable.pop_back();
        *ptr = Node(args...);
        return ptr;
    }

    void dealloc(Node* node) {
        usable.push_back(node);
    }

private:
    std::vector<Node*> usable;
    std::array<Node, Len> buffer;
};

} // namespace CppCp

#endif
