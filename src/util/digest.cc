/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "digest.hh"
#include "base64.hh"

#include <iostream>
#include <string>

#include <openssl/sha.h>

using namespace std;
using namespace gg;

string digest::sha256( const string& input )
{
  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256;
  SHA256_Init( &sha256 );
  SHA256_Update( &sha256, input.c_str(), input.length() );
  SHA256_Final( hash, &sha256 );
  string b64 = base64::encode( string( hash, hash + SHA256_DIGEST_LENGTH ) );
  return b64;
}
