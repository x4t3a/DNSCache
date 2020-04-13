#include <iostream>
#include <net/dns_cache.hpp>

auto main([[maybe_unused]] int argc, [[maybe_unused]] char const* argv[]) -> int
{
    net::DNSCache::init(core::Capacity{ 3 });
    if (auto dns_cache_opt{ net::DNSCache::getInstance() })
    {
        auto& dns_cache{ dns_cache_opt->get() };

        net::FQDN google_fqdn{ "google.com" };
        net::IP   google_ip{ "1.2.3.4" };

        net::FQDN facebook_fqdn{ "facebook.com" };

        dns_cache.update(google_fqdn, google_ip);
        dns_cache.update(facebook_fqdn, net::IP{ "3.4.3.5" });

        dns_cache.update(net::FQDN{ "nn.ru" }, net::IP{ "4.4.4.4" });
        dns_cache.update(net::FQDN{ "mail.ru" }, net::IP{ "4.4.4.4" });

        {
            auto cached_google_ip = dns_cache.resolve(google_fqdn);
            std::cout << cached_google_ip << '\n';
        }

        {
            auto cached_facebook_ip = dns_cache.resolve(facebook_fqdn);
            std::cout << cached_facebook_ip << '\n';
        }
    }
}
