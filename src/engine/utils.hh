#pragma once

#include <cstdarg>
#include <string>
#include <fstream>
#include <vector>

#ifdef __PRETTY_FUNCTION__
#define info() \
  printf("%s:%d in %s", __FILE__, __LINE__, __PRETTY_FUNCTION__);
#else
#define info() \
  printf("%s:%d in %s", __FILE__, __LINE__, __func__);
#endif

#define assertf(X, ...) \
  do { \
    if (!(X)) { \
      printf("assertion `%s' failed at ", #X); \
      info() \
      printf(": "); \
      printf(__VA_ARGS__); \
      puts(""); \
      exit(1); \
    } \
  } while (0)

#define die(...) do { printf(__VA_ARGS__); puts(""); exit(1); } while (0)

#define normal_exit(...) do { printf(__VA_ARGS__); puts(""); exit(0); } while (0)

#define warning(...) do { \
  printf("warning: "); \
  printf(__VA_ARGS__); \
  puts(""); \
} while (0)

#define warning_ln(...) do { \
  printf("warning at "); \
  info() \
  printf(": "); \
  printf(__VA_ARGS__); \
  puts(""); \
} while (0)

#define _glsl(X) "#version 120\n" #X

inline void print_packet(uint8_t *packet, size_t len
    , const char *msg = "packet") {
  printf("%s: ", msg);
  for (size_t i = 0; i < len; i++) {
    int numbits = 8;
    while (--numbits >= 0)
      printf("%c", (packet[i] & ((uint8_t)1 << numbits)) ? '1' : '0');
    printf(" ");
  }
  printf("\n");
}

inline void read_file_to_vector(const std::string &filename
    , std::vector<char> &dest) {
  std::ifstream ifs(filename, std::ios::binary);
  assertf(ifs, "failed to open file \"%s\"", filename.c_str());
  ifs.seekg(0, ifs.end);
  size_t length = ifs.tellg();
  ifs.seekg(0, ifs.beg);
  dest.resize(length);
  ifs.read(dest.data(), length);
  ifs.close();
}

inline std::string read_file_to_string(const std::string &filename) {
  std::ifstream ifs(filename); // std::ios::binary?
  assertf(ifs, "failed to open file \"%s\"", filename.c_str());
  std::string buffer { std::istreambuf_iterator<char>(ifs)
    , std::istreambuf_iterator<char>() };
  return buffer; // std::move?
}

