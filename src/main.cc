
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>

#include "base64.h"

constexpr const char *file_in = "test/text.txt";

int main(int argc, char **argv) {
  std::ifstream file(file_in);  // open file for reading
  const std::string str(
      (std::istreambuf_iterator<char>(file)),
      std::istreambuf_iterator<char>());  // read file into string
  file.close();
  // std::cout << str << std::endl;
  {
    auto start_encode =
        std::chrono::high_resolution_clock::now();  // start encode timing
    std::string encoded = test1::b64encode(str);
    auto end_encode =
        std::chrono::high_resolution_clock::now();  // end encode timing

    auto start_decode =
        std::chrono::high_resolution_clock::now();  // start decode timing
    std::string decoded = test1::b64decode(encoded);
    auto end_decode =
        std::chrono::high_resolution_clock::now();  // end decode timing

    if (str == decoded) {
      std::cout << "Congratulations 🎇 test1" << std::endl;
    }

    auto encode_time = std::chrono::duration_cast<std::chrono::microseconds>(
        end_encode - start_encode);  // calculate encode time
    auto decode_time = std::chrono::duration_cast<std::chrono::microseconds>(
        end_decode - start_decode);  // calculate decode time
    // std::cout << "test1 encoded: " << encoded << std::endl;
    // std::cout << "test1 decoded: " << decoded << std::endl;

    std::cout << "test1 encode time: " << encode_time.count() << " microseconds"
              << std::endl;  // print encode time
    std::cout << "test1 decode time: " << decode_time.count() << " microseconds"
              << std::endl;  // print decode time
  }

  {
    auto start_encode =
        std::chrono::high_resolution_clock::now();  // start encode timing
    std::string encoded = test2::b64encode(str);
    auto end_encode =
        std::chrono::high_resolution_clock::now();  // end encode timing

    auto start_decode =
        std::chrono::high_resolution_clock::now();  // start decode timing
    std::string decoded = test2::b64decode(encoded);
    auto end_decode =
        std::chrono::high_resolution_clock::now();  // end decode timing

    if (str == decoded) {
      std::cout << "Congratulations 🎇 test2" << std::endl;
    }

    auto encode_time = std::chrono::duration_cast<std::chrono::microseconds>(
        end_encode - start_encode);  // calculate encode time
    auto decode_time = std::chrono::duration_cast<std::chrono::microseconds>(
        end_decode - start_decode);  // calculate decode time

    // std::cout << "test2 encoded: " << encoded << std::endl;
    // std::cout << "test2 decoded: " << decoded << std::endl;

    std::cout << "test2 encode time: " << encode_time.count() << " microseconds"
              << std::endl;  // print encode time
    std::cout << "test2 decode time: " << decode_time.count() << " microseconds"
              << std::endl;  // print decode time
  }
  // std::cout << encoded << std::endl;
  // std::cout << decoded << std::endl;

  return 0;
}
