#pragma once
struct CDetouredFunc {
    static CDetouredFunc *FindOptional(void *) { return nullptr; }
    void TemporaryDisable() {}
    void TemporaryEnable() {}
};
