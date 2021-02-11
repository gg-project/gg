/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "base64.hh"

#include <algorithm>
#include <string>
#include <openssl/evp.h>
#include <iostream>
#include <vector>

using namespace std;
using namespace gg;

// Modified from
// https://stackoverflow.com/questions/5288076/base64-encoding-and-decoding-with-openssl
string base64::encode( const string& in ) {
  const unsigned char * input = reinterpret_cast<const unsigned char *>(in.c_str());
  int length = in.size();
  const auto pl = 4*((length+2)/3);
  auto output = reinterpret_cast<char *>(calloc(pl+1, 1)); //+1 for the terminating null that EVP_EncodeBlock adds on
  const auto ol = EVP_EncodeBlock(reinterpret_cast<unsigned char *>(output), input, length);
  if (pl != ol) { std::cerr << "Whoops, encode predicted " << pl << " but we got " << ol << "\n"; }
  string o(output);
  free(output);
  return o;
}

string base64::decode(const string& in) {
  const char *input = in.c_str();
  int length = in.size();
  const auto pl = 3*length/4;
  auto output = reinterpret_cast<unsigned char *>(calloc(pl+1, 1));
  const auto ol = EVP_DecodeBlock(output, reinterpret_cast<const unsigned char *>(input), length);
  if (pl != ol) { std::cerr << "Whoops, decode predicted " << pl << " but we got " << ol << "\n"; }
  string o(reinterpret_cast<char *>(output));
  free(output);
  return o;
}

string base64::url_decode(const string& in) {
  string i(in);
  replace(i.begin(), i.end(), '-', '+');
  replace(i.begin(), i.end(), '_', '/');
  return decode(i);
}
string base64::url_encode(const string& in) {
  string o = encode(in);
  replace(o.begin(), o.end(), '+', '-');
  replace(o.begin(), o.end(), '/', '_');
  return o;
}

