// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#pragma once
#include <array>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
namespace neon {
// Minimal SHA-256 (public domain style) adapted for project
class Sha256 {
  uint64_t bitlen_=0;
  std::array<uint32_t,8> state_{
    0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19};
  std::array<uint8_t,64> buf_{}; size_t buflen_=0;
  static uint32_t rotr(uint32_t x, uint32_t n){ return (x>>n)|(x<<(32-n)); }
  static uint32_t ch(uint32_t x,uint32_t y,uint32_t z){ return (x&y)^(~x&z); }
  static uint32_t maj(uint32_t x,uint32_t y,uint32_t z){ return (x&y)^(x&z)^(y&z); }
  static uint32_t bsig0(uint32_t x){ return rotr(x,2)^rotr(x,13)^rotr(x,22); }
  static uint32_t bsig1(uint32_t x){ return rotr(x,6)^rotr(x,11)^rotr(x,25); }
  static uint32_t ssig0(uint32_t x){ return rotr(x,7)^rotr(x,18)^(x>>3); }
  static uint32_t ssig1(uint32_t x){ return rotr(x,17)^rotr(x,19)^(x>>10); }
  static const uint32_t K[64];
  void transform(const uint8_t block[64]){
    uint32_t w[64];
    for (int i=0;i<16;++i){
      w[i]=(block[i*4]<<24)|(block[i*4+1]<<16)|(block[i*4+2]<<8)|(block[i*4+3]);
    }
    for (int i=16;i<64;++i) w[i]=ssig1(w[i-2])+w[i-7]+ssig0(w[i-15])+w[i-16];
    uint32_t a=state_[0],b=state_[1],c=state_[2],d=state_[3],e=state_[4],f=state_[5],g=state_[6],h=state_[7];
    for (int i=0;i<64;++i){
      uint32_t t1=h+bsig1(e)+ch(e,f,g)+K[i]+w[i];
      uint32_t t2=bsig0(a)+maj(a,b,c);
      h=g; g=f; f=e; e=d+t1; d=c; c=b; b=a; a=t1+t2;
    }
    state_[0]+=a; state_[1]+=b; state_[2]+=c; state_[3]+=d; state_[4]+=e; state_[5]+=f; state_[6]+=g; state_[7]+=h;
  }
 public:
  void update(const void* data, size_t len){
    const uint8_t* p = static_cast<const uint8_t*>(data);
    bitlen_ += (uint64_t)len * 8;
    while (len>0){
      size_t t = 64 - buflen_; if (t>len) t=len;
      std::memcpy(buf_.data()+buflen_, p, t);
      buflen_ += t; p += t; len -= t;
      if (buflen_==64){ transform(buf_.data()); buflen_=0; }
    }
  }
  std::array<uint8_t,32> digest(){
    uint8_t pad[64]{}; pad[0]=0x80;
    size_t padlen = (buflen_<56)? (56-buflen_) : (120-buflen_);
    update(pad, padlen);
    uint8_t lenb[8]; for (int i=0;i<8;++i) lenb[7-i]=(bitlen_>>(i*8))&0xff;
    update(lenb, 8);
    std::array<uint8_t,32> out{};
    for (int i=0;i<8;++i){
      out[i*4]=(state_[i]>>24)&0xff; out[i*4+1]=(state_[i]>>16)&0xff;
      out[i*4+2]=(state_[i]>>8)&0xff; out[i*4+3]=state_[i]&0xff;
    }
    return out;
  }
  static std::string hex(const std::array<uint8_t,32>& d){
    static const char* k="0123456789abcdef"; std::string s; s.reserve(64);
    for (uint8_t b: d){ s.push_back(k[b>>4]); s.push_back(k[b&0x0f]); }
    return s;
  }
};
inline const uint32_t Sha256::K[64] = {
  0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
  0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
  0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
  0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
  0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
  0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
  0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
  0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};
} // namespace neon


inline std::string HmacSha256(const std::string& key, const std::string& data){
  // RFC 2104 HMAC using Sha256
  const size_t B=64;
  std::string k = key;
  if (k.size()>B){ Sha256 hk; hk.update(k.data(), k.size()); auto d = hk.digest(); k = std::string((char*)d.data(), d.size()); }
  if (k.size()<B) k.append(B - k.size(), '\0');
  std::string o(B,'\x5c'), i(B,'\x36');
  for (size_t n=0;n<B;++n){ o[n] = (char)(o[n] ^ k[n]); i[n] = (char)(i[n] ^ k[n]); }
  Sha256 hi; hi.update(i.data(), i.size()); hi.update(data.data(), data.size()); auto di = hi.digest();
  Sha256 ho; ho.update(o.data(), o.size()); ho.update(di.data(), di.size()); auto do_ = ho.digest();
  return Sha256::hex(do_);
}
