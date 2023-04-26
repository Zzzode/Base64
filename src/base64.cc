/*
   base64.cpp and base64.h

   base64 encoding and decoding with C++.
   More information at
     https://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp

   Version: 2.rc.09 (release candidate)

   Copyright (C) 2004-2017, 2020-2022 René Nyffenegger

   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this source code must not be misrepresented; you must not
      claim that you wrote the original source code. If you use this source
      code in a product, an acknowledgment in the product documentation would
      be appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not
      be misrepresented as being the original source code.

   3. This notice may not be removed or altered from any source distribution.

   René Nyffenegger rene.nyffenegger@adp-gmbh.ch
*/

#include "base64.h"

#include <_types/_uint8_t.h>

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace test1 {
static constexpr char kPadding = '=';
/* clang-format off */
static const signed char base64de[] = {
        /* '+', ',', '-', '.', '/', '0', '1', '2', */
        62, -1, -1, -1, 63, 52, 53, 54,

        /* '3', '4', '5', '6', '7', '8', '9', ':', */
        55, 56, 57, 58, 59, 60, 61, -1,

        /* ';', '<', '=', '>', '?', '@', 'A', 'B', */
        -1, -1, 0, -1, -1, -1, 0, 1,

        /* 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', */
        2, 3, 4, 5, 6, 7, 8, 9,

        /* 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', */
        10, 11, 12, 13, 14, 15, 16, 17,

        /* 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', */
        18, 19, 20, 21, 22, 23, 24, 25,

        /* '[', '\', ']', '^', '_', '`', 'a', 'b', */
        -1, -1, -1, -1, -1, -1, 26, 27,

        /* 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', */
        28, 29, 30, 31, 32, 33, 34, 35,

        /* 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', */
        36, 37, 38, 39, 40, 41, 42, 43,

        /* 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', */
        44, 45, 46, 47, 48, 49, 50, 51,
};
/* clang-format on */

// Returns -1 in case of failure.
int decode(const char* str, uint32_t len, uint8_t* ret, uint32_t dst_size) {
  if (!len || str == nullptr) {
    return -1;
  }
  const size_t min_dst_size = dec_size(len);
  if (dst_size < min_dst_size) {
    return -1;
  }
  size_t wr_size = 0;
#pragma omp parallel for
  for (int j = 0; j < len; j += 4) {
    uint32_t val =
        (base64de[str[j] - '+'] << 18) | (base64de[str[j + 1] - '+'] << 12) |
        (base64de[str[j + 2] - '+'] << 6) | (base64de[str[j + 3] - '+']);
    wr_size = (j >> 2) * 3;
    ret[wr_size] = val >> 16;
    ret[wr_size + 1] = val >> 8;
    ret[wr_size + 2] = val;
  }

  if (str[len - 1] == kPadding) {
    wr_size--;
  } else if (str[len - 2] == kPadding) {
    wr_size -= 2;
  }
  return wr_size;
}

void encode(const uint8_t* bytes, uint32_t len, char* chars) {
  uint32_t end = len / 3;
  constexpr const char* b64_chars =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
#pragma omp parallel for
  for (uint32_t i = 0; i < end; i++) {
    int j = i + (i << 1), k = i << 2;
    uint32_t three_bytes = bytes[j] << 16 | bytes[j + 1] << 8 | bytes[j + 2];
    chars[k] = b64_chars[three_bytes >> 18 & 63];
    chars[k + 1] = b64_chars[three_bytes >> 12 & 63];
    chars[k + 2] = b64_chars[three_bytes >> 6 & 63];
    chars[k + 3] = b64_chars[three_bytes & 63];
  }

  int loc = end << 2;
  end += end << 1;
  switch (len - end) {
    case 0:
      break;
    case 1: {
      uint32_t last = bytes[end] << 4;
      chars[loc++] = b64_chars[last >> 6 & 63];
      chars[loc++] = b64_chars[last & 63];
      chars[loc++] = '=';
      chars[loc++] = '=';
      break;
    }
    case 2: {
      uint32_t last = bytes[end] << 10 | bytes[end + 1] << 2;
      chars[loc++] = b64_chars[last >> 12 & 63];
      chars[loc++] = b64_chars[last >> 6 & 63];
      chars[loc++] = b64_chars[last & 63];
      chars[loc++] = '=';
      break;
    }
    default:
      break;
  }
  chars[loc] = 0;
}

std::string b64encode(const std::string& str) {
  const char* input = str.c_str();
  size_t len = str.size();
  char* output = new char[(len + 2) / 3 * 4];
  encode((const uint8_t*)input, len, output);
  return output;
}

std::string b64decode(const std::string& str) {
  const char* input = str.c_str();
  size_t len = str.size();

  size_t j = 0, pad1 = len % 4 || input[len - 1] == '=',
         pad2 = pad1 && (len % 4 > 2 || input[len - 2] != '=');
  const size_t last = (len - pad1) / 4 << 2;
  size_t size = str.size();

  char* output = new char[last / 4 * 3 + pad1 + pad2];

  if (decode(input, len, (uint8_t*)output, len) == -1) {
    return "";
  }

  return output;
}
}  // namespace test1

namespace test2 {
static constexpr const char* B64chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static constexpr const int B64index[256] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  62, 63, 62, 62, 63, 52, 53, 54, 55, 56, 57,
    58, 59, 60, 61, 0,  0,  0,  0,  0,  0,  0,  0,  1,  2,  3,  4,  5,  6,
    7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    25, 0,  0,  0,  0,  63, 0,  26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
    37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51};

const std::string b64encode(const void* data, const size_t& len) {
  std::string result((len + 2) / 3 * 4, '=');
  unsigned char* p = (unsigned char*)data;
  char* str = &result[0];
  size_t j = 0, pad = len % 3;
  const size_t last = len - pad;

  for (size_t i = 0; i < last; i += 3) {
    int n = int(p[i]) << 16 | int(p[i + 1]) << 8 | p[i + 2];
    str[j++] = B64chars[n >> 18];
    str[j++] = B64chars[n >> 12 & 0x3F];
    str[j++] = B64chars[n >> 6 & 0x3F];
    str[j++] = B64chars[n & 0x3F];
  }
  if (pad)  /// Set padding
  {
    int n = --pad ? int(p[last]) << 8 | p[last + 1] : p[last];
    str[j++] = B64chars[pad ? n >> 10 & 0x3F : n >> 2];
    str[j++] = B64chars[pad ? n >> 4 & 0x03F : n << 4 & 0x3F];
    str[j++] = pad ? B64chars[n << 2 & 0x3F] : '=';
  }
  return result;
}

const std::string b64decode(const void* data, const size_t& len) {
  if (len == 0) return "";

  unsigned char* p = (unsigned char*)data;
  size_t j = 0, pad1 = len % 4 || p[len - 1] == '=',
         pad2 = pad1 && (len % 4 > 2 || p[len - 2] != '=');
  const size_t last = (len - pad1) / 4 << 2;
  std::string result(last / 4 * 3 + pad1 + pad2, '\0');
  unsigned char* str = (unsigned char*)&result[0];

  for (size_t i = 0; i < last; i += 4) {
    int n = B64index[p[i]] << 18 | B64index[p[i + 1]] << 12 |
            B64index[p[i + 2]] << 6 | B64index[p[i + 3]];
    str[j++] = n >> 16;
    str[j++] = n >> 8 & 0xFF;
    str[j++] = n & 0xFF;
  }
  if (pad1) {
    int n = B64index[p[last]] << 18 | B64index[p[last + 1]] << 12;
    str[j++] = n >> 16;
    if (pad2) {
      n |= B64index[p[last + 2]] << 6;
      str[j++] = n >> 8 & 0xFF;
    }
  }
  return result;
}

std::string b64encode(const std::string& str) {
  return b64encode(str.c_str(), str.size());
}

std::string b64decode(const std::string& str64) {
  return b64decode(str64.c_str(), str64.size());
}

}  // namespace test2