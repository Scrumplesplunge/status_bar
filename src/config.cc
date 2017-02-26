#include "config.h"

#include <algorithm>
#include <cctype>
#include <fstream>

namespace {

std::runtime_error SyntaxError(std::string filename, int line) {
  return std::runtime_error(
      filename + ":" + std::to_string(line) + ": Syntax error.");
}

bool isident(char c) {
  return isalpha(c) || c == '_';
}

bool isnotident(char c) { return !isident(c); }

}  // namespace

Config LoadConfig(std::string filename) {
  Config config;

  std::ifstream file(filename);
  if (!file.good()) {
    throw std::runtime_error(
        "Could not open config file \"" + filename + "\".");
  }

  std::string section = "";
  std::string line;
  int line_number = 0;
  while (std::getline(file, line)) {
    line_number++;

    // Find the first non-whitespace character on the line.
    auto i = std::find_if_not(line.begin(), line.end(), isspace);
    if (i == line.end()) continue;  // Line consists only of whitespace.
    if (*i == '#') continue;        // Line contains a comment.
    if (*i == '[') {
      // Parse the section name.
      auto j = std::find(i + 1, line.end(), ']');
      if (j == line.end())
        throw SyntaxError(filename, line_number);
      section = std::string(i + 1, j);
      if (std::any_of(section.begin(), section.end(), isnotident))
        throw SyntaxError(filename, line_number);  // Section must be an ident.
      // Confirm that the remainder of the line is blank or comment.
      auto k = std::find_if_not(j + 1, line.end(), isspace);
      if (k == line.end()) continue;  // Remainder of line is whitespace.
      if (*k == '#') continue;  // Line ends with a comment.
      throw SyntaxError(filename, line_number);  // Junk after ]
    }
    if (isident(*i)) {
      // Parse the key.
      auto j = std::find_if_not(i, line.end(), isident);
      std::string key(i, j);
      auto k = std::find(j, line.end(), '=');
      if (k == line.end())
        throw SyntaxError(filename, line_number);  // No '=' found.
      // Parse the value.
      auto l = std::find_if_not(k + 1, line.end(), isspace);
      if (l == line.end())
        throw SyntaxError(filename, line_number);  // No value after '='.
      // Find the last non-whitespace character before the end of the line, or
      // before the start of any trailing comment.
      auto comment_start = std::find(l, line.end(), '#');
      auto m = std::find_if_not(
          std::reverse_iterator<std::string::iterator>(comment_start),
          line.rend(), isspace);
      std::string value(l, m.base() + 1);
      // Store the value in the config map.
      config[section][key] = value;
    }
  }

  return config;
}

const Section& Get(const Config& config, const char* section_name) {
  auto i = config.find(section_name);
  if (i == config.end()) {
    static const Section empty_section;
    return empty_section;
  }
  return i->second;
}

const Section& Get(const Config& config, const char* section_name,
                   const Section& default_value) {
  auto i = config.find(section_name);
  if (i == config.end()) return default_value;
  return i->second;
}

const std::string& Get(const Config& config, const char* section,
                       const char* variable) {
  return Get(Get(config, section), variable);
}

const std::string& Get(const Section& section, const char* variable) {
  auto i = section.find(variable);
  if (i == section.end()) {
    static const std::string empty_string;
    return empty_string;
  }
  return i->second;
}

const std::string& Get(const Section& section, const char* variable,
                       const std::string& value) {
  auto i = section.find(variable);
  if (i == section.end()) return value;
  return i->second;
}
