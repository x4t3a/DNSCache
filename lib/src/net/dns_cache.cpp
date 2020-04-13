#include "core/flat_map.hpp"
#include "core/ladder.hpp"
#include "core/types.hpp"
#include "net/dns_cache.hpp"
#include "net/util.hpp"

#include <array>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <utility>

namespace net
{

///
/// \brief The DNSCache::DNSCacheImpl class
///
class DNSCache::DNSCacheImpl
{
public:
    using NodeKeyType   = FQDN;
    using NodeValueType = IPV4Raw;

    ///
    /// \brief The Node struct
    ///
    struct Node
        : public core::Ladder<Node>::NodeTrait
        , public core::FlatMap<NodeKeyType, NodeValueType, Node>::NodeTrait
    {
        using NodeKeyReference = NodeKeyType const&;

        ///
        /// \brief operator NodeKeyReference is a helper cast operator.
        /// \details It's defined, so we don't have to define tons of comparison operators.
        /// Just use the existing ones.
        /// \return Const reference to the key held by the node.
        ///
        operator NodeKeyReference () const noexcept(true)
        { return this->first; }

    }; // Node

private:
    using DNSLadder     = core::Ladder<Node>;
    using DNSDictionary = core::FlatMap<NodeKeyType, NodeValueType, Node>;

private:
    std::unique_ptr<Node[]> storage{};
    DNSLadder               ladder;
    DNSDictionary           dictionary;

public:
    DNSCacheImpl(core::Capacity capacity) noexcept(false)
        : storage{ std::make_unique<Node[]>(capacity) }
        , ladder{ storage.get(), capacity }
        , dictionary{}
    {
        dictionary.setAllocateCallback(
            [this] () -> Node*
            { return this->ladder.getFreeNode(); }
        );

        dictionary.setCreateCallback(
            [this] (Node* created_node) -> DNSDictionary::CreateOrUpdateStatus
            {
                if (nullptr == created_node)
                { return DNSDictionary::CreateOrUpdateStatus::FATAL_ERROR; }

                auto promoting_status{ this->ladder.promote(created_node, DNSLadder::TO_TOP) };
                if (DNSLadder::PromotingStatus::ERROR == promoting_status)
                { return DNSDictionary::CreateOrUpdateStatus::FATAL_ERROR; }

                return DNSDictionary::CreateOrUpdateStatus::SUCCESS;
            } // lambda
        );

        auto use_or_update_cb{
            [this] (Node* updated_node) -> DNSDictionary::CreateOrUpdateStatus
            {
                if (nullptr == updated_node)
                { return DNSDictionary::CreateOrUpdateStatus::FATAL_ERROR; }

                auto promoting_status{ this->ladder.promote(updated_node, DNSLadder::ONE_UP) };
                if (DNSLadder::PromotingStatus::ERROR == promoting_status)
                { return DNSDictionary::CreateOrUpdateStatus::FATAL_ERROR; }

                return DNSDictionary::CreateOrUpdateStatus::SUCCESS;
            } // lambda
        };

        dictionary.setUpdateCallback(use_or_update_cb);
        dictionary.setUseCallback(use_or_update_cb);
    }

    [[nodiscard]]
    auto maxSize() noexcept(true) -> core::Capacity
    { return this->ladder.maxSize(); }

public:
    auto update(FQDN const& fqdn, IP const& ip) noexcept(false) -> void;

    [[nodiscard]]
    auto resolve(FQDN const& fqdn) noexcept(true) -> IP;
    
    ~DNSCacheImpl() noexcept(true) = default;

}; // DNSCache::DNSCacheImpl

auto DNSCache::DNSCacheImpl::update(
    FQDN const&  fqdn,
    IP const& ip
) noexcept(false) -> void
{
    auto raw_ip = strToIPV4Raw(ip).value_or(0);
    this->dictionary.insertOrUpdate(fqdn, raw_ip);
}

[[nodiscard]]
auto DNSCache::DNSCacheImpl::resolve(FQDN const& fqdn) noexcept(true) -> IP
{
    auto raw_ip_opt{ IPV4RawToStr(this->dictionary.at(fqdn)) };
    return raw_ip_opt.value_or(IP{});
}

DNSCache::DNSCache(core::Capacity capacity)
    : impl{std::make_unique<DNSCacheImpl>(capacity)}
{}

auto DNSCache::update(FQDN const& fqdn, IP const& ip) noexcept(false) -> void
{
    if (nullptr != impl)
    {
        std::scoped_lock lck{this->mutex};
        if (nullptr != impl)
        { impl->update(fqdn, ip); }
    }
}

auto DNSCache::resolve(FQDN const& fqdn) noexcept(true) -> IP
{
    if (nullptr != impl)
    {
        std::scoped_lock lck{this->mutex};
        if (nullptr != impl)
        { return impl->resolve(fqdn); }
    }
    
    return {};
}

DNSCache::~DNSCache() noexcept(true) = default;

auto DNSCache::maxSize() noexcept(true) -> core::Capacity
{
    return (nullptr != impl) ? impl->maxSize() : 0;
}

} // net
