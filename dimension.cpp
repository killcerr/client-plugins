#include "log.hpp"
#include "path.hpp"

nlohmann::json dimension_config = {
    {
        "Overworld",
        {
            {"min", -64},
            {"max", 1024},
        },
    },
    {
        "Nether",
        {
            {"min", 0},
            {"max", 1024},
        },
    },
    {
        "TheEnd",
        {
            {"min", 0},
            {"max", 1024},
        },
    },
};
#include "hook.hpp"
struct height_range_t {
  short min, max;
};
HOOK(dimension_ctor_hook,
     "48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 54 41 55 41 56 41 "
     "57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 "
     "45 ? 41 8B D9 41 8B F8",
     void *, void *_this, void *a, int i, height_range_t r, void *b,
     std::string n) {
  detour->unHook();
  try {
    r.max = dimension_config[n]["max"].get<short>();
    r.min = dimension_config[n]["min"].get<short>();
  } catch (...) {
  }
  auto res = origin(_this, a, i, r, b, n);
  detour->hook();
  return res;
}
void init_dimension() {
  init_config("dimension.json", dimension_config);
  static dimension_ctor_hook hook;
  hook.detour->hook();
  LOG("dimension loaded");
}