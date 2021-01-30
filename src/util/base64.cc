/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "base64.hh"

#include <string>
#include <vector>

using namespace std;
using namespace gg;

#define ALPHABET                                                               \
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_"

// Modified from
// https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c
string base64::encode( const string& in )
{

  string out;

  int val = 0, valb = -6;
  for ( uint8_t c : in ) {
    val = ( val << 8 ) + c;
    valb += 8;
    while ( valb >= 0 ) {
      // modified from original for URL
      out.push_back( ALPHABET[( val >> valb ) & 0x3F] );
      valb -= 6;
    }
  }
  if ( valb > -6 )
    out.push_back( ALPHABET[( ( val << 8 ) >> ( valb + 8 ) ) & 0x3F] );
  return out;
}

string base64::decode( const string& in )
{

  string out;

  vector<int> T( 256, -1 );
  for ( int i = 0; i < 64; i++ )
    T[ALPHABET[i]] = i;

  int val = 0, valb = -8;
  for ( uint8_t c : in ) {
    if ( T[c] == -1 )
      break;
    val = ( val << 6 ) + T[c];
    valb += 6;
    if ( valb >= 0 ) {
      out.push_back( char( ( val >> valb ) & 0xFF ) );
      valb -= 8;
    }
  }
  return out;
}
