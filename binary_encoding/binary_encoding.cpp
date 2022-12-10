#include <array>
#include <cstdint>
#include <cstring>
#include <string>
#include <iostream>


template<class T>
std::string encode_binary(const T& object) {
  auto src = reinterpret_cast<const char*>(&object);
  return std::string(src, sizeof(T));
}

template<class T>
void decode_binary(const std::string& binary, T& object) {
  auto dst = reinterpret_cast<char*>(&object);
  memcpy(dst, binary.c_str(), sizeof(T));
}

char b64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

template<class T>
std::string encode_base64(const T& object) {
  constexpr int length_encoding = 4*(sizeof(T)/3) + 4*(sizeof(T)%3>0);
  std::cout << sizeof(T) << ' ' << length_encoding << std::endl;
  std::string encoded;
  encoded.reserve(length_encoding);
  auto ptr = reinterpret_cast<const unsigned char*>(&object);
  auto end = ptr+sizeof(T);
  uint_fast32_t accumulator;
  while (ptr < end-2) {
    accumulator  = (*ptr++) << 16;
    accumulator |= (*ptr++) << 8;
    accumulator |= (*ptr++);
    encoded.append(1, b64[(accumulator>>18) & 0x3F]);
    encoded.append(1, b64[(accumulator>>12) & 0x3F]);
    encoded.append(1, b64[(accumulator>>6) & 0x3F]);
    encoded.append(1, b64[accumulator & 0x3F]);
  }
  if constexpr(sizeof(T)%3 == 1) {
    accumulator = (*ptr++) << 16;
    encoded.append(1, b64[(accumulator>>18) & 0x3F]);
    encoded.append(1, b64[(accumulator>>12) & 0x3F]);
    encoded.append(2, '=');
  }
  else if constexpr(sizeof(T)%3 == 2) {
    accumulator  = (*ptr++) << 16;
    accumulator |= (*ptr++) << 8;
    encoded.append(1, b64[(accumulator>>18) & 0x3F]);
    encoded.append(1, b64[(accumulator>>12) & 0x3F]);
    encoded.append(1, b64[(accumulator>>6) & 0x3F]);
    encoded.append(1, '=');
  }
  std::cout << encoded.length() << ' ' << encoded.capacity() << std::endl;
  return encoded;
}

struct MyStruct {
  int a,b;
};

int main() {
  std::cout << sizeof(std::array<double,10>) << std::endl;

  //MyStruct x = {{'a', 'n', 'y', ' ', 'c', 'a', 'r', 'n', 'a', 'l', ' ', 'p', 'l', 'e', 'a', 's', 'u', 'r', 'e'}};
  MyStruct x = {2020,-42}; 
  auto binary_encoding = encode_binary(x);
  std::hash<std::string> hasher;
  std::cout << binary_encoding.length() << ' ' << hasher(binary_encoding) << std::endl;
  MyStruct y;
  decode_binary(binary_encoding, y);
  std::cout << y.a << ' ' << y.b << std::endl;
  //auto base64_encoding = encode_base64(x);
  //std::cout << binary_encoding << std::endl;
  //std::cout << base64_encoding << std::endl;
}

