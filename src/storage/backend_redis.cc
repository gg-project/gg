/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "backend_redis.hh"
#include <iostream>

using namespace std;
using namespace gg;
using namespace storage;

void RedisStorageBackend::put( const std::vector<PutRequest>& requests,
                               const PutCallback& success_callback )
{
  if ( verbose ) {
    cerr << "Redis puts:" << endl;
    for (const auto& r : requests) {
      cerr << " * " << r.object_key << endl;
    }
  }
  client_.upload_files( requests, success_callback );
}

void RedisStorageBackend::get( const std::vector<GetRequest>& requests,
                               const GetCallback& success_callback )
{
  if ( verbose ) {
    cerr << "Redis gets:" << endl;
    for (const auto& r : requests) {
      cerr << " * " << r.object_key << endl;
    }
  }
  client_.download_files( requests, success_callback );
}
