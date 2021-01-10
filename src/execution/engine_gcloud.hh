/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <chrono>
#include <unordered_map>

#include "engine.hh"
#include "net/http_request.hh"
#include "thunk/thunk.hh"
#include "util/uri.hh"

namespace gg {

/* Engine for Google Cloud Functions */
class GCFExecutionEngine : public ExecutionEngine
{
private:
  ParsedURI parsed_url_;
  Address address_;
  SSLContext ssl_context_ {};

  size_t running_jobs_ { 0 };
  std::map<uint64_t, std::chrono::steady_clock::time_point> start_times_ {};

  HTTPRequest generate_request( const gg::thunk::Thunk& thunk );

  static float compute_cost( const std::chrono::steady_clock::time_point& begin,
                             const std::chrono::steady_clock::time_point& end
                             = std::chrono::steady_clock::now() );

public:
  GCFExecutionEngine( const size_t max_jobs, const std::string& function_url )
    : ExecutionEngine( max_jobs )
    , parsed_url_( function_url )
    , address_( parsed_url_.host, parsed_url_.protocol )
  {}

  void force_thunk( const gg::thunk::Thunk& thunk,
                    ExecutionLoop& exec_loop ) override;

  bool is_remote() const { return true; }
  bool can_execute( const gg::thunk::Thunk& thunk ) const override;
  std::string label() const override { return "gcloud"; }
  size_t job_count() const override;
};

} // namespace gg
