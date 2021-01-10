/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <string>
#include <sys/types.h>

#include "util/file_descriptor.hh"
#include "util/optional.hh"

namespace gg {

class ThunkPlaceholder
{
public:
  enum class Type
  {
    LinkerScript,
    ShellScript
  };

private:
  std::string content_hash_;

  void write( const std::string& filename, const Type type ) const;

public:
  ThunkPlaceholder( const std::string& content_hash );

  const std::string& content_hash() const { return content_hash_; }

  void write( const std::string& filename ) const;

  static Optional<ThunkPlaceholder> read( const std::string& filename );
  static bool is_placeholder( FileDescriptor&& fd );

  std::string str( const Type type = Type::ShellScript ) const;
};

} // namespace gg
