#include "config.h"

#include <algorithm>
#include <cctype>
#include <fstream>

namespace {

bool isident(char c) { return isalpha(c) || c == '_'; }
bool isnotident(char c) { return !isident(c); }

}  // namespace

Config::Config(std::string filename, Config* fallback_config)
    : filename_(filename), fallback_config_(fallback_config) {
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
        throw SyntaxError(line_number, "Expected ']'.");
      section = std::string(i + 1, j);
      if (std::any_of(section.begin(), section.end(), isnotident))
        throw SyntaxError(line_number, "Section name is not an identifier.");
      // Confirm that the remainder of the line is blank or comment.
      auto k = std::find_if_not(j + 1, line.end(), isspace);
      if (k == line.end()) continue;  // Remainder of line is whitespace.
      if (*k == '#') continue;  // Line ends with a comment.
      throw SyntaxError(line_number, "Unexpected characters after ']'.");
    }
    if (isident(*i)) {
      // Parse the key.
      auto j = std::find_if_not(i, line.end(), isident);
      std::string key(i, j);
      auto k = std::find(j, line.end(), '=');
      if (k == line.end())
        throw SyntaxError(line_number, "Expected '=' after variable name.");
      // Parse the value.
      auto l = std::find_if_not(k + 1, line.end(), isspace);
      if (l == line.end())
        throw SyntaxError(line_number, "Expected value after '='.");
      // Find the last non-whitespace character before the end of the line, or
      // before the start of any trailing comment.
      auto comment_start = std::find(l, line.end(), '#');
      auto m = std::find_if_not(
          std::reverse_iterator<std::string::iterator>(comment_start - 1),
          line.rend(), isspace);
      std::string value(l, m.base() + 1);
      // Store the value in the config map.
      contents_[section][key] = Variable{this, line_number, value};
    }
  }
}

const std::string& Config::Get(const SectionName& section_name,
                               const VariableName& variable_name) const {
  return GetVariable(section_name, variable_name).value;
}

std::string Config::Get(const SectionName& section_name,
                        const VariableName& variable_name,
                        const std::string& default_value) const {
  return GetVariable(section_name, variable_name, default_value).value;
}

std::runtime_error Config::SyntaxError(
    int line, const std::string& message) const {
  return std::runtime_error(
      filename_ + ":" + std::to_string(line) + ": " + message);
}

std::runtime_error Config::MissingConfigValue(
    const Config::SectionName& section_name,
    const Config::VariableName& variable_name) const {
  return std::runtime_error(
      "No value given for variable \"" + variable_name + "\" under section [" +
      section_name + "] in \"" + filename_ + "\", and no value was set by "
      "default.");
}

std::runtime_error Config::BadValueError(
    const Config::SectionName& section_name,
    const Config::VariableName& variable_name,
    const Config::Variable& variable) const {
  return std::runtime_error(
      "Illegal value for variable \"" + variable_name + "\" under section [" +
      section_name + "] at " + variable.definer->filename_ + ":" +
      std::to_string(variable.line));
}

const Config::Variable& Config::GetVariable(
    const Config::SectionName& section_name,
    const Config::VariableName& variable_name) const {
  // Look up the section.
  auto i = contents_.find(section_name);
  if (i == contents_.end()) {
    // Try looking up in the fallback config if there is one.
    if (fallback_config_ != nullptr)
      return fallback_config_->GetVariable(section_name, variable_name);
    throw MissingConfigValue(section_name, variable_name);
  }
  const Section& section = i->second;
  // Look up the variable in the section.
  auto j = section.find(variable_name);
  if (j == section.end()) {
    // Try looking up in the fallback config if there is one.
    if (fallback_config_ != nullptr)
      return fallback_config_->GetVariable(section_name, variable_name);
    throw MissingConfigValue(section_name, variable_name);
  }
  return j->second;
}

Config::Variable Config::GetVariable(
    const Config::SectionName& section_name,
    const Config::VariableName& variable_name,
    const std::string& default_value) const {
  // Look up the section.
  auto i = contents_.find(section_name);
  if (i == contents_.end()) {
    // Try looking up in the fallback config if there is one.
    if (fallback_config_ != nullptr) {
      return fallback_config_->GetVariable(
          section_name, variable_name, default_value);
    }
    return Variable{this, -1, default_value};
  }
  const Section& section = i->second;
  // Look up the variable in the section.
  auto j = section.find(variable_name);
  if (j == section.end()) {
    // Try looking up in the fallback config if there is one.
    if (fallback_config_ != nullptr) {
      return fallback_config_->GetVariable(
          section_name, variable_name, default_value);
    }
    return Variable{this, -1, default_value};
  }
  return j->second;
}
