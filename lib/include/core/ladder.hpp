#pragma once

#include "core/types.hpp"

#include <stdexcept>

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
    struct ToBottom {};

    enum class PromotingStatus : std::uint8_t
    {
        SUCCESS,
        NON_PROMOTABLE, // Already at the top: cannot promote any further. Not an error.
        ERROR,
    };

    enum class DemotingStatus : std::uint8_t
    {
        SUCCESS,
        NON_DEMOTABLE, // Already at the bottom
        ERROR
    };

public: // Constants:
    inline static constexpr ToTop TO_TOP{};
    inline static constexpr OneUp ONE_UP{};
    inline static constexpr core::Capacity MINIMAL_VIABLE_CAPACITY{ 3 };

private: // Fields:
    Capacity const capacity{};
    Node*          ladder_bottom{}; // start of the linked list
    Node*          ladder_top{}; // end of the linked list for fast promoting reallocated nodes

public: // RAII:
    Ladder(Node* storage, Capacity capacity) noexcept(false)
        : capacity{ capacity }
        , ladder_bottom{ storage }
        , ladder_top{ storage + (capacity - 1) }
    {
        if ((nullptr == this->ladder_bottom) or (MINIMAL_VIABLE_CAPACITY > this->capacity))
        { throw std::logic_error("BadArgs"); }

        this->ladder_bottom->next_ladder_item = this->ladder_bottom + 1;

        auto const last_to_modify{ this->ladder_top - 1 };
        for (auto it_ptr{ this->ladder_bottom->next_ladder_item }; it_ptr <= last_to_modify; ++it_ptr)
        {
            it_ptr->next_ladder_item = it_ptr + 1;
            it_ptr->prev_ladder_item = it_ptr - 1;
        }

        this->ladder_top->prev_ladder_item = this->ladder_top - 1;
        this->ladder_top->next_ladder_item = nullptr;
    }

public: // Methods:
    [[nodiscard]]
    auto maxSize() noexcept(true) -> Capacity
    { return this->capacity; }

    [[nodiscard]]
    auto releaseBottom() noexcept(false) -> Node*
    {
        if (nullptr != this->ladder_bottom)
        {
            auto free_node                        = this->ladder_bottom;
            this->ladder_bottom                   = free_node->next_ladder_item;

            if ((nullptr != this->ladder_bottom) and
                (nullptr != this->ladder_bottom->prev_ladder_item))
            { this->ladder_bottom->prev_ladder_item = nullptr; }

            free_node->next_ladder_item           = nullptr;
            free_node->prev_ladder_item           = nullptr;
            return free_node;
        }

        throw std::runtime_error{ "Bad ladder bottom!" };
    }

    auto demote(Node* demotee, ToBottom const&) noexcept(true) -> DemotingStatus
    {
        if (nullptr == demotee)
        return DemotingStatus::SUCCESS;
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
