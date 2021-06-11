/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "util.hh"

#include <cstdlib>
#include <glob.h>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

namespace gg {

string safe_getenv( const string& key )
{
  const char* const value = getenv( key.c_str() );
  if ( not value ) {
    throw runtime_error( "missing environment variable: " + key );
  }
  return value;
}

string safe_getenv_or( const string& key, const string& def_val )
{
  const char* const value = getenv( key.c_str() );
  if ( not value ) {
    return def_val;
  }
  return value;
}

string format_bytes( size_t bytes )
{
  const char* sizes[] = { "B", "KiB", "MiB", "GiB", "TiB" };
  double val = bytes;

  size_t i;
  for ( i = 0; i < 4 and bytes >= 1024; i++, bytes /= 1024 ) {
    val = bytes / 1024.0;
  }

  ostringstream oss;
  oss << fixed << setprecision( 1 ) << val << " " << sizes[i];
  return oss.str();
}

std::vector<std::string> cxx_glob( const std::string& glob_ )
{
      glob_t globbuf;
    int err = glob(glob_.c_str(), 0, NULL, &globbuf);
    std::vector<std::string> output;
    if(err == 0)
    {
        for (size_t i = 0; i < globbuf.gl_pathc; i++)
        {
            output.emplace_back(globbuf.gl_pathv[i]);
        }

        globfree(&globbuf);
    }

    return output;
}

} // namespace gg
