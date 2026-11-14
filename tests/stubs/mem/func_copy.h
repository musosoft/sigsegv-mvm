#pragma once
// The x64 branch must never try to relocate a replacement body.
template<class... A> std::size_t CopyAndFixUpFuncBytes(A...) { std::abort(); }
