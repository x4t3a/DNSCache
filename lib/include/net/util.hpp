#pragma once

#include "net/types.hpp"

#include <optional>
#include <string>

namespace net
{

using IPV4RawResult = std::optional<IPV4Raw>;

auto strToIPV4Raw(std::string const& str_ip) noexcept(true) -> IPV4RawResult;

using IPV4StrResult = std::optional<IP>;

auto IPV4RawToStr(IPV4Raw raw_ip) noexcept(true) -> IPV4StrResult;

} // net
