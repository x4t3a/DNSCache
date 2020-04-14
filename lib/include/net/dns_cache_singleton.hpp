#pragma once

#include "core/singleton.hpp"
#include "net/dns_cache.hpp"

namespace net
{

using DNSCacheSingleton = core::Singleton<net::DNSCache>;

} // net
