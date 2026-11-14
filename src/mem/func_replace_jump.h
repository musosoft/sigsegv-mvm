#ifndef SIGSEGV_FUNC_REPLACE_JUMP_H
#define SIGSEGV_FUNC_REPLACE_JUMP_H

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace FuncReplaceJump {
constexpr std::size_t Size = 14;

// jmp qword ptr [rip + 0]; the inline address is data, not relocated code.
// Unlike movabs/jmp this tail jump preserves every register and RFLAGS.
inline bool Encode(void *buffer, std::size_t capacity, std::uint64_t callback)
{
    if (buffer == nullptr || capacity < Size) return false;
    const std::uint8_t opcode[] = {0xff, 0x25, 0, 0, 0, 0};
    auto *bytes = static_cast<std::uint8_t *>(buffer);
    std::memcpy(bytes, opcode, sizeof(opcode));
    // Explicit little endian; safe for unaligned buffers.
    for (std::size_t i = 0; i < 8; ++i) bytes[6 + i] = callback >> (8 * i);
    return true;
}
}
#endif
