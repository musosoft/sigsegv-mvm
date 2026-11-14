#pragma once
struct TestProtection {
    void *page;
    std::size_t size;
    TestProtection(void *p, std::size_t length) {
        protected_length = length;
        auto ps = static_cast<std::uintptr_t>(sysconf(_SC_PAGESIZE));
        auto start = reinterpret_cast<std::uintptr_t>(p) & ~(ps - 1);
        page = reinterpret_cast<void *>(start);
        size = ((reinterpret_cast<std::uintptr_t>(p) + length + ps - 1) & ~(ps - 1)) - start;
        if (mprotect(page, size, PROT_READ | PROT_WRITE | PROT_EXEC) != 0) std::abort();
    }
    ~TestProtection() { if (mprotect(page, size, PROT_READ | PROT_EXEC) != 0) std::abort(); }
};
#define MemProtModifier_RX_RWX(p, n) TestProtection test_protection(p, n)
