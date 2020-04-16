#ifndef GTEST_POINTERMATH_H
#define GTEST_POINTERMATH_H

#include <nctl/PointerMath.h>
#include "gtest/gtest.h"

namespace {

const size_t Address = 0x100;
const size_t Value = 0x10;
const uint8_t Alignment = 16;
const uint8_t Disalign = 5;

const uint8_t HeaderSize = 8;
const uint8_t SmallHeaderSize = 4;

}

#endif
