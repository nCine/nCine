#!/usr/bin/env lua

require("ncine_header")
nc = ncine

tolerance = 0.01

function print_color(c)
	return "<" .. c.r .. ", " .. c.g .. ", " .. c.b .. ", " .. c.a .. ">"
end

function assert_float(a, b, tolerance)
	assert((b - a > -tolerance) and (b - a < tolerance))
end


function assert_color(c, r, g, b, a, tolerance)
	assert_float(c.r, r, tolerance)
	assert_float(c.g, g, tolerance)
	assert_float(c.b, b, tolerance)
	assert_float(c.a, a, tolerance)
end

function test_color()
	col1 = { r = 1.0, g = 0.5, b = 1.0, a = 1.0 }
	print("First color: " .. print_color(col1))
	col2 = { r = 0.5, g = 1.0, b = 0.25, a = 1.0 }
	print("Second color: " .. print_color(col2))

	col_mul = nc.color.mul(col1, col2)
	print("Color multiplication: " .. print_color(col_mul))
	assert_color(col_mul, 0.5, 0.5, 0.25, 1.0, tolerance)
end

test_color()
