#include <net/dns_cache_singleton.hpp>
#include <net/util.hpp>

#include <boost/ut.hpp>

#include <cstdint>
#include <numeric>
#include <vector>

auto generatePseudoDomain(net::IP const& ip) -> net::FQDN
{
    net::FQDN fqdn = "subd" + ip;
    std::size_t index = 0;
    while (true)
    {
         index = fqdn.find(".", index);
         if (index == std::string::npos)
         { break; }

         fqdn.replace(index, 1, ".subd");
         ++index;
    }

    return fqdn;
}

using TestCnt = std::vector<std::pair<net::FQDN, net::IP>>;

auto generateTestData(std::size_t test_cnt_size) noexcept(true) -> TestCnt
{
    TestCnt generated;
    generated.resize(test_cnt_size);

    std::vector<net::IPV4Raw> raw_ips;
    raw_ips.resize(test_cnt_size);

    std::iota(std::begin(raw_ips), std::end(raw_ips), 0x01'01'01'01);

    for (std::size_t i{ 0 }; i < test_cnt_size; ++i)
    {
        auto ip{ net::IPV4RawToStr(raw_ips[i]).value_or("0.0.0.0") };
        auto fqdn = net::FQDN{ generatePseudoDomain(ip) + ".test.domain" };
        generated[i] = { fqdn, ip };
    }

    return generated;
}

auto main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) -> int
{
    using namespace boost::ut::literals;
    using namespace boost::ut;

    using namespace core;
    using namespace net;

    "non_updating_and_non_overfilling_minimal_capacity"_test = []
    {
        auto capacity{ DNSCache::minViableCapacity() };
        decltype(capacity) i{ 0 };
        auto test_data{ generateTestData(capacity) };
        Size current_size{ 0 };

        try
        {
            DNSCache dns_cache{ capacity };

            for ( ; i < capacity; ++i)
            {
                expect(current_size == dns_cache.size()) << "Bad size after creation!";
                expect(test_data[i].first != "subd0.subd0.subd0.subd0.test.domain");

                dns_cache.update(test_data[i].first, test_data[i].second);
                ++current_size;

                expect(current_size == dns_cache.size()) << "Bad size!";

                auto result_ip{ dns_cache.resolve(test_data[i].first) };

                expect(not result_ip.empty()) << "Haven't found existing pair!";
                expect(test_data[i].second == result_ip) << "Got wrong value!";
            }
        }
        catch (std::exception const& excp)
        {
            expect(false) << "Got exception{" << i << "}: " << excp.what();
        }
    };

    "non_updating_overfilling_minimal_capacity"_test = []
    {
        auto capacity{ DNSCache::minViableCapacity() };
        decltype(capacity) i{ 0 };
        constexpr auto overfilling_ratio{ 3 };
        auto test_data_size{ overfilling_ratio * capacity };
        auto test_data{ generateTestData(test_data_size) };
        Size current_size{ 0 };

        try
        {
            DNSCache dns_cache{ capacity };

            for (i = 0 ; i < test_data_size; ++i)
            {
                expect(current_size == dns_cache.size()) << "Bad size after creation!";
                expect(test_data[i].first != "subd0.subd0.subd0.subd0.test.domain");

                dns_cache.update(test_data[i].first, test_data[i].second);
                ++current_size;

                expect((current_size % capacity) == dns_cache.size()) << "Bad size!";

                auto result_ip{ dns_cache.resolve(test_data[i].first) };

                expect(not result_ip.empty()) << "Haven't found existing pair!";
                expect(test_data[i].second == result_ip) << "Got wrong value!";
            }

            // Check non-existing
            "check_existent"_test = [&]
            {
                std::cout << ( test_data_size - capacity) << ' ' << capacity << std::endl;
                for (i = test_data_size - capacity; i < test_data_size; ++i)
                {
                    std::cout << "ex: " << test_data[i].first << std::endl;
                    auto result_ip{ dns_cache.resolve(test_data[i].first) };
                    expect(result_ip == test_data[i].first);
                }
            };

            // Check non-existing
            "check_non_existent"_test = [&]
            {
                for (i = 0; i < test_data_size - capacity; ++i)
                {
                    auto result_ip{ dns_cache.resolve(test_data[i].first) };
                    std::cout << "result_ip{" << result_ip << '}' << std::endl;
                    expect(result_ip == "");
                }
            };
        }
        catch (std::exception const& excp)
        {
            expect(false) << "Got exception{" << i << "}: " << excp.what();
        }
    };
}
