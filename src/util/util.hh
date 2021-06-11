/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <string>
#include <vector>

namespace gg {

std::string safe_getenv( const std::string& key );
std::string safe_getenv_or( const std::string& key,
                            const std::string& def_val );
std::string format_bytes( size_t bytes );
std::vector<std::string> cxx_glob( const std::string& glob );


template<typename E>
constexpr auto to_underlying( E e ) noexcept
{
  return static_cast<std::underlying_type_t<E>>( e );
}

} // namespace gg
