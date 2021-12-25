#include "gtest_rect.h"

namespace {

class RectTest : public ::testing::Test
{
  public:
	const int X = 50;
	const int Y = 100;
	const int Width = 200;
	const int Height = 75;

	RectTest()
	    : rect_(X, Y, Width, Height) {}

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
	printf("Constructing a new rectangle with the specified top-left point and size with components: ");
	printRect(newRect);

	ASSERT_EQ(newRect.x, X);
	ASSERT_EQ(newRect.y, Y);
	ASSERT_EQ(newRect.w, Width);
	ASSERT_EQ(newRect.h, Height);
}

TEST_F(RectTest, VectorsConstructor)
{
	const nc::Vector2i pos(X, Y);
	const nc::Vector2i dim(Width, Height);

	const nc::Recti newRect(pos, dim);
	printf("Constructing a new rectangle from the specified top-left point and size as vectors: ");
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

	const nc::Recti newRect = nc::Recti::fromCenterSize(cx, cy, Width, Height);
	printf("Constructing a new rectangle from center and size with components: ");
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
	const nc::Vector2i center(cx, cy);
	const nc::Vector2i dim(Width, Height);

	const nc::Recti newRect = nc::Recti::fromCenterSize(center, dim);
	printf("Constructing a new rectangle from center and size with vectors: ");
	printRect(newRect);

	ASSERT_EQ(newRect.x, X);
	ASSERT_EQ(newRect.y, Y);
	ASSERT_EQ(newRect.w, dim.x);
	ASSERT_EQ(newRect.h, dim.y);
	ASSERT_EQ(newRect.center().x, center.x);
	ASSERT_EQ(newRect.center().y, center.y);
}

TEST_F(RectTest, MinMaxComponentsConstructor)
{
	const nc::Recti newRect = nc::Recti::fromMinMax(X, Y, X + Width, Y + Height);
	printf("Constructing a new rectangle from minimum and maximum with components: ");
	printRect(newRect);

	ASSERT_EQ(newRect.x, X);
	ASSERT_EQ(newRect.y, Y);
	ASSERT_EQ(newRect.w, Width);
	ASSERT_EQ(newRect.h, Height);

	ASSERT_EQ(newRect.min().x, X);
	ASSERT_EQ(newRect.min().y, Y);
	ASSERT_EQ(newRect.max().x, X + Width);
	ASSERT_EQ(newRect.max().y, Y + Height);
}

TEST_F(RectTest, MinMaxComponentsConstructorNegativeSize)
{
	const nc::Recti newRect = nc::Recti::fromMinMax(X, Y, X - Width, Y - Height);
	printf("Constructing a new rectangle from minimum and maximum with components: ");
	printRect(newRect);

	ASSERT_EQ(newRect.x, X);
	ASSERT_EQ(newRect.y, Y);
	ASSERT_EQ(newRect.w, -Width);
	ASSERT_EQ(newRect.h, -Height);

	ASSERT_EQ(newRect.min().x, X - Width);
	ASSERT_EQ(newRect.min().y, Y - Height);
	ASSERT_EQ(newRect.max().x, X);
	ASSERT_EQ(newRect.max().y, Y);
}

TEST_F(RectTest, MinMaxVectorsConstructor)
{
	const nc::Vector2i min(X, Y);
	const nc::Vector2i max(X + Width, Y + Height);

	const nc::Recti newRect = nc::Recti::fromMinMax(min, max);
	printf("Constructing a new rectangle from minimum and maximum with vectors: ");
	printRect(newRect);

	ASSERT_EQ(newRect.x, X);
	ASSERT_EQ(newRect.y, Y);
	ASSERT_EQ(newRect.w, Width);
	ASSERT_EQ(newRect.h, Height);

	ASSERT_EQ(newRect.min().x, X);
	ASSERT_EQ(newRect.min().y, Y);
	ASSERT_EQ(newRect.max().x, X + Width);
	ASSERT_EQ(newRect.max().y, Y + Height);
}

TEST_F(RectTest, GetCenter)
{
	const nc::Recti newRect(X, Y, Width, Height);
	printf("Constructing a new rectangle with the specified top-left point and size with components: ");
	printRect(newRect);

	const nc::Vector2i center = newRect.center();
	printf("Rectangle center at: <%d, %d>", center.x, center.y);

	ASSERT_EQ(center.x, X + Width / 2);
	ASSERT_EQ(center.y, Y + Height / 2);
}

TEST_F(RectTest, GetCenterNegativeSize)
{
	const nc::Recti newRect(X, Y, -Width, -Height);
	printf("Constructing a new rectangle with the specified top-left point and size with components: ");
	printRect(newRect);

	const nc::Vector2i center = newRect.center();
	printf("Rectangle center at: <%d, %d>", center.x, center.y);

	ASSERT_EQ(center.x, X - Width / 2);
	ASSERT_EQ(center.y, Y - Height / 2);
}

TEST_F(RectTest, GetMin)
{
	const nc::Recti newRect(X, Y, Width, Height);
	printf("Constructing a new rectangle with the specified top-left point and size with components: ");
	printRect(newRect);

	const nc::Vector2i min = newRect.min();
	printf("Rectangle minimum coordinates at: <%d, %d>", min.x, min.y);

	ASSERT_LE(min.x, X);
	ASSERT_EQ(min.y, Y);
}

TEST_F(RectTest, GetMinNegativeSize)
{
	const nc::Recti newRect(X, Y, -Width, -Height);
	printf("Constructing a new rectangle with the specified top-left point and size with components: ");
	printRect(newRect);

	const nc::Vector2i min = newRect.min();
	printf("Rectangle minimum coordinates at: <%d, %d>", min.x, min.y);

	ASSERT_LE(min.x, X - Width);
	ASSERT_EQ(min.y, Y - Height);
}

TEST_F(RectTest, GetMax)
{
	const nc::Recti newRect(X, Y, Width, Height);
	printf("Constructing a new rectangle with the specified top-left point and size with components: ");
	printRect(newRect);

	const nc::Vector2i max = newRect.max();
	printf("Rectangle maximum coordinates at: <%d, %d>", max.x, max.y);

	ASSERT_LE(max.x, X + Width);
	ASSERT_EQ(max.y, Y + Height);
}

TEST_F(RectTest, GetMaxNegativeSize)
{
	const nc::Recti newRect(X, Y, -Width, -Height);
	printf("Constructing a new rectangle with the specified top-left point and size with components: ");
	printRect(newRect);

	const nc::Vector2i max = newRect.max();
	printf("Rectangle maximum coordinates at: <%d, %d>", max.x, max.y);

	ASSERT_LE(max.x, X);
	ASSERT_EQ(max.y, Y);
}

TEST_F(RectTest, SetComponents)
{
	const int diff = 10;

	rect_.set(X + diff, Y + diff, Width + diff, Height + diff);
	printf("Setting new top-left point and size with components for the rectangle: ");
	printRect(rect_);

	ASSERT_EQ(rect_.x, X + diff);
	ASSERT_EQ(rect_.y, Y + diff);
	ASSERT_EQ(rect_.w, Width + diff);
	ASSERT_EQ(rect_.h, Height + diff);
}

TEST_F(RectTest, SetVectors)
{
	const int diff = 10;
	const nc::Vector2i point(X + diff, Y + diff);
	const nc::Vector2i size(Width + diff, Height + diff);

	rect_.set(point, size);
	printf("Setting new top-left point and size for the rectangle with vectors: ");
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

TEST_F(RectTest, SetSizeComponents)
{
	const int diff = 10;

	rect_.setSize(Width + diff, Height + diff);
	printf("Setting a new size for the rectangle with components: ");
	printRect(rect_);

	ASSERT_EQ(rect_.x, X);
	ASSERT_EQ(rect_.y, Y);
	ASSERT_EQ(rect_.w, Width + diff);
	ASSERT_EQ(rect_.h, Height + diff);
}

TEST_F(RectTest, SetSizeVector)
{
	const int diff = 10;
	const nc::Vector2i newSize(Width + diff, Height + diff);

	rect_.setSize(newSize);
	printf("Setting a new size for the rectangle with a vector: ");
	printRect(rect_);

	ASSERT_EQ(rect_.x, X);
	ASSERT_EQ(rect_.y, Y);
	ASSERT_EQ(rect_.w, Width + diff);
	ASSERT_EQ(rect_.h, Height + diff);
}

TEST_F(RectTest, SetCenterSizeComponents)
{
	const int diff = 10;

	const int cx = X + (Width + diff) / 2;
	const int cy = Y + (Height + diff) / 2;
	const int tx = 10;
	const int ty = 20;

	rect_.setCenterSize(cx + tx, cy + ty, Width + diff, Height + diff);
	printf("Setting a new center and size for the rectangle with components: ");
	printRect(rect_);

	ASSERT_EQ(rect_.x, X + tx);
	ASSERT_EQ(rect_.y, Y + ty);
	ASSERT_EQ(rect_.w, Width + diff);
	ASSERT_EQ(rect_.h, Height + diff);
	ASSERT_EQ(rect_.center().x, cx + tx);
	ASSERT_EQ(rect_.center().y, cy + ty);
}

TEST_F(RectTest, SetCenterSizeVector)
{
	const int diff = 10;

	const int cx = X + (Width + diff) / 2;
	const int cy = Y + (Height + diff) / 2;
	const int tx = 10;
	const int ty = 20;
	const nc::Vector2i newCenter(cx + tx, cy + ty);
	const nc::Vector2i newSize(Width + diff, Height + diff);

	rect_.setCenterSize(newCenter, newSize);
	printf("Setting a new center and size for the rectangle with vectors: ");
	printRect(rect_);

	ASSERT_EQ(rect_.x, X + tx);
	ASSERT_EQ(rect_.y, Y + ty);
	ASSERT_EQ(rect_.w, Width + diff);
	ASSERT_EQ(rect_.h, Height + diff);
	ASSERT_EQ(rect_.center().x, newCenter.x);
	ASSERT_EQ(rect_.center().y, newCenter.y);
}

TEST_F(RectTest, SetMinMaxComponents)
{
	const int diff = 10;

	rect_.setMinMax(X + diff, Y + diff, X + Width - diff, Y + Height - diff);
	printf("Setting a new minimum and maximum for the rectangle with components: ");
	printRect(rect_);

	ASSERT_EQ(rect_.x, X + diff);
	ASSERT_EQ(rect_.y, Y + diff);
	ASSERT_EQ(rect_.w, Width - 2 * diff);
	ASSERT_EQ(rect_.h, Height - 2 * diff);

	ASSERT_EQ(rect_.min().x, X + diff);
	ASSERT_EQ(rect_.min().y, Y + diff);
	ASSERT_EQ(rect_.max().x, X + Width - diff);
	ASSERT_EQ(rect_.max().y, Y + Height - diff);
}

TEST_F(RectTest, SetMinMaxVectors)
{
	const int diff = 10;
	const nc::Vector2i min(X + diff, Y + diff);
	const nc::Vector2i max(X + Width - diff, Y + Height - diff);

	rect_.setMinMax(min, max);
	printf("Setting a new minimum and maximum for the rectangle with vectors: ");
	printRect(rect_);

	ASSERT_EQ(rect_.x, X + diff);
	ASSERT_EQ(rect_.y, Y + diff);
	ASSERT_EQ(rect_.w, Width - 2 * diff);
	ASSERT_EQ(rect_.h, Height - 2 * diff);

	ASSERT_EQ(rect_.min().x, X + diff);
	ASSERT_EQ(rect_.min().y, Y + diff);
	ASSERT_EQ(rect_.max().x, X + Width - diff);
	ASSERT_EQ(rect_.max().y, Y + Height - diff);
}

TEST_F(RectTest, InvertPositiveSize)
{
	nc::Recti newRect(X, Y, Width, Height);
	newRect.invertSize();
	printf("Inverting the rectangle size: ");
	printRect(newRect);

	ASSERT_TRUE(rect_.contains(newRect));
	ASSERT_TRUE(newRect.contains(rect_));

	ASSERT_EQ(newRect.x, X + Width);
	ASSERT_EQ(newRect.y, Y + Height);
	ASSERT_EQ(newRect.w, -Width);
	ASSERT_EQ(newRect.h, -Height);
}

TEST_F(RectTest, InvertNegativeSize)
{
	nc::Recti newRect(X + Width, Y + Height, -Width, -Height);
	newRect.invertSize();
	printf("Inverting the rectangle size: ");
	printRect(newRect);

	ASSERT_TRUE(rect_.contains(newRect));
	ASSERT_TRUE(newRect.contains(rect_));

	ASSERT_EQ(newRect.x, X);
	ASSERT_EQ(newRect.y, Y);
	ASSERT_EQ(newRect.w, Width);
	ASSERT_EQ(newRect.h, Height);
}

TEST_F(RectTest, PointContained)
{
	const int x = X + Width / 2;
	const int y = Y + Height / 2;

	printf("Checking if the <%u, %u> point is inside the rectangle ", x, y);
	printRect(rect_);

	ASSERT_TRUE(rect_.contains(x, y));
}

TEST_F(RectTest, PointOnBorder)
{
	const int x = X;
	const int y = Y;

	printf("Checking if the <%u, %u> point is inside the rectangle ", x, y);
	printRect(rect_);

	ASSERT_TRUE(rect_.contains(x, y));
}

TEST_F(RectTest, PointNotContained)
{
	const int diff = 10;
	const int x = X + Width + diff;
	const int y = Y + Height + diff;

	printf("Checking if the <%u, %u> point is inside the rectangle ", x, y);
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

TEST_F(RectTest, RectInsideNegativeSizeFirst)
{
	rect_.set(X + Width, Y + Height, -Width, -Height);
	const int diff = 5;

	const nc::Recti newRect(X + diff, Y + diff, Width - diff, Height - diff);
	printf("Constructing a new rectangle inside the first one: ");
	printRect(newRect);

	ASSERT_TRUE(rect_.contains(newRect));
	ASSERT_TRUE(newRect.overlaps(rect_));
	ASSERT_TRUE(rect_.overlaps(newRect));
}

TEST_F(RectTest, RectInsideNegativeSizeOther)
{
	const int diff = 5;

	const nc::Recti newRect(X + Width - diff, Y + Height - diff, -Width + diff, -Height + diff);
	printf("Constructing a new rectangle inside the first one: ");
	printRect(newRect);

	ASSERT_TRUE(rect_.contains(newRect));
	ASSERT_TRUE(newRect.overlaps(rect_));
	ASSERT_TRUE(rect_.overlaps(newRect));
}

TEST_F(RectTest, RectInsideNegativeSizeBoth)
{
	rect_.set(X + Width, Y + Height, -Width, -Height);
	const int diff = 5;

	const nc::Recti newRect(X + Width - diff, Y + Height - diff, -Width + diff, -Height + diff);
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

TEST_F(RectTest, RectOverlapMinMaxNotContained)
{
	const int diff = 5;

	const nc::Recti newRect(X - diff, Y - diff, Width, Height + 2 * diff);
	printf("Constructing a new rectangle that overlaps the first one: ");
	printRect(newRect);

	ASSERT_FALSE(rect_.contains(newRect));
	ASSERT_TRUE(newRect.overlaps(rect_));
	ASSERT_TRUE(rect_.overlaps(newRect));
}

TEST_F(RectTest, RectOverlapNegativeSizeFirst)
{
	rect_.set(X + Width, Y + Height, -Width, -Height);
	const int diff = 5;

	const nc::Recti newRect(X + diff, Y + diff, Width, Height);
	printf("Constructing a new rectangle that overlaps the first one: ");
	printRect(newRect);

	ASSERT_FALSE(rect_.contains(newRect));
	ASSERT_TRUE(newRect.overlaps(rect_));
	ASSERT_TRUE(rect_.overlaps(newRect));
}

TEST_F(RectTest, RectOverlapNegativeSizeOther)
{
	const int diff = 5;

	const nc::Recti newRect(X + Width - diff, Y + Height - diff, -Width, -Height);
	printf("Constructing a new rectangle that overlaps the first one: ");
	printRect(newRect);

	ASSERT_FALSE(rect_.contains(newRect));
	ASSERT_TRUE(newRect.overlaps(rect_));
	ASSERT_TRUE(rect_.overlaps(newRect));
}

TEST_F(RectTest, RectOverlapNegativeSizeBoth)
{
	rect_.set(X + Width, Y + Height, -Width, -Height);
	const int diff = 5;

	const nc::Recti newRect(X + Width - diff, Y + Height - diff, -Width, -Height);
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

TEST_F(RectTest, RectIntersectInside)
{
	const int diff = 5;

	const nc::Recti newRect(X + diff, Y + diff, Width - diff, Height - diff);
	printf("Constructing a new rectangle that intersects the first one: ");
	printRect(newRect);
	rect_.intersect(newRect);

	ASSERT_TRUE(newRect == rect_);
	ASSERT_TRUE(newRect.contains(rect_));
	ASSERT_TRUE(rect_.contains(newRect));

	ASSERT_EQ(rect_.x, X + diff);
	ASSERT_EQ(rect_.y, Y + diff);
	ASSERT_EQ(rect_.w, Width - diff);
	ASSERT_EQ(rect_.h, Height - diff);
}

TEST_F(RectTest, RectIntersectDownRight)
{
	const int diff = 5;

	const nc::Recti newRect(X + diff, Y + diff, Width, Height);
	printf("Constructing a new rectangle that intersects the first one: ");
	printRect(newRect);
	rect_.intersect(newRect);

	ASSERT_TRUE(newRect.contains(rect_));
	ASSERT_FALSE(rect_.contains(newRect));

	ASSERT_EQ(rect_.x, X + diff);
	ASSERT_EQ(rect_.y, Y + diff);
	ASSERT_EQ(rect_.w, Width - diff);
	ASSERT_EQ(rect_.h, Height - diff);
}

TEST_F(RectTest, RectIntersectUpLeft)
{
	const int diff = 5;

	const nc::Recti newRect(X - diff, Y - diff, Width, Height);
	printf("Constructing a new rectangle that intersects the first one: ");
	printRect(newRect);
	rect_.intersect(newRect);

	ASSERT_TRUE(newRect.contains(rect_));
	ASSERT_FALSE(rect_.contains(newRect));

	ASSERT_EQ(rect_.x, X);
	ASSERT_EQ(rect_.y, Y);
	ASSERT_EQ(rect_.w, Width - diff);
	ASSERT_EQ(rect_.h, Height - diff);
}

TEST_F(RectTest, RectIntersectNegativeSizeFirstTopLeft)
{
	rect_.set(X + Width, Y + Height, -Width, -Height);
	const int diff = 5;

	const nc::Recti newRect(X - diff, Y - diff, Width, Height);
	printf("Constructing a new rectangle that intersects the first one: ");
	printRect(newRect);
	rect_.intersect(newRect);

	ASSERT_TRUE(newRect.contains(rect_));
	ASSERT_FALSE(rect_.contains(newRect));

	ASSERT_EQ(rect_.x, X + Width - diff);
	ASSERT_EQ(rect_.y, Y + Height - diff);
	ASSERT_EQ(rect_.w, -Width + diff);
	ASSERT_EQ(rect_.h, -Height + diff);
}

TEST_F(RectTest, RectIntersectNegativeSizeFirstDownRight)
{
	rect_.set(X + Width, Y + Height, -Width, -Height);
	const int diff = 5;

	const nc::Recti newRect(X + diff, Y + diff, Width, Height);
	printf("Constructing a new rectangle that intersects the first one: ");
	printRect(newRect);
	rect_.intersect(newRect);

	ASSERT_TRUE(newRect.contains(rect_));
	ASSERT_FALSE(rect_.contains(newRect));

	ASSERT_EQ(rect_.x, X + Width);
	ASSERT_EQ(rect_.y, Y + Height);
	ASSERT_EQ(rect_.w, -Width + diff);
	ASSERT_EQ(rect_.h, -Height + diff);
}

TEST_F(RectTest, RectIntersectNegativeSizeOtherTopLeft)
{
	const int diff = 5;

	const nc::Recti newRect(X + Width - diff, Y + Height - diff, -Width, -Height);
	printf("Constructing a new rectangle that intersects the first one: ");
	printRect(newRect);
	rect_.intersect(newRect);

	ASSERT_TRUE(newRect.contains(rect_));
	ASSERT_FALSE(rect_.contains(newRect));

	ASSERT_EQ(rect_.x, X);
	ASSERT_EQ(rect_.y, Y);
	ASSERT_EQ(rect_.w, Width - diff);
	ASSERT_EQ(rect_.h, Height - diff);
}

TEST_F(RectTest, RectIntersectNegativeSizeOtherDownRight)
{
	const int diff = 5;

	const nc::Recti newRect(X + Width + diff, Y + Height + diff, -Width, -Height);
	printf("Constructing a new rectangle that intersects the first one: ");
	printRect(newRect);
	rect_.intersect(newRect);

	ASSERT_TRUE(newRect.contains(rect_));
	ASSERT_FALSE(rect_.contains(newRect));

	ASSERT_EQ(rect_.x, X + diff);
	ASSERT_EQ(rect_.y, Y + diff);
	ASSERT_EQ(rect_.w, Width - diff);
	ASSERT_EQ(rect_.h, Height - diff);
}

TEST_F(RectTest, RectIntersectNegativeSizeBothTopLeft)
{
	rect_.set(X + Width, Y + Height, -Width, -Height);
	const int diff = 5;

	const nc::Recti newRect(X + Width - diff, Y + Height - diff, -Width, -Height);
	printf("Constructing a new rectangle that intersects the first one: ");
	printRect(newRect);
	rect_.intersect(newRect);

	ASSERT_TRUE(newRect.contains(rect_));
	ASSERT_FALSE(rect_.contains(newRect));

	ASSERT_EQ(rect_.x, X + Width - diff);
	ASSERT_EQ(rect_.y, Y + Height - diff);
	ASSERT_EQ(rect_.w, -Width + diff);
	ASSERT_EQ(rect_.h, -Height + diff);
}

TEST_F(RectTest, RectIntersectNegativeSizeBothDownRight)
{
	rect_.set(X + Width, Y + Height, -Width, -Height);
	const int diff = 5;

	const nc::Recti newRect(X + Width + diff, Y + Height + diff, -Width, -Height);
	printf("Constructing a new rectangle that intersects the first one: ");
	printRect(newRect);
	rect_.intersect(newRect);

	ASSERT_TRUE(newRect.contains(rect_));
	ASSERT_FALSE(rect_.contains(newRect));

	ASSERT_EQ(rect_.x, X + Width);
	ASSERT_EQ(rect_.y, Y + Height);
	ASSERT_EQ(rect_.w, -Width + diff);
	ASSERT_EQ(rect_.h, -Height + diff);
}

}
