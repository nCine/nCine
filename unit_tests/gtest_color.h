#ifndef GTEST_COLOR_H
#define GTEST_COLOR_H

#include <ncine/Color.h>
#include <ncine/Colorf.h>
#include "gtest/gtest.h"

namespace nc = ncine;

namespace {

void printColor(const nc::Color &col)
{
	printf("<%u, %u, %u, %u>\n", col.r(), col.g(), col.b(), col.a());
}

void printColor(const nc::Colorf &col)
{
	printf("<%.2f, %.2f, %.2f, %.2f>\n", col.r(), col.g(), col.b(), col.a());
}

}

#endif
