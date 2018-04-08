#include "gtest_rect.h"

namespace {

class RectTest : public ::testing::Test
{
  public:
	const int X = 50;
	const int Y = 100;
	const int Width = 200;
	const int Height = 75;

	RectTest() : rect_(X, Y, Width, Height) { }

	nc::Recti rect_;
};

TEST_F(RectTest, DefaultConstructor)
{
	const nc::Recti newRect;
	printf("Constructing a new rectangle with the default constructor: ");
	printRect(newRect);

	ASSERT_EQ(newRect.x, 0);
	ASSERT_EQ(newRect.y, 0);
	ASSERT_EQ(newRect.w, 0);
	ASSERT_EQ(newRect.h, 0);
}

TEST_F(RectTest, ComponentsConstructor)
{
	const nc::Recti newRect(X, Y, Width, Height);
	printf("Constructing a new rectangle with the specified components: ");
	printRect(newRect);

	ASSERT_EQ(newRect.x, X);
	ASSERT_EQ(newRect.y, Y);
	ASSERT_EQ(newRect.w, Width);
	ASSERT_EQ(newRect.h, Height);
}

TEST_F(RectTest, VectorsConstructor)
{
	nc::Vector2i pos(X, Y);
	nc::Vector2i dim(Width, Height);

	const nc::Recti newRect(pos, dim);
	printf("Constructing a new rectangle from two vectors: ");
	printRect(newRect);

	ASSERT_EQ(newRect.x, pos.x);
	ASSERT_EQ(newRect.y, pos.y);
	ASSERT_EQ(newRect.w, dim.x);
	ASSERT_EQ(newRect.h, dim.y);
}

TEST_F(RectTest, CenterComponentsConstructor)
{
	const int cx = X + Width / 2;
	const int cy = Y + Height / 2;

	const nc::Recti newRect = nc::Recti::fromCenterAndSize(cx, cy, Width, Height);
	printf("Constructing a new rectangle from center with components: ");
	printRect(newRect);

	ASSERT_EQ(newRect.x, X);
	ASSERT_EQ(newRect.y, Y);
	ASSERT_EQ(newRect.w, Width);
	ASSERT_EQ(newRect.h, Height);
	ASSERT_EQ(newRect.center().x, cx);
	ASSERT_EQ(newRect.center().y, cy);
}

TEST_F(RectTest, CenterVectorsConstructor)
{
	const int cx = X + Width / 2;
	const int cy = Y + Height / 2;
	nc::Vector2i center(cx, cy);
	nc::Vector2i dim(Width, Height);

	const nc::Recti newRect = nc::Recti::fromCenterAndSize(center, dim);
	printf("Constructing a new rectangle from center with vectors: ");
	printRect(newRect);

	ASSERT_EQ(newRect.x, X);
	ASSERT_EQ(newRect.y, Y);
	ASSERT_EQ(newRect.w, dim.x);
	ASSERT_EQ(newRect.h, dim.y);
	ASSERT_EQ(newRect.center().x, center.x);
	ASSERT_EQ(newRect.center().y, center.y);
}

TEST_F(RectTest, SetComponents)
{
	const int diff = 10;

	rect_.set(X + diff, Y + diff, Width + diff, Height + diff);
	printf("Setting new components for the rectangle: ");
	printRect(rect_);

	ASSERT_EQ(rect_.x, X + diff);
	ASSERT_EQ(rect_.y, Y + diff);
	ASSERT_EQ(rect_.w, Width + diff);
	ASSERT_EQ(rect_.h, Height + diff);
}

TEST_F(RectTest, SetCenterComponents)
{
	const int cx = X + Width / 2;
	const int cy = Y + Height / 2;
	const int tx = 10;
	const int ty = 20;

	rect_.setCenter(cx + tx, cy + ty);
	printf("Setting a new center for the rectangle with components: ");
	printRect(rect_);

	ASSERT_EQ(rect_.x, X + tx);
	ASSERT_EQ(rect_.y, Y + ty);
	ASSERT_EQ(rect_.w, Width);
	ASSERT_EQ(rect_.h, Height);
	ASSERT_EQ(rect_.center().x, cx + tx);
	ASSERT_EQ(rect_.center().y, cy + ty);
}

TEST_F(RectTest, SetCenterVector)
{
	const int cx = X + Width / 2;
	const int cy = Y + Height / 2;
	const int tx = 10;
	const int ty = 20;
	const nc::Vector2i newCenter(cx + tx, cy + ty);

	rect_.setCenter(newCenter);
	printf("Setting a new center for the rectangle with a vector: ");
	printRect(rect_);

	ASSERT_EQ(rect_.x, X + tx);
	ASSERT_EQ(rect_.y, Y + ty);
	ASSERT_EQ(rect_.w, Width);
	ASSERT_EQ(rect_.h, Height);
	ASSERT_EQ(rect_.center().x, newCenter.x);
	ASSERT_EQ(rect_.center().y, newCenter.y);
}

TEST_F(RectTest, RectInside)
{
	const int diff = 5;

	const nc::Recti newRect(X + diff, Y + diff, Width - diff, Height - diff);
	printf("Constructing a new rectangle inside the first one: ");
	printRect(newRect);

	ASSERT_TRUE(rect_.contains(newRect));
	ASSERT_TRUE(newRect.overlaps(rect_));
	ASSERT_TRUE(rect_.overlaps(newRect));
}

TEST_F(RectTest, RectOverlap)
{
	const int diff = 5;

	const nc::Recti newRect(X + diff, Y + diff, Width, Height);
	printf("Constructing a new rectangle that overlaps the first one: ");
	printRect(newRect);

	ASSERT_FALSE(rect_.contains(newRect));
	ASSERT_TRUE(newRect.overlaps(rect_));
	ASSERT_TRUE(rect_.overlaps(newRect));
}

TEST_F(RectTest, SameRectOverlap)
{
	const nc::Recti newRect(rect_);
	printf("Constructing a new rectangle with the copy constructor: ");
	printRect(newRect);

	ASSERT_TRUE(newRect == rect_);
	ASSERT_TRUE(newRect.contains(rect_));
	ASSERT_TRUE(rect_.contains(newRect));
	ASSERT_TRUE(newRect.overlaps(rect_));
	ASSERT_TRUE(rect_.overlaps(newRect));
}

TEST_F(RectTest, RectAdjoining)
{
	const int diff = 5;

	const nc::Recti newRect(X + Width + 1, Y, Width, Height);
	printf("Constructing a new rectangle adjoining the first one: ");
	printRect(newRect);

	ASSERT_FALSE(newRect.contains(rect_));
	ASSERT_FALSE(rect_.contains(newRect));
	ASSERT_FALSE(newRect.overlaps(rect_));
	ASSERT_FALSE(rect_.overlaps(newRect));
}

TEST_F(RectTest, RectNotOverlapping)
{
	const int diff = 5;

	const nc::Recti newRect(X + Width + diff, Y + Height + diff, Width, Height);
	printf("Constructing a new rectangle that does not overlap the first one: ");
	printRect(newRect);

	ASSERT_FALSE(newRect.contains(rect_));
	ASSERT_FALSE(rect_.contains(newRect));
	ASSERT_FALSE(newRect.overlaps(rect_));
	ASSERT_FALSE(rect_.overlaps(newRect));
}

TEST_F(RectTest, PointContained)
{
	const int x = X + Width / 2;
	const int y = Y + Height / 2;

	printf("Checking if <%u, %u> is inside the rectangle ", x, y);
	printRect(rect_);

	ASSERT_TRUE(rect_.contains(x, y));
}

TEST_F(RectTest, PointOnBorder)
{
	const int x = X;
	const int y = Y;

	printf("Checking if <%u, %u> is inside the rectangle ", x, y);
	printRect(rect_);

	ASSERT_TRUE(rect_.contains(x, y));
}

TEST_F(RectTest, PointNotContained)
{
	const int diff = 10;
	const int x = X + Width + diff;
	const int y = Y + Height + diff;

	printf("Checking if <%u, %u> is inside the rectangle ", x, y);
	printRect(rect_);

	ASSERT_FALSE(rect_.contains(x, y));
}

TEST_F(RectTest, VectorPointContained)
{
	const int x = X + Width / 2;
	const int y = Y + Height / 2;
	const nc::Vector2i p(x, y);

	printf("Checking if <%u, %u> is inside the rectangle ", p.x, p.y);
	printRect(rect_);

	ASSERT_TRUE(rect_.contains(p));
}

}
