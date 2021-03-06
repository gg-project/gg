/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include "backend.hh"
#include "net/gcloud.hh"
#include "net/s3.hh"

namespace gg {

class GoogleStorageBackend : public StorageBackend
{
private:
  S3Client client_;
  std::string bucket_;

public:
  GoogleStorageBackend( const GoogleStorageCredentials& credentials,
                        const std::string& bucket );

  void put(
    const std::vector<storage::PutRequest>& requests,
    const PutCallback& success_callback
    = []( const storage::PutRequest& ) {} ) override;

  void get(
    const std::vector<storage::GetRequest>& requests,
    const GetCallback& success_callback
    = []( const storage::GetRequest& ) {} ) override;
};

} // namespace gg
