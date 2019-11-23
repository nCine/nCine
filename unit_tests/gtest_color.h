#ifndef GTEST_COLOR_H
#define GTEST_COLOR_H

#include <ncine/Color.h>
#include <ncine/Colorf.h>
#include <ncine/ColorHdr.h>
#include "gtest/gtest.h"

namespace nc = ncine;

namespace {

void printColor(const nc::Color &color)
{
	printf("<%u, %u, %u, %u>\n", color.r(), color.g(), color.b(), color.a());
}

void printColor(const char *message, const nc::Color &color)
{
	printf("%s ", message);
	printColor(color);
}

void printColor(const nc::Colorf &color)
{
	printf("<%.2f, %.2f, %.2f, %.2f>\n", color.r(), color.g(), color.b(), color.a());
}

void printColor(const char *message, const nc::Colorf &color)
{
	printf("%s ", message);
	printColor(color);
}

void printColor(const nc::ColorHdr &color)
{
	printf("<%.2f, %.2f, %.2f>\n", color.r(), color.g(), color.b());
}

void printColor(const char *message, const nc::ColorHdr &color)
{
	printf("%s ", message);
	printColor(color);
}

}

#endif
