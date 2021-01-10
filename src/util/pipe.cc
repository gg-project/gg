/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <unistd.h>

#include "exception.hh"
#include "pipe.hh"

using namespace std;
using namespace gg;

pair<FileDescriptor, FileDescriptor> gg::make_pipe()
{
  int pipe_fds[2];
  CheckSystemCall( "pipe", pipe( pipe_fds ) );
  return { pipe_fds[0], pipe_fds[1] };
}
