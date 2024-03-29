/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <string>

#include "file_descriptor.hh"

namespace gg {

/* Unix domain socket */
class IPCSocket : public FileDescriptor
{
public:
  IPCSocket();

  void bind( const std::string& path );
  void connect( const std::string& path );

  void listen( const int backlog = 200 );
  FileDescriptor accept( void );
};

} // namespace gg
