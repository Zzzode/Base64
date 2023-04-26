//
//  base64 encoding and decoding with C++.
//  Version: 2.rc.09 (release candidate)
//

#ifndef BASE64_H_
#define BASE64_H_

#include <string>

namespace test1 {
inline constexpr size_t dec_size(size_t src_size) {
  return (src_size + 3) / 4 * 3;
}

std::string b64encode(const std::string& str);
std::string b64decode(const std::string& str);
}  // namespace test1

namespace test2 {
std::string b64encode(const std::string& str);
std::string b64decode(const std::string& str);
}  // namespace test2
#endif
