/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <chrono>
#include <ctime>
#include <string>
#include <vector>

namespace gg {

class TimeLog
{
private:
  std::chrono::milliseconds start_;
  std::chrono::milliseconds prev_;
  std::vector<std::pair<std::string, std::chrono::milliseconds>> points_ {};

public:
  TimeLog();
  void add_point( const std::string& title );

  std::string str() const;
};

} // namespace gg
