/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <string>
#include <vector>

namespace gg {

class FileManifest
{
private:
  typedef std::string DummyDirectoryEntry;
  typedef std::pair<std::string, std::string> OutputTagEntry;

  std::vector<DummyDirectoryEntry> dummy_directories_ {};
  std::vector<OutputTagEntry> output_tags_ {};

public:
  FileManifest() {}

  void add_dummy_directory( const std::string& dummy_dir );
  void add_output_tag( const std::string& filename, const std::string& tag );

  std::string serialize() const;
};

} // namespace gg
