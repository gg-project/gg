/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <initializer_list>
#include <sys/signalfd.h>

#include "file_descriptor.hh"

namespace gg {

/* wrapper class for Unix signal masks and signal file descriptor */

class SignalMask
{
private:
  sigset_t mask_;

public:
  SignalMask( const std::initializer_list<int> signals );
  const sigset_t& mask( void ) const { return mask_; }
  void set_as_mask( void ) const;

  static SignalMask current_mask( void );
  bool operator==( const SignalMask& other ) const;
};

class SignalFD
{
private:
  FileDescriptor fd_;

public:
  SignalFD( const SignalMask& signals );

  FileDescriptor& fd( void ) { return fd_; }
  signalfd_siginfo read_signal( void ); /* read one signal */
};

} // namespace gg
