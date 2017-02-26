#pragma once

#include "config.h"

#include <sstream>
#include <stdexcept>

template <typename T>
T Config::GetAs(const Config::SectionName& section_name,
                const Config::VariableName& variable_name) const {
  const Variable& variable = GetVariable(section_name, variable_name);
  std::istringstream input(variable.value);
  T output;
  input >> output;
  if (!input.good()) throw BadValueError(section_name, variable_name, variable);
  return output;
}

template <typename T>
T Config::GetAs(const SectionName& section_name,
                const VariableName& variable_name,
                const T& default_value) const {
  // Variables cannot have empty contents, so this will only return "" if the
  // variable was not set.
  const Variable& variable = GetVariable(section_name, variable_name, "");
  if (variable.value.length() == 0) return default_value;
  std::istringstream input(variable.value);
  T output;
  input >> output;
  // Check that a value was successfully read.
  if (!input.good()) throw BadValueError(section_name, variable_name, variable);
  // Check that the input operation consumed the entire string.
  char c;
  if (input.get(c))
    throw BadValueError(section_name, variable_name, variable);
  return output;
}
