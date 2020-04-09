#pragma once

#include "core/singleton.hpp"
#include "core/types.hpp"
#include "net/types.hpp"

#include <memory>
#include <string>

namespace net
{

class DNSCache : public ::util::Singleton<DNSCache>
{
private:
    class DNSCacheImpl;
    std::unique_ptr<DNSCacheImpl> impl; //{nullptr};

protected:
    using SingletonBase = typename ::util::Singleton<DNSCache>;
    friend SingletonBase;

    explicit DNSCache(core::Capacity capacity = 0);

public:

    auto maxSize() noexcept(true) -> core::Capacity;

    DNSCache& operator = (DNSCache const&) = delete;
    DNSCache& operator = (DNSCache&&)      = delete;
    DNSCache(DNSCache const&)              = delete;
    DNSCache(DNSCache&&)                   = delete;

    ~DNSCache() noexcept(true); // = default

    auto update(FQDN const& fqdn, IP const& ip) noexcept(true) -> void;

    [[nodiscard]]
    auto resolve(FQDN const& fqdn) noexcept(true) -> IP;
    
}; // DNSCache

} // net
