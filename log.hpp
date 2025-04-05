#pragma once
#include "fmt/printf.h" // IWYU pragma: keep

#define LOG(F, ...) fmt::println(F, __VA_ARGS__)