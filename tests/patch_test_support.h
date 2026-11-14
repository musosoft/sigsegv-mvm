// Minimal engine boundary doubles. CPatch/CFuncReplace and ByteBuf remain real.
#pragma once
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <sys/mman.h>
#include <unistd.h>
inline void DevMsg(const char *, ...) {}
inline void DevWarning(const char *, ...) {}
struct Color { Color(int, int, int) {} };
inline void ConColorMsg(Color, const char *, ...) {}
template<int N> struct CFmtStrN {
    char text[N]{};
    template<class... A> CFmtStrN(const char *fmt, A... args) { std::snprintf(text, N, fmt, args...); }
    operator const char *() const { return text; }
};
template<class T> class AutoList {
    static inline std::vector<AutoList *> entries;
public:
    AutoList() { entries.push_back(this); }
    ~AutoList() { entries.erase(std::find(entries.begin(), entries.end(), this)); }
    static std::vector<T *> List() {
        std::vector<T *> result;
        for (auto *p : entries) result.push_back(static_cast<T *>(p));
        return result;
    }
};
inline void *test_target = nullptr;
inline std::size_t protected_length = 0;
