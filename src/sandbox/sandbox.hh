/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <functional>
#include <string>
#include <sys/stat.h>
#include <unordered_map>
#include <unordered_set>

#include "trace/tracer.hh"
#include "util/path.hh"

namespace gg {

struct Permissions
{
  bool read, write, execute;
};

class SandboxedProcess
{
private:
  Tracer tracer_;
  roost::Directory working_directory_;

  /* map from inode num to access modes (O_RDONLY, O_WRONLY, O_RDRW) */
  std::map<std::pair<dev_t, ino_t>, Permissions> allowed_inodes_ {};

  /* map from pathname to access modes (O_RDONLY, O_WRONLY, O_RDRW) */
  std::unordered_map<std::string, Permissions> allowed_paths_ {};

  std::unordered_set<std::string> allow_candidates_ {};

  void allow_file( const std::string& pathname, const Permissions permissions );
  Optional<Permissions> get_permissions( const std::string& pathname );

  bool file_syscall_entry( const SystemCallInvocation& syscall );

  bool open_entry( const SystemCallInvocation& syscall );
  bool open_exit( const SystemCallInvocation& syscall );
  bool rename_entry( const SystemCallInvocation& syscall );
  bool execve_entry( const SystemCallInvocation& syscall );

  void syscall_entry( TracedThreadInfo& tcb );
  void syscall_exit( const TracedThreadInfo& tcb );

public:
  SandboxedProcess(
    const std::string& name,
    const std::string& working_directory,
    const std::unordered_map<std::string, Permissions>& allowed_files,
    std::function<int()>&& child_procedure,
    std::function<void()>&& preparation_procedure = []() {} );

  /* throws an exception if sandbox violation happens. */
  void execute();
};

class SandboxViolation : public std::runtime_error
{
public:
  SandboxViolation( const std::string& s_error, const std::string& details )
    : runtime_error( s_error + ": " + details )
  {}
};

} // namespace gg
