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

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace test1 {
// Depending on the url parameter in base64_chars, one of two sets of base64
// characters needs to be chosen. They differ in their last two characters.
static constexpr const char* base64_chars[2] = {
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/",
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_"};

// Return the position of chr within base64_encode()
static unsigned int pos_of_char(const unsigned char chr) {
  if (chr >= 'A' && chr <= 'Z')
    return chr - 'A';
  else if (chr >= 'a' && chr <= 'z')
    return chr - 'a' + ('Z' - 'A') + 1;
  else if (chr >= '0' && chr <= '9')
    return chr - '0' + ('Z' - 'A') + ('z' - 'a') + 2;
  else if (chr == '+' || chr == '-')
    return 62;  // Be liberal with input and accept both url ('-') and non-url
                // ('+') base 64 characters (
  else if (chr == '/' || chr == '_')
    return 63;  // Ditto for '/' and '_'
  else
    // 2020-10-23: Throw std::exception rather than const char*
    //(Pablo Martin-Gomez, https://github.com/Bouska)
    throw std::runtime_error("Input is not valid base64-encoded data.");
}

static std::string base64_encode(unsigned char const* bytes_to_encode,
                                 size_t in_len, bool url) {
  size_t len_encoded = (in_len + 2) / 3 * 4;

  char trailing_char = url ? '.' : '=';

  // Choose set of base64 characters. They differ for the last two positions,
  // depending on the url parameter.
  // A bool (as is the parameter url) is guaranteed to evaluate to either 0 or 1
  // in C++ therefore, the correct character set is chosen by subscripting
  // base64_chars with url.
  const char* base64_chars_ = base64_chars[url ? 1 : 0];

  std::string ret;
  ret.reserve(len_encoded);

  unsigned int pos = 0;

  while (pos < in_len) {
    ret.push_back(base64_chars_[(bytes_to_encode[pos + 0] & 0xfc) >> 2]);

    if (pos + 1 < in_len) {
      ret.push_back(base64_chars_[((bytes_to_encode[pos + 0] & 0x03) << 4) +
                                  ((bytes_to_encode[pos + 1] & 0xf0) >> 4)]);

      if (pos + 2 < in_len) {
        ret.push_back(base64_chars_[((bytes_to_encode[pos + 1] & 0x0f) << 2) +
                                    ((bytes_to_encode[pos + 2] & 0xc0) >> 6)]);
        ret.push_back(base64_chars_[bytes_to_encode[pos + 2] & 0x3f]);
      } else {
        ret.push_back(base64_chars_[(bytes_to_encode[pos + 1] & 0x0f) << 2]);
        ret.push_back(trailing_char);
      }
    } else {
      ret.push_back(base64_chars_[(bytes_to_encode[pos + 0] & 0x03) << 4]);
      ret.push_back(trailing_char);
      ret.push_back(trailing_char);
    }

    pos += 3;
  }

  return ret;
}

static std::string base64_decode(std::string const& encoded_string,
                                 bool remove_linebreaks) {
  if (encoded_string.empty()) return {};

  if (remove_linebreaks) {
    std::string copy(encoded_string);

    copy.erase(std::remove(copy.begin(), copy.end(), '\n'), copy.end());

    return base64_decode(copy, false);
  }

  size_t length_of_string = encoded_string.length();
  size_t pos = 0;

  // The approximate length (bytes) of the decoded string might be one or
  // two bytes smaller, depending on the amount of trailing equal signs
  // in the encoded string. This approximation is needed to reserve
  // enough space in the string to be returned.
  size_t approx_length_of_decoded_string = length_of_string / 4 * 3;
  std::string ret;
  ret.reserve(approx_length_of_decoded_string);

  while (pos < length_of_string) {
    // Iterate over encoded input string in chunks. The size of all
    // chunks except the last one is 4 bytes.
    //
    // The last chunk might be padded with equal signs or dots
    // in order to make it 4 bytes in size as well, but this
    // is not required as per RFC 2045.
    //
    // All chunks except the last one produce three output bytes.
    //
    // The last chunk produces at least one and up to three bytes.

    size_t pos_of_char_1 = pos_of_char(encoded_string.at(pos + 1));

    // Emit the first output byte that is produced in each chunk:
    ret.push_back(static_cast<std::string::value_type>(
        ((pos_of_char(encoded_string.at(pos + 0))) << 2) +
        ((pos_of_char_1 & 0x30) >> 4)));

    if ((pos + 2 <
         length_of_string) &&  // Check for data that is not padded with equal
                               // signs (which is allowed by RFC 2045)
        encoded_string.at(pos + 2) != '=' &&
        encoded_string.at(pos + 2) !=
            '.'  // accept URL-safe base 64 strings, too, so check for '.' also.
    ) {
      // Emit a chunk's second byte (which might not be produced in the last
      // chunk).
      unsigned int pos_of_char_2 = pos_of_char(encoded_string.at(pos + 2));
      ret.push_back(static_cast<std::string::value_type>(
          ((pos_of_char_1 & 0x0f) << 4) + ((pos_of_char_2 & 0x3c) >> 2)));

      if ((pos + 3 < length_of_string) && encoded_string.at(pos + 3) != '=' &&
          encoded_string.at(pos + 3) != '.') {
        // Emit a chunk's third byte (which might not be produced in the last
        // chunk).
        ret.push_back(static_cast<std::string::value_type>(
            ((pos_of_char_2 & 0x03) << 6) +
            pos_of_char(encoded_string.at(pos + 3))));
      }
    }

    pos += 4;
  }

  return ret;
}

std::string decode(std::string const& s, bool remove_linebreaks) {
  return base64_decode(s, remove_linebreaks);
}

std::string encode(std::string const& s, bool url) {
  return base64_encode(reinterpret_cast<const unsigned char*>(s.data()),
                       s.length(), url);
}
}  // namespace test1

namespace test2 {
static constexpr const unsigned char base64_table[65] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/**
 * base64_encode - Base64 encode
 * @src: Data to be encoded
 * @len: Length of the data to be encoded
 * @out_len: Pointer to output length variable, or %NULL if not used
 * Returns: Allocated buffer of out_len bytes of encoded data,
 * or empty string on failure
 */
std::string base64_encode(const unsigned char* src, size_t len) {
  unsigned char *out, *pos;
  const unsigned char *end, *in;

  size_t olen;

  olen = 4 * ((len + 2) / 3); /* 3-byte blocks to 4-byte */

  if (olen < len) return std::string(); /* integer overflow */

  std::string outStr;
  outStr.resize(olen);
  out = (unsigned char*)&outStr[0];

  end = src + len;
  in = src;
  pos = out;
  while (end - in >= 3) {
    *pos++ = base64_table[in[0] >> 2];
    *pos++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
    *pos++ = base64_table[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
    *pos++ = base64_table[in[2] & 0x3f];
    in += 3;
  }

  if (end - in) {
    *pos++ = base64_table[in[0] >> 2];
    if (end - in == 1) {
      *pos++ = base64_table[(in[0] & 0x03) << 4];
      *pos++ = '=';
    } else {
      *pos++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
      *pos++ = base64_table[(in[1] & 0x0f) << 2];
    }
    *pos++ = '=';
  }

  return outStr;
}

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

std::string base64_decode(const std::string& in) {
  std::string out;

  std::vector<int> T(256, -1);
  for (int i = 0; i < 64; i++)
    T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] =
        i;

  int val = 0, valb = -8;
  for (unsigned char c : in) {
    if (T[c] == -1) break;
    val = (val << 6) + T[c];
    valb += 6;
    if (valb >= 0) {
      out.push_back(char((val >> valb) & 0xFF));
      valb -= 8;
    }
  }
  return out;
}

}  // namespace test2