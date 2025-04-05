#include "log.hpp"
#pragma comment(lib, "Advapi32.lib")
#include "Windows.h" // IWYU pragma: keep
#include "path.hpp"

std::string get_main_exe_filename() {
  char exe_filename[1024]{0};
  GetModuleFileName(NULL, exe_filename, 1024);
  std::string res = exe_filename;
  return res.substr(res.find_last_of("\\") + 1);
}
std::string get_cwd() {
  char exe_filename[1024]{0};
  GetModuleFileName(NULL, exe_filename, 1024);
  std::string res = exe_filename;
  return res.substr(0, res.find_last_of("\\"));
}
std::string get_path() {

  if (get_main_exe_filename() == "Minecraft.Windows.exe") {
    char username[100]{0};
    DWORD len = 100;
    GetUserName(username, &len);
    std::string name = username;
    return R"(C:\Users\)" + name +
           R"(\AppData\Local\Packages\Microsoft.MinecraftUWP_8wekyb3d8bbwe\LocalState\games\com.mojang)";
  } else {
    return get_cwd() + R"(\plugins\dimhook)";
  }
}
#include <fstream>
void init_config(const std::string &name, nlohmann::json &config) {
  const auto path = get_path() + "\\" + name;
  try {
    config = nlohmann::json::parse(std::ifstream{path}, nullptr, true, true);
  } catch (...) {
    try {
      std::ofstream{path} << config.dump(4);
    } catch (...) {
      LOG("rewrite {} failed", name);
    }
  }
}