/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <cstddef>

namespace gg {

constexpr std::size_t operator"" _KiB( unsigned long long v )
{
  return 1024u * v;
}

constexpr std::size_t operator"" _MiB( unsigned long long v )
{
  return 1024_KiB * v;
}

} // namespace gg
