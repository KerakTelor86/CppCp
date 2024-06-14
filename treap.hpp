#ifndef CPPCP_TREAP
#define CPPCP_TREAP

#include <concepts>
#include <functional>
#include <type_traits>
#include <utility>

#include "allocator.hpp"
#include "debug.hpp"
#include "random.hpp"

namespace CppCp {

namespace Treap {

#ifndef CPPCP_EMPTY
#define CPPCP_EMPTY
struct Empty {};
#endif

template <typename Val, bool is_reversible> struct Node {
    Val val, cum_val;
    Node* left;
    Node* right;
    i32 priority;
    usize size;

    [[no_unique_address]] std::conditional_t<is_reversible, bool, Empty> flip;

    Node() {}

    Node(const Val& _val)
        : val(_val),
          cum_val(_val),
          left(nullptr),
          right(nullptr),
          priority(rng32()),
          size(1),
          flip() {}
};

} // namespace Treap

namespace {
template <
    typename Val,
    typename Op = std::plus<>,
    bool is_reversible = false,
    typename Allocator = DynamicAllocator<Treap::Node<Val, is_reversible>>>
    requires std::is_invocable_r_v<Val, Op, Val, Val>
             && std::assignable_from<Val&, Val>
             && is_node_allocator<
                 Allocator,
                 Treap::Node<Val, is_reversible>,
                 Val>
class ImplicitTreap {
public:
    using Node = Treap::Node<Val, is_reversible>;

    ImplicitTreap() : root(nullptr) {}

    template <typename T>
        requires IndexableContainer<T>
                 && std::assignable_from<Val&, decltype(T()[0])>
    ImplicitTreap(const T& source) : root(nullptr) {
        for (const auto& i : source) {
            push_back(i);
        }
    }

    ~ImplicitTreap() {
        if (root != nullptr) {
            erase(0, size() - 1);
        }
    }

    ImplicitTreap(const ImplicitTreap& other) : root(nullptr) {
        walk_inorder(other.root, [&](const Node* node) {
            root = merge(root, allocator.alloc(node->val));
        });
    }

    ImplicitTreap& operator=(const ImplicitTreap& other) {
        if (this == &other) {
            return *this;
        }
        if (root != nullptr) {
            erase(0, size() - 1);
        }
        this(other);
        return *this;
    }

    const Val& operator[](const usize pos) const {
        debug_assert(
            pos < size(), "trying to access an element past end of structure"
        );
        return get(root, pos)->val;
    }

    void set(const usize pos, const Val& val) {
        debug_assert(
            pos < size(), "trying to access an element past end of structure"
        );

        const auto [left, rest] = split(root, pos);
        const auto [target, right] = split(rest, 1);

        target->val = val;
        update(target);

        root = merge(merge(left, target), right);
    }

    Val query(const usize l_pos, const usize r_pos) const {
        debug_assert(l_pos <= r_pos, "trying to query a segment of size < 0");
        debug_assert(
            r_pos < size(), "r_pos for query is past end of structure"
        );

        const auto [left, rest] = split(root, l_pos);
        const auto [target, right] = split(rest, r_pos - l_pos + 1);

        auto ret = target->cum_val;

        root = merge(merge(left, target), right);
        return ret;
    }

    void update(const usize pos, const Val& val) {
        debug_assert(
            pos < size(), "trying to access an element past end of structure"
        );

        const auto [left, rest] = split(root, pos);
        const auto [target, right] = split(rest, 1);

        target->val = op(target->val, val);
        update(target);

        root = merge(merge(left, target), right);
    }

    void for_each(const auto& func) const {
        walk_inorder(root, [&](const Node* node) { func(node->val); });
    }

    auto map(const auto& func) const {
        std::vector<decltype(func(Val()))> ret;
        ret.reserve(size());
        for_each([&](const Val& val) { ret.push_back(func(val)); });
        return ret;
    }

    void push_back(const Val& val) {
        insert(size(), val);
    }

    void insert(const usize pos, const Val& val) {
        debug_assert(pos <= size(), "pos for insert is past end of structure");
        const auto [left, right] = split(root, pos);
        const auto target = allocator.alloc(val);
        root = merge(merge(left, target), right);
    }

    void erase(const usize pos) {
        erase(pos, pos + 1);
    }

    void erase(const usize l_pos, const usize r_pos) {
        debug_assert(l_pos <= r_pos, "trying to erase a segment of size < 0");
        debug_assert(
            r_pos < size(), "r_pos for erase is past end of structure"
        );

        const auto [left, rest] = split(root, l_pos);
        const auto [target, right] = split(rest, r_pos - l_pos + 1);

        walk_postorder(target, [&](Node* node) { allocator.dealloc(node); });

        root = merge(left, right);
    }

    void reverse(const usize l_pos, const usize r_pos)
        requires(is_reversible)
    {
        debug_assert(l_pos <= r_pos, "trying to reverse a segment of size < 0");
        debug_assert(
            r_pos < size(), "r_pos for reverse is past end of structure"
        );

        const auto [left, rest] = split(root, l_pos);
        const auto [target, right] = split(rest, r_pos - l_pos + 1);

        if (target != nullptr) {
            target->flip ^= 1;
        }

        root = merge(merge(left, target), right);
    }

    std::pair<ImplicitTreap, ImplicitTreap> split(const usize left_size) {
        debug_assert(
            left_size <= size(), "size too small to perform requested split"
        );
        const auto [left, right] = split(root, left_size);
        root = nullptr;
        return {ImplicitTreap(left), ImplicitTreap(right)};
    }

    void join(const ImplicitTreap& other) {
        root = merge(root, other.root);
    }

    usize size() const {
        return safe_get_size(root);
    }

private:
    mutable Node* root;

    static Allocator allocator;

    static constexpr auto op = Op();

    ImplicitTreap(Node* node) : root(node) {}

    void propagate(Node* node) const {
        if (node == nullptr) {
            return;
        }
        if constexpr (is_reversible) {
            if (node->flip) {
                std::swap(node->left, node->right);
                for (const auto& child : {node->left, node->right}) {
                    if (child != nullptr) {
                        child->flip ^= node->flip;
                    }
                }
                node->flip = false;
            }
        }
    }

    void update(Node* node) const {
        node->cum_val = node->val;
        node->size = 1;
        for (const auto& child : {node->left, node->right}) {
            if (child != nullptr) {
                propagate(child);
                node->cum_val = op(node->cum_val, child->cum_val);
                node->size += child->size;
            }
        }
    }

    static usize safe_get_size(const Node* node) {
        if (node == nullptr) {
            return 0;
        }
        return node->size;
    }

    std::pair<Node*, Node*> split(Node* node, const usize size) const {
        debug_assert(
            safe_get_size(node) >= size, "split size must be <= node size"
        );
        if (node == nullptr) {
            return {nullptr, nullptr};
        }
        propagate(node);
        const usize left_size = safe_get_size(node->left);
        if (size <= left_size) {
            const auto [left, mid] = split(node->left, size);
            node->left = mid;
            update(node);
            return {left, node};
        } else {
            const auto [mid, right] = split(node->right, size - left_size - 1);
            node->right = mid;
            update(node);
            return {node, right};
        }
    }

    Node* merge(Node* left, Node* right) const {
        if (left == nullptr) {
            return right;
        }
        propagate(left);
        if (right == nullptr) {
            return left;
        }
        propagate(right);
        if (left->priority < right->priority) {
            left->right = merge(left->right, right);
            update(left);
            return left;
        } else {
            right->left = merge(left, right->left);
            update(right);
            return right;
        }
    }

    Node* get(Node* node, const usize offset) const {
        propagate(node);
        const usize cur_idx = safe_get_size(node->left);
        if (offset == cur_idx) {
            return node;
        } else if (offset < cur_idx) {
            return get(node->left, offset);
        } else {
            return get(node->right, offset - cur_idx - 1);
        }
    }

    void walk_inorder(Node* node, const auto& func) const {
        if (node == nullptr) {
            return;
        }
        propagate(node);
        walk_inorder(node->left, func);
        func(node);
        walk_inorder(node->right, func);
    }

    void walk_preorder(Node* node, const auto& func) const {
        if (node == nullptr) {
            return;
        }
        propagate(node);
        func(node);
        walk_preorder(node->left, func);
        walk_preorder(node->right, func);
    }

    void walk_postorder(Node* node, const auto& func) const {
        if (node == nullptr) {
            return;
        }
        propagate(node);
        walk_postorder(node->left, func);
        walk_postorder(node->right, func);
        func(node);
    }
};

template <typename Val, typename Op, bool is_reversible, typename Allocator>
    requires std::is_invocable_r_v<Val, Op, Val, Val>
                 && std::assignable_from<Val&, Val>
                 && is_node_allocator<
                     Allocator,
                     Treap::Node<Val, is_reversible>,
                     Val>
Allocator
    ImplicitTreap<Val, Op, is_reversible, Allocator>::allocator = Allocator();
} // namespace

} // namespace CppCp

#endif
