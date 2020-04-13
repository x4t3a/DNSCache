#pragma once

#include "core/types.hpp"

namespace core
{

template <typename Node>
class Ladder
{
public: // Types:
    class NodeTrait
    {
        Node* next_ladder_item{};
        Node* prev_ladder_item{};

        friend Ladder<Node>;
    };

    struct ToTop {};
    struct OneUp {};

    enum class PromotingStatus : std::uint8_t
    {
        SUCCESS,
        NON_PROMOTABLE, // Already at the top: cannot promote any further. Not an error.
        ERROR,
    };

public: // Constants:
    inline static constexpr ToTop TO_TOP{};
    inline static constexpr OneUp ONE_UP{};

private: // Fields:
    Capacity const capacity{};
    Node*          ladder_bottom{}; // start of the linked list
    Node*          ladder_top{}; // end of the linked list for fast promoting reallocated nodes

public: // RAII:
    Ladder(Node* storage, Capacity capacity) noexcept(false)
        : capacity{ capacity }
        , ladder_bottom{ storage }
        , ladder_top{ storage + capacity }
    {
        constexpr auto MINIMAL_VIABLE_SIZE{ 3 };
        if ((nullptr == this->ladder_bottom) or (MINIMAL_VIABLE_SIZE > this->capacity))
        { throw std::logic_error("BadArgs"); }

        auto second{ std::next(this->ladder_bottom) };
        this->ladder_bottom->next_ladder_item = second;

        auto const last_to_modify{ std::prev(this->ladder_top) };
        for (auto it_ptr{ second }; it_ptr < last_to_modify; ++it_ptr)
        {
            it_ptr->next_ladder_item = std::next(it_ptr);
            it_ptr->prev_ladder_item = std::prev(it_ptr);
        }
    }

public: // Methods:
    [[nodiscard]]
    auto maxSize() noexcept(true) -> Capacity
    { return this->capacity; }

    [[nodiscard]]
    auto getFreeNode() noexcept(false) -> Node*
    {
        if (nullptr != this->ladder_bottom)
        {
            auto free_node              = this->ladder_bottom;
            this->ladder_bottom         = free_node->next_ladder_item;
            free_node->next_ladder_item = nullptr;
            return free_node;
        }

        throw std::runtime_error{ "Bad ladder bottom!" };
    }

    [[nodiscard]]
    auto promote(Node* promotee, ToTop const&) noexcept(true) -> PromotingStatus
    {
        if ((nullptr == promotee) or (nullptr == this->ladder_top))
        { return PromotingStatus::ERROR; }

        if (promotee == this->ladder_top)
        { return PromotingStatus::NON_PROMOTABLE; }
        
        promotee->next_ladder_item = this->ladder_top->next_ladder_item;
        promotee->prev_ladder_item = this->ladder_top;

        this->ladder_top->next_ladder_item = promotee;
        this->ladder_top                   = promotee;

        return PromotingStatus::SUCCESS;
    }

    [[nodiscard]]
    auto promote(Node* promotee, OneUp const&) noexcept(true) -> PromotingStatus
    {
        if (nullptr == promotee)
        { return PromotingStatus::ERROR; }

        if (nullptr == promotee->next_ladder_item)
        {
            if (promotee != this->ladder_top)
            { this->ladder_top = promotee; }

            return PromotingStatus::NON_PROMOTABLE;
        }

        auto demotee{ promotee->next_ladder_item };

        promotee->next_ladder_item = demotee->next_ladder_item;
        demotee->next_ladder_item  = promotee;

        demotee->prev_ladder_item  = promotee->prev_ladder_item;
        promotee->prev_ladder_item = demotee;

        return PromotingStatus::SUCCESS;
    }

}; // Ladder

} // core
