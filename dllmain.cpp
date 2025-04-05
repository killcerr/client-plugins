#define _AMD64_
#define _CRT_SECURE_NO_WARNINGS
#include <consoleapi.h>
#include <consoleapi2.h>
#include <cstdio>
#include <fcntl.h>
#include <io.h>
#include <processEnv.h>

void init_console() {
  AllocConsole();
  SetConsoleCP(65001);
  freopen("CONOUT$", "w", stdout);
  freopen("CONOUT$", "w", stderr);
  freopen("CONIN$", "r", stdin);
  _setmode(_fileno(stdout), _O_U8TEXT);
  _setmode(_fileno(stderr), _O_U8TEXT);
  _setmode(_fileno(stdin), _O_U8TEXT);
  HANDLE handle;
  DWORD mode;
  handle = GetStdHandle(((DWORD)-10));
  GetConsoleMode(handle, &mode);
  SetConsoleMode(handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
  handle = GetStdHandle(((DWORD)-11));
  GetConsoleMode(handle, &mode);
  SetConsoleMode(handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
  handle = GetStdHandle(((DWORD)-12));
  GetConsoleMode(handle, &mode);
  SetConsoleMode(handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

#include "log.hpp"
void init_packet();
void init_dimension();
void init() {
  init_console();
  LOG("dll loaded");
  init_dimension();
  init_packet();
}
void deinit() {}
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
  switch (fdwReason) {
  case DLL_PROCESS_ATTACH:
    init();
    break;

  case DLL_PROCESS_DETACH:
    if (lpvReserved != nullptr) {
      break;
    }
    deinit();
    break;
  }
  return TRUE;
}