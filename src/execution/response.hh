/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <exception>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <vector>

#include "util/optional.hh"

namespace gg {

class FetchDependenciesError : public std::exception
{};

class ExecutionError : public std::runtime_error
{
public:
  ExecutionError( const std::string& msg )
    : std::runtime_error( msg )
  {}
  ExecutionError()
    : std::runtime_error( "ExecutionError" )
  {}
};

class UploadOutputError : public std::exception
{};

enum class JobStatus
{
  Success = 0,
  RateLimit,
  InvocationFailure,
  OperationalFailure,
  FetchDependenciesFailure,
  ExecutionFailure,
  UploadOutputFailure,
  SocketFailure,
  ChildProcessFailure,
};

class ExecutionResponse
{
public:
  struct Output
  {
    std::string tag;
    std::string hash;
    off_t size;
    bool is_executable;
    std::string data;
  };

private:
  ExecutionResponse() {}

public:
  JobStatus status {};

  std::string thunk_hash {};
  std::vector<Output> outputs {};

  std::string stdout {};

  static ExecutionResponse parse_message( const std::string& message );
};

} // namespace gg
