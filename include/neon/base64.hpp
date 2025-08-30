// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#pragma once
#include <string>
namespace neon {
inline std::string Base64Encode(const std::string& s){
  static const char* T="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  std::string o; o.reserve(((s.size()+2)/3)*4);
  size_t i=0;
  while (i<s.size()){
    uint32_t a = (unsigned char)s[i++];
    uint32_t b = i<s.size()? (unsigned char)s[i++] : 0;
    uint32_t c = i<s.size()? (unsigned char)s[i++] : 0;
    uint32_t triple = (a<<16)|(b<<8)|c;
    o.push_back(T[(triple>>18)&0x3F]);
    o.push_back(T[(triple>>12)&0x3F]);
    o.push_back(i-1 > s.size()? '=' : T[(triple>>6)&0x3F]);
    o.push_back(i > s.size()? '=' : T[triple&0x3F]);
  }
  size_t mod = s.size()%3;
  if (mod==1){ o[o.size()-1]='='; o[o.size()-2]='='; }
  else if (mod==2){ o[o.size()-1]='='; }
  return o;
}
inline std::string Base64Decode(const std::string& in){
  auto val = [](char c)->int{
    if ('A'<=c && c<='Z') return c-'A';
    if ('a'<=c && c<='z') return c-'a'+26;
    if ('0'<=c && c<='9') return c-'0'+52;
    if (c=='+') return 62; if (c=='/') return 63; return -1;
  };
  std::string out; out.reserve((in.size()/4)*3);
  int buffer=0, bits=0;
  for (char c: in){
    if (c=='=') break;
    int v = val(c); if (v<0) continue;
    buffer = (buffer<<6)|v; bits += 6;
    if (bits>=8){ bits-=8; out.push_back(char((buffer>>bits)&0xFF)); }
  }
  return out;
}
} // namespace neon
