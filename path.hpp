#pragma once
#include <string>
std::string get_main_exe_filename();
std::string get_cwd();
std::string get_path();
#include "nlohmann/json.hpp" // IWYU pragma: keep
void init_config(const std::string &, nlohmann::json &);