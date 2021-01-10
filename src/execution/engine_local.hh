/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <thread>

#include "engine.hh"

namespace gg {

class LocalExecutionEngine : public ExecutionEngine
{
private:
  bool mixed_ { false };
  size_t running_jobs_ { 0 };

public:
  LocalExecutionEngine( const bool mixed = false,
                        const size_t max_jobs
                        = std::thread::hardware_concurrency() )
    : ExecutionEngine( max_jobs )
    , mixed_( mixed )
  {}

  void force_thunk( const gg::thunk::Thunk& thunk,
                    ExecutionLoop& exec_loop ) override;
  size_t job_count() const override;

  bool is_remote() const override { return mixed_; }
  std::string label() const override { return "local"; }
  bool can_execute( const gg::thunk::Thunk& ) const override { return true; }
};

} // namespace gg
