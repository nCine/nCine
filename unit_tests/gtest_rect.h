#ifndef GTEST_RECT_H
#define GTEST_RECT_H

#include "Rect.h"
#include "gtest/gtest.h"

namespace nc = ncine;

namespace {

void printRect(const nc::Recti &rect)
{
	printf("<%d, %d> - <%d, %d> (w: %d, h: %d)\n",
	       rect.x, rect.y, rect.x + rect.w, rect.y + rect.h, rect.w, rect.h);
}

void printRect(const nc::Rectf &rect)
{
	printf("<%.2f, %.2f> - <%.2f, %.2f> (w: %.2f, h: %.2f)\n",
	       rect.x, rect.y, rect.x + rect.w, rect.y + rect.h, rect.w, rect.h);
}

}

#endif
