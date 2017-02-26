#pragma once

#include <chrono>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

// Thin wrapper around std::chrono::milliseconds to allow string-parsing of
// numbers with units.
struct Duration { std::chrono::milliseconds value; };

std::istream& operator>>(std::istream& input, Duration& duration);

class Config {
 public:
  struct Variable {
    const Config* definer;
    int line;
    std::string value;
  };
  using VariableName = std::string;
  using SectionName = std::string;
  using Section = std::map<VariableName, Variable>;

  // Load a config from a file. If a fallback_config is specified, any variables
  // which are unset in this config will be looked up in the fallback config.
  Config(std::string filename, Config* fallback_config = nullptr);

  // Get the value of a variable, or throw an exception if the variable is not
  // defined.
  const std::string& Get(const SectionName& section_name,
                         const VariableName& variable_name) const;

  // Like above, but additionally parse as type T or throw an exception if the
  // variable is invalid.
  template <typename T>
  T GetAs(const SectionName& section_name,
          const VariableName& variable_name) const;

  // Get the value of a variable, or return the default value if the variable is
  // not defined.
  std::string Get(const SectionName& section_name,
                  const VariableName& variable_name,
                  const std::string& default_value) const;

  template <typename T>
  T GetAs(const SectionName& section_name, const VariableName& variable_name,
          const T& default_value) const;

 private:
  std::runtime_error SyntaxError(int line, const std::string& message) const;
  std::runtime_error MissingConfigValue(
      const SectionName& section_name, const VariableName& variable_name) const;
  std::runtime_error BadValueError(
      const SectionName& section_name, const VariableName& variable_name,
      const Variable& variable) const;

  const Variable& GetVariable(
      const SectionName& section_name, const VariableName& variable_name) const;
  Variable GetVariable(const SectionName& section_name,
                       const VariableName& variable_name,
                       const std::string& default_value) const;

  std::string filename_;
  std::map<SectionName, Section> contents_;
  Config* fallback_config_;
};

// Template definitions are here.
#include "config.inl.h"
