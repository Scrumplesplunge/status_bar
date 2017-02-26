#pragma once

#include <map>
#include <string>

using Section = std::map<std::string, std::string>;
using Config = std::map<std::string, Section>;

// Load a config file.
Config LoadConfig(std::string filename);

// Fetch the specified section from the configuration.
const Section& Get(const Config& config, const char* section_name);
const Section& Get(const Config& config, const char* section_name,
                   const Section& default_value);

// Fetch the specified variable from the configuration.
const std::string& Get(const Config& config, const char* section,
                       const char* variable);
const std::string& Get(const Section& section, const char* variable);
const std::string& Get(const Section& section, const char* variable,
                       const std::string& value);
