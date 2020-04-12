#pragma once

#include <functional>

namespace core
{

template <
    typename KeyType,
    typename ValueType,
    typename Node
>
class FlatLLRBMap
{
private: // Types:
    using BinaryFlag = std::uint8_t;

    struct Flags
    {
        BinaryFlag is_red : 1;

        Flags() noexcept(true)
            : is_red{BIN_FALSE}
        {}

        auto isRed() const noexcept(true) -> bool
        { return this->is_red; }

        auto setRed() noexcept(true) -> void
        { this->is_red = BIN_TRUE; }

        auto setBlack() noexcept(true) -> void
        { this->is_red = BIN_FALSE; }
    };

private: // Constants:
    inline static constexpr BinaryFlag BIN_FALSE{0b0};
    inline static constexpr BinaryFlag BIN_TRUE{0b1};

public: // Types:
    class NodeTrait
    {
        using key_type    = KeyType;
        using mapped_type = ValueType;

        Node* left{};
        Node* parent{};
        Node* right{};

        Flags flags{};

        friend FlatLLRBMap<KeyType, ValueType, Node>;

    public: // Fields:
        key_type    first;
        mapped_type second;

    }; // NodeTrait

public:
    enum class CmpResult : std::uint8_t
    { EQ, LT, GT };

    template <
        typename LHSType,
        typename RHSType,
        bool LHS2RHS = std::is_convertible_v<LHSType const&, RHSType const&>,
        bool RHS2LHS = std::is_convertible_v<RHSType const&, LHSType const&>,
        typename = std::enable_if_t<LHS2RHS or RHS2LHS>
    >
    inline static auto cmp(LHSType const& lhs, RHSType const& rhs) -> CmpResult
    {
        CmpResult cmp_result{ CmpResult::EQ };

        if constexpr (LHS2RHS)
        {
            cmp_result = (
                (static_cast<RHSType const&>(lhs) == rhs)
                    ? CmpResult::EQ
                    : ((static_cast<RHSType const&>(lhs) < rhs)
                        ? CmpResult::LT
                        : CmpResult::GT)
            );
        }
        else if constexpr (RHS2LHS)
        {
            cmp_result = (
                (lhs == static_cast<LHSType const&>(rhs))
                ? CmpResult::EQ
                : ((lhs < static_cast<LHSType const&>(rhs))
                    ? CmpResult::LT
                    : CmpResult::GT)
            );
        }

        return cmp_result;
    }

    auto createNode(KeyType const& key, ValueType const& value) noexcept(false) -> Node*
    {
        if (not this->allocate_cb)
        { throw std::bad_alloc{}; }

        if (auto new_node{ this->allocate_cb() };
            nullptr != new_node)
        {
            new_node->first  = key;
            new_node->second = value;
            
            if (this->create_cb and
                (CreateOrUpdateStatus::FATAL_ERROR == this->create_cb(new_node)))
            { throw std::runtime_error{"Fatal error in the create callback!"}; }

            return new_node;
        }

        throw std::bad_alloc{};
    }

    auto updateImpl(KeyType const& key, ValueType const& value, Node* node) noexcept(true) -> Node*
    {
        if (nullptr == node)
        {
            auto new_node{ this->createNode(key, value) };
            return new_node;
        }

        auto cmp_result = cmp(key, node->first);
        switch (cmp_result)
        {
            case CmpResult::EQ:
            {
                if (this->update_cb and
                    (CreateOrUpdateStatus::FATAL_ERROR == this->update_cb(node)))
                { throw std::runtime_error{"Fatal error in the update callback!"}; }

                node->second = value;
            }
            break;

            case CmpResult::LT:
            { node->left = updateImpl(key, value, node->left); }
            break;

            case CmpResult::GT:
            { node->right = updateImpl(key, value, node->right); }
            break;
        }

        return node;
    }

    void insertOrUpdate(KeyType const& key, ValueType const& value)
    {
        this->search_tree_root = this->updateImpl(key, value, this->search_tree_root);
    }

    ValueType& at(KeyType const& key) noexcept(false)
    {
        auto node_ptr_it{ this->search_tree_root };
        while (nullptr != node_ptr_it)
        {
            switch (cmp(key, *node_ptr_it))
            {
                case CmpResult::LT:
                { node_ptr_it = node_ptr_it->left; }
                break;

                case CmpResult::EQ:
                {
                    if (this->use_cb and
                        (CreateOrUpdateStatus::FATAL_ERROR == this->use_cb(node_ptr_it)))
                    { throw std::runtime_error{"Use callback fatal error!"}; }

                    return node_ptr_it->second;
                }
                break;

                case CmpResult::GT:
                { node_ptr_it = node_ptr_it->right; }
                break;
            }
        }

        throw std::out_of_range{""};
    }

public:
    enum class CreateOrUpdateStatus : std::uint8_t
    {
        SUCCESS,    // Welp, all's good!
        ERROR,      // It's possible to live on w/ this kind of error.
        FATAL_ERROR // Complite FUBAR!

    }; // CreateOrUpdateStatus

public:
    using AllocateCallback = std::function<Node* ()>;
    using AccessCallback   = std::function<CreateOrUpdateStatus (Node*)>;

    auto setAllocateCallback(AllocateCallback allocate_cb) noexcept(true) -> void
    { this->allocate_cb = std::move(allocate_cb); }

    auto setCreateCallback(AccessCallback create_cb) noexcept(true) -> void
    { this->create_cb = std::move(create_cb); }

    auto setUpdateCallback(AccessCallback update_cb) noexcept(true) -> void
    { this->update_cb = std::move(update_cb); }

    auto setUseCallback(AccessCallback use_cb) noexcept(true) -> void
    { this->use_cb = std::move(use_cb); }

private: // Fields:
    Node*            search_tree_root{};
    AllocateCallback allocate_cb{};
    AccessCallback   create_cb{};
    AccessCallback   update_cb{};
    AccessCallback   use_cb{};

}; // FlatLLRBMap

} // core
