/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <memory>

#include "execution/connection.hh"
#include "execution/meow/message.hh"
#include "thunk/thunk.hh"

namespace gg {
namespace meow {

std::string handle_put_message( const Message& message );
Message create_put_message( const std::string& hash );
Message create_execute_message( const gg::thunk::Thunk& thunk );

} // namespace meow
} // namespace gg
