/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "base16.hh"

#include <string>
#include <vector>

using namespace std;
using namespace gg;

#define ALPHABET                                                               \
  "0123456789ABCDEF"

string base16::encode( const string& in )
{

  string out;
  for ( uint8_t c : in ) {
    out.push_back(ALPHABET[(c & 0x0F)]);
    out.push_back(ALPHABET[((c >> 4) & 0x0F)]);
  }
  return out;
}
