/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#pragma once

#include <string>

namespace gg {

class HTTPHeader
{
private:
  std::string key_, value_;

public:
  HTTPHeader( const std::string& buf );
  HTTPHeader( const std::string& key, const std::string& value );

  const std::string& key() const { return key_; }
  const std::string& value() const { return value_; }

  std::string str() const { return key_ + ": " + value_; }
};

} // namespace gg
