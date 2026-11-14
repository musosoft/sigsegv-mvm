#pragma once
#include "util/buf.h"
enum class ScanDir { FORWARD };
enum class ScanResults { ALL };
template<ScanDir, ScanResults, int> struct CMaskedScanner {};
struct CAddrAddrBounds {
    unsigned char *begin, *end;
    CAddrAddrBounds(void *a, void *b) : begin(static_cast<unsigned char *>(a)), end(static_cast<unsigned char *>(b)) {}
};
template<class T> class CScan {
    std::vector<unsigned char *> matches;
public:
    CScan(CAddrAddrBounds bounds, const ByteBuf& bytes, const ByteBuf& mask) {
        for (auto *p = bounds.begin; p + bytes.GetSize() <= bounds.end; ++p) {
            bool match = true;
            for (std::size_t i = 0; i < bytes.GetSize(); ++i)
                if ((p[i] & mask[i]) != (bytes[i] & mask[i])) match = false;
            if (match) matches.push_back(p);
        }
    }
    bool ExactlyOneMatch() const { return matches.size() == 1; }
    const auto& Matches() const { return matches; }
    auto FirstMatch() const { return matches.front(); }
};
