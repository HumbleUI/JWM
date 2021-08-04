#pragma once

#include <vector>
#include <cstdint>
#include <map>
#include <string>

namespace jwm
{
    using ByteBuf = std::vector<uint8_t>;
    using DataTransfer = std::map<std::string, ByteBuf>;
} // namespace jwm
