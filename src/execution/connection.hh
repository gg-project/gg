/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <iostream>
#include <queue>
#include <string>

#include "net/nb_secure_socket.hh"
#include "net/socket.hh"

namespace gg {

class ExecutionLoop;

template<class SocketType>
class Connection
{
  friend class ExecutionLoop;

private:
  SocketType socket_ {};
  std::string write_buffer_ {};

public:
  Connection() {}

  Connection( SocketType&& sock )
    : socket_( std::move( sock ) )
  {}

  Connection( const Connection& ) = delete;

  ~Connection()
  {
    if ( write_buffer_.size() ) {
      /* std::cerr << "Connection destroyed with data left in write buffer" <<
       * std::endl; */
    }
  }

  void enqueue_write( const std::string& str ) { write_buffer_.append( str ); }
  const SocketType& socket() const { return socket_; }
};

using TCPConnection = Connection<TCPSocket>;
using SSLConnection = Connection<NBSecureSocket>;

} // namespace gg
