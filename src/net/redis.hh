/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <functional>
#include <string>
#include <vector>

#include "net/requests.hh"

namespace gg {

struct RedisClientConfig
{
  std::string ip { "0.0.0.0" };
  uint16_t port { 6379 };

  std::string username {};
  std::string password {};

  size_t max_threads { 32 };
  size_t max_batch_size { 32 };
};

class Redis
{
private:
  RedisClientConfig config_;

public:
  Redis( const RedisClientConfig& config )
    : config_( config )
  {}

  void upload_files(
    const std::vector<storage::PutRequest>& upload_requests,
    const std::function<void( const storage::PutRequest& )>& success_callback
    = []( const storage::PutRequest& ) {} );

  void download_files(
    const std::vector<storage::GetRequest>& download_requests,
    const std::function<void( const storage::GetRequest& )>& success_callback
    = []( const storage::GetRequest& ) {} );
};

} // namespace gg
