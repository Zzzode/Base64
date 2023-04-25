//
//  base64 encoding and decoding with C++.
//  Version: 2.rc.09 (release candidate)
//

#ifndef BASE64_H_
#define BASE64_H_

#include <string>

namespace test1 {
std::string encode(std::string const& s, bool url = false);
std::string decode(std::string const& s, bool remove_linebreaks = false);
}  // namespace test1

namespace test2 {
std::string base64_encode(const unsigned char* src, size_t len);
std::string base64_decode(const void* data, const size_t len);
std::string base64_decode(const std::string& encoded_string);
std::string b64encode(const std::string& str);
std::string b64decode(const std::string& str64);
}  // namespace test2
#endif
