/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "net/requests.hh"
#include "util/optional.hh"
#include "util/path.hh"

namespace gg {

typedef std::function<void( const storage::PutRequest& )> PutCallback;
typedef std::function<void( const storage::GetRequest& )> GetCallback;

class StorageBackend
{
protected:
  roost::path remote_index_path_ {};

public:
  virtual void put(
    const std::vector<storage::PutRequest>& requests,
    const PutCallback& success_callback = []( const storage::PutRequest& ) {} )
    = 0;

  virtual void get(
    const std::vector<storage::GetRequest>& requests,
    const GetCallback& success_callback = []( const storage::GetRequest& ) {} )
    = 0;

  bool is_available( const std::string& hash );
  void set_available( const std::string& hash );

  static std::unique_ptr<StorageBackend> create_backend(
    const std::string& uri );

  virtual ~StorageBackend() {}
};

} // namespace gg
