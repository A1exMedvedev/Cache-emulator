#pragma once

#include <iostream>

static constexpr size_t MEMORY_SIZE = 1 << 17; // 2 ^ ADDRESS_LEN
static constexpr size_t ADDRESS_LEN = 17; // given in the condition
static constexpr size_t CACHE_TAG_LEN = 17 - 4 - 6; // ADDRESS_LEN - CACHE_INDEX_LEN - CACHE_OFFSET_LEN
static constexpr size_t CACHE_INDEX_LEN = 4; // given in the condition
static constexpr size_t CACHE_OFFSET_LEN = 6; // log(CACHE_LINE_SIZE)
static constexpr size_t CACHE_SIZE = 64  * 64; // CACHE_LINE_COUNT * CACHE_LINE_SIZE
static constexpr size_t CACHE_LINE_SIZE = 64; // given in the condition
static constexpr size_t CACHE_LINE_COUNT = 64; // given in the condition
static constexpr size_t CACHE_SET_COUNT = 1 << 4; // 2 ^ CACHE_INDEX_LEN
static constexpr size_t CACHE_WAY = 64 / 16; // CACHE_LINE_COUNT / CACHE_SET_COUNT
