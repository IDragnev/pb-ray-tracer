#include "doctest/doctest.h"
#include "pbrt/memory/MemoryArena.hpp"

#include <algorithm>

namespace mem = idragnev::pbrt::memory;

bool areAllZeros(const int* const arr, const std::size_t size) {
    return std::all_of(arr, arr + size, [](const int i) { return i == 0; });
}

TEST_CASE("alloc<T> uses zero initialization by default") {
    mem::MemoryArena arena;

    const auto size = 10;

    int* const arr = arena.alloc<int>(size);

    CHECK(areAllZeros(arr, size));
}

TEST_CASE("allocation bigger than the block size") {
    const auto blockSize = 1024u;
    mem::MemoryArena arena(blockSize);

    const auto size = blockSize + 10;

    int* const arr = arena.alloc<int>(size);

    CHECK(areAllZeros(arr, size));
}

TEST_CASE("multiple allocations") {
    mem::MemoryArena arena;

    const auto size = 100;

    int* const arr1 = arena.alloc<int>(size);
    int* const arr2 = arena.alloc<int>(size);
    int* const arr3 = arena.alloc<int>(size);

    CHECK(areAllZeros(arr1, size));
    CHECK(areAllZeros(arr2, size));
    CHECK(areAllZeros(arr3, size));
}

TEST_CASE("reset does not free the allocated memory "
          "but simply reuses it") {
    mem::MemoryArena arena;
    [[maybe_unused]] void* const arr = arena.alloc(10);

    const auto allocSize = arena.totalAllocationSize();
    arena.reset();

    CHECK(arena.totalAllocationSize() == allocSize);
}