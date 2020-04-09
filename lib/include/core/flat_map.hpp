#pragma once

#include "flat_llrb_map.hpp"

namespace core
{

template <typename KeyType, typename ValueType, typename NodeType>
using FlatMap = FlatLLRBMap<KeyType, ValueType, NodeType>;

} // core
