#pragma once
#include "libhat.hpp" // IWYU pragma: keep
#include "path.hpp"
#include "polyhook2/Detour/x64Detour.hpp" // IWYU pragma: keep

#define HOOK(NAME, SIG, RT, ...)                                               \
  struct NAME {                                                                \
    static inline PLH::x64Detour *detour = nullptr;                            \
    static inline uintptr_t tramp{};                                           \
    static inline RT (*origin)(__VA_ARGS__) = nullptr;                         \
    static RT hooked(__VA_ARGS__);                                             \
    NAME() {                                                                   \
      auto md = hat::process::get_module(get_main_exe_filename())              \
                    ->get_module_data();                                       \
      origin = reinterpret_cast<decltype(origin)>(                             \
          hat::find_pattern(md.begin(), md.end(),                              \
                            hat::compile_signature<SIG>())                     \
              .get());                                                         \
      detour =                                                                 \
          new PLH::x64Detour(reinterpret_cast<uintptr_t>(origin),              \
                             reinterpret_cast<uintptr_t>(hooked), &tramp);     \
    }                                                                          \
    ~NAME() {                                                                  \
      if (detour->isHooked()) {                                                \
        detour->unHook();                                                      \
      }                                                                        \
      delete detour;                                                           \
    }                                                                          \
  };                                                                           \
  RT NAME::hooked(__VA_ARGS__)

template <typename T> inline T sig2addr(hat::signature_view view) {
  auto md =
      hat::process::get_module(get_main_exe_filename())->get_module_data();
  auto res = hat::find_pattern(md.begin(), md.end(), view);
  return reinterpret_cast<T>(res.get());
}