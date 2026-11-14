#include "mem/func_replace_jump.h"
#include "mem/patch.h"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <sys/mman.h>
#include <unistd.h>

#define CHECK(x) do { if (!(x)) { std::fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); std::abort(); } } while (0)
extern "C" {
std::uint64_t observed[17];
std::uint64_t expected_rsp;
void probe(void *);
}

static void encoding()
{
    for (std::size_t offset = 0; offset < 16; ++offset) {
        std::array<unsigned char, 64> bytes;
        for (std::size_t capacity = 0; capacity < 14; ++capacity) {
            bytes.fill(0xa5);
            CHECK(!FuncReplaceJump::Encode(bytes.data() + offset, capacity, 0));
            for (auto byte : bytes) CHECK(byte == 0xa5);
        }
        for (auto address : {0ULL, 0x0123456789abcdefULL, 0xffffffffffffffffULL}) {
            bytes.fill(0xa5);
            CHECK(FuncReplaceJump::Encode(bytes.data() + offset, 14, address));
            const unsigned char prefix[] = {0xff, 0x25, 0, 0, 0, 0};
            CHECK(std::memcmp(bytes.data() + offset, prefix, 6) == 0);
            for (unsigned i = 0; i < 8; ++i) CHECK(bytes[offset + 6 + i] == ((address >> (8 * i)) & 255));
            for (std::size_t i = 0; i < bytes.size(); ++i)
                if (i < offset || i >= offset + 14) CHECK(bytes[i] == 0xa5);
        }
    }
    CHECK(!FuncReplaceJump::Encode(nullptr, 14, 0));
}

static void distant(std::uintptr_t source, std::uintptr_t destination)
{
    const auto page = static_cast<std::size_t>(sysconf(_SC_PAGESIZE));
    auto map = [page](std::uintptr_t address) {
        void *p = mmap(reinterpret_cast<void *>(address), page, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0);
        CHECK(p == reinterpret_cast<void *>(address));
        return static_cast<unsigned char *>(p);
    };
    auto *entry = map(source);
    auto *callback = map(destination);
    std::memset(entry, 0xcc, page);
    entry[0] = 0xc3;
    std::array<unsigned char, 64> original;
    std::memcpy(original.data(), entry, original.size());
    // A callback with no prologue: ret. Any state change is from the jump.
    callback[0] = 0xc3;
    CHECK(mprotect(callback, page, PROT_READ | PROT_EXEC) == 0);
    test_target = entry;
    // Deliberately enormous declared callback length: x64 must not copy/read it.
    CFuncReplace patch(1ULL << 30, callback, "test target");
    CHECK(patch.GetLength() == 14);
    CHECK(patch.Init());
    CHECK(patch.Check());
    CHECK(mprotect(entry, page, PROT_READ | PROT_EXEC) == 0);
    patch.Apply();
    CHECK(patch.IsApplied());
    CHECK(protected_length == 14);
    std::array<unsigned char, 14> expected;
    CHECK(FuncReplaceJump::Encode(expected.data(), expected.size(), destination));
    CHECK(std::memcmp(entry, expected.data(), expected.size()) == 0);
    patch.Apply(); // idempotent: must not replace the saved original bytes
    CHECK(std::memcmp(entry + 14, original.data() + 14, original.size() - 14) == 0);
    CHECK(mprotect(entry, page, PROT_READ | PROT_EXEC) == 0);
    probe(entry);
    for (unsigned i = 0; i < 15; ++i) CHECK(observed[i] == i + 1);
    CHECK(observed[15] == expected_rsp);
    CHECK((observed[16] & 0xcd5) == (0x246 & 0xcd5));
    patch.UnApply();
    CHECK(!patch.IsApplied());
    CHECK(protected_length == 14);
    patch.UnApply();
    CHECK(std::memcmp(entry, original.data(), original.size()) == 0);
    CHECK(mprotect(entry, page, PROT_READ | PROT_EXEC) == 0);
    // Assembly call: generated machine code has no Clang UBSan function-type
    // metadata preceding its entry point.
    probe(entry);
    CHECK(munmap(entry, page) == 0);
    CHECK(munmap(callback, page) == 0);
}

int main()
{
    encoding();
    // ASan HighMem on Linux x86_64, outside its reserved shadow/gap.
    constexpr std::uintptr_t base = 0x200000000000ULL;
    for (auto gap : {3ULL << 30, 32ULL << 40}) {
        distant(base, base + gap);
        distant(base + gap, base);
    }
    std::puts("PASS encoding, unaligned/bounds, 3 GiB/32 TiB both directions, GPR/flags/stack, byte restore");
}
