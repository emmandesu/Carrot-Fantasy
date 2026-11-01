#pragma once
#include <cstdio>
#include <string>
#if defined(_WIN32)
#include <io.h>
#else
#include <unistd.h>
#endif

namespace SaveIO {
// All game saves are written on the GL thread; a temp file prevents partial JSON.
inline bool writeJSON(const std::string& path, const std::string& json) {
    const auto temporary = path + ".tmp";
    auto file = std::fopen(temporary.c_str(), "wb");
    if (!file) return false;
    bool ok = std::fwrite(json.data(), 1, json.size(), file) == json.size();
    ok = std::fflush(file) == 0 && ok;
#if defined(_WIN32)
    ok = _commit(_fileno(file)) == 0 && ok;
#else
    ok = fsync(fileno(file)) == 0 && ok;
#endif
    ok = std::fclose(file) == 0 && ok;
#if defined(_WIN32)
    // std::rename cannot replace an existing Windows destination.
    if (ok) std::remove(path.c_str());
#endif
    if (ok) ok = std::rename(temporary.c_str(), path.c_str()) == 0;
    if (!ok) std::remove(temporary.c_str());
    return ok;
}
}
