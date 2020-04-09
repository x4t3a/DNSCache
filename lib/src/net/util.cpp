#include "net/util.hpp"

#include <array>

extern "C"
{
#include <arpa/inet.h>

} // extern "C"

namespace net
{

auto strToIPV4Raw(std::string const& str_ip) noexcept(true) -> IPV4RawResult
{
    constexpr auto INET_PTON_SUCCESS{ 1 };

    IPV4Raw raw_ip{0};
    auto ip_convert_status{
        ::inet_pton(AF_INET, str_ip.c_str(), &raw_ip)
    };

    if (INET_PTON_SUCCESS == ip_convert_status)
    { return IPV4RawResult{raw_ip}; }
    
    return std::nullopt;
}

auto IPV4RawToStr(IPV4Raw raw_ip) noexcept(true) -> IPV4StrResult
{
    std::array<char, INET_ADDRSTRLEN> ip_buffer{};

    if (::inet_ntop(AF_INET, &raw_ip, ip_buffer.data(), INET_ADDRSTRLEN))
    { return IPV4StrResult{ip_buffer.data()}; }
    
    return std::nullopt;
}

} // net
