#pragma once

#include "core/types.hpp"
#include "net/types.hpp"

#include <memory>
#include <mutex>
#include <string>

namespace net
{

class DNSCache
{
private:
    class DNSCacheImpl;

private:
    std::mutex                    mutex;
    std::unique_ptr<DNSCacheImpl> impl;

public:

    explicit DNSCache(core::Capacity capacity = 0);

    ~DNSCache() noexcept(true); // = default

    static auto minViableCapacity() noexcept(true) -> core::Capacity; // unfortunately can't be constexpr
    auto size() const noexcept(true) -> core::Size;
    auto maxSize() noexcept(true) -> core::Capacity;

    DNSCache& operator = (DNSCache const&) = delete;
    DNSCache& operator = (DNSCache&&)      = delete;
    DNSCache(DNSCache const&)              = delete;
    DNSCache(DNSCache&&)                   = delete;

    auto update(FQDN const& fqdn, IP const& ip) noexcept(false) -> void;

    [[nodiscard]]
    auto resolve(FQDN const& fqdn) noexcept(true) -> IP;
    
}; // DNSCache

} // net
