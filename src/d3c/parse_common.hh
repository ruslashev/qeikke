#pragma once

#include <string>
#include <fstream>

static std::string stream_get_next_value(std::ifstream &stream) {
  std::string s;
  while (stream >> s) {
    if (s == "/*") // ignore block comments
      while (s != "*/")
        stream >> s;
    else if (s == "{" || s == "}") { // ignore braces and parens for purpose
    } else if (s == "(" || s == ")") { // of simplifying parsing
    } else
      return s;
  }
  return "EOF";
}

static std::string stream_get_next_string(std::ifstream &stream) {
  std::string s = stream_get_next_value(stream);
  return s.substr(1, s.size() - 2); // strip quotes
}

static float stream_get_next_float(std::ifstream &stream) {
  return atof(stream_get_next_value(stream).c_str());
}

static float stream_get_next_int(std::ifstream &stream) {
  return atoi(stream_get_next_value(stream).c_str());
}

