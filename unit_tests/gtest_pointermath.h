#ifndef GTEST_POINTERMATH_H
#define GTEST_POINTERMATH_H

#include <nctl/PointerMath.h>
#include "gtest/gtest.h"

namespace {

const size_t Address = 0x100;
const size_t Value = 0x10;
const size_t Alignment = 16;
const size_t Disalign = 5;

const size_t HeaderSize = 8;
const size_t SmallHeaderSize = 4;

}

#endif
