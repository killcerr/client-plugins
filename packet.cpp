#define _AMD64_
#define NOMINMAX

#include "hook.hpp"
#include "log.hpp"

#include <map>
#include <memory>
#include <string>

#include "windef.h" // IWYU pragma: keep

#include "processthreadsapi.h"
#include "stringapiset.h"
#include "winbase.h"
#include "winerror.h"
#include "winnt.h"

extern nlohmann::json dimension_config;

class packet_t {
public:
  virtual ~packet_t() = 0;                    // 0
  virtual int get_id() = 0;                   // 1
  virtual std::string get_name() = 0;         // 2
  virtual void unk1() = 0;                    // 3
  virtual void unk2() = 0;                    // 4
  virtual void unk3() = 0;                    // 5
  virtual bool disallow_batching() const = 0; // 6
  virtual bool is_valid() const = 0;          // 7
  virtual void unk4() = 0;                    // 8
  char filler[0x28];
};
class dim_def_packet_t : public packet_t {
public:
  virtual ~dim_def_packet_t() = 0;

  struct def_t {
    int min;
    int max;
    int type;
  };

  std::map<std::string, def_t> defs;
};
std::shared_ptr<packet_t> (*create_packet)(int);
std::string wstr2str(std::wstring_view wstr, unsigned int codePage) {
  int len = WideCharToMultiByte(codePage, 0, wstr.data(), (int)wstr.size(),
                                nullptr, 0, nullptr, nullptr);
  std::string str;
  if (len == 0)
    return str;
  str.resize(len);
  WideCharToMultiByte(codePage, 0, wstr.data(), (int)wstr.size(), str.data(),
                      (int)str.size(), nullptr, nullptr);
  return str;
}

HOOK(network_system_send,
     "40 53 55 56 57 41 54 41 56 41 57 48 81 EC ? ? ? ? 48 8B 05 ? ? ? "
     "? 48 33 C4 48 89 84 24 ? ? ? ? 45 0F B6 E1 4D 8B F8",
     void, void *_this, void *id, packet_t *pkt, unsigned char sub) {
  static std::mutex mutex;
  std::lock_guard lock{mutex};
  detour->unHook();
  PWSTR res;
  HRESULT hr = GetThreadDescription(GetCurrentThread(), &res);
  if (SUCCEEDED(hr)) {
    LOG("thread:{}", wstr2str({res}, CP_ACP));
    LocalFree(res);
  }
  LOG("packet:{}", pkt->get_name());
  if (pkt->get_name() == "StartGamePacket") {
    auto dim_pkt = create_packet(180);
    auto dim_raw = (dim_def_packet_t *)dim_pkt.get();
    dim_raw->defs["minecraft:overworld"].max =
        std::min(dimension_config["Overworld"]["max"].get<int>(), 512);
    dim_raw->defs["minecraft:overworld"].min =
        std::max(dimension_config["Overworld"]["min"].get<int>(), -512);
    dim_raw->defs["minecraft:overworld"].type = 5;
    dim_raw->defs["minecraft:nether"].max =
        dimension_config["Nether"]["max"].get<int>();
    dim_raw->defs["minecraft:nether"].min =
        dimension_config["Nether"]["min"].get<int>();
    dim_raw->defs["minecraft:the_end"].max =
        dimension_config["TheEnd"]["max"].get<int>();
    dim_raw->defs["minecraft:the_end"].min =
        dimension_config["TheEnd"]["min"].get<int>();
    origin(_this, id, dim_pkt.get(), sub);
    origin(_this, id, pkt, sub);
  } else {
    origin(_this, id, pkt, sub);
  }
  detour->hook();
}

void init_packet() {
  create_packet = sig2addr<decltype(create_packet)>(
      hat::compile_signature<"48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B "
                             "05 ? ? ? ? 48 33 "
                             "C4 48 89 44 24 ? 48 8B D9 48 89 4C 24 ? 33 "
                             "F6">());
  static network_system_send hook;
  hook.detour->hook();
  LOG("packet loaded");
}