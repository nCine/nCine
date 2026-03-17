#ifndef GTEST_HASH_FUNCTIONS_H
#define GTEST_HASH_FUNCTIONS_H

#include <nctl/HashFunctions.h>
#include <nctl/HashSet.h>
#include "gtest/gtest.h"

namespace nc = ncine;

namespace {

const char *Text = "The quick brown fox jumps over the lazy dog";
const unsigned int TextLength = 43;

const char *SmallTextA = "Hello";
const unsigned int SmallTextALength = 5;

const char *SmallTextB = "World";
const unsigned int SmallTextBLength = 5;

const char ShortBuffer[16] = "abcdefg";

const unsigned int CollisionCount = 10000;

}

#endif
