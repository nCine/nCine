#!/usr/bin/env lua

if ncine == nil then
	ncine = require "libncine"
end

nc = ncine

tolerance = 0.00001

function print_vec4(v)
	return "<" .. v.x .. ", " .. v.y .. ", " .. v.z .. ", " .. v.w ..">"
end

function assert_float(a, b, tolerance)
	assert((b - a > -tolerance) and (b - a < tolerance))
end

function assert_vec4(v, x, y, z, w, tolerance)
	assert_float(v.x, x, tolerance)
	assert_float(v.y, y, tolerance)
	assert_float(v.z, z, tolerance)
	assert_float(v.w, w, tolerance)
end

function test_vec4()
	vec1 = { x = 1.0, y = 2.0, z = 4.0, w = 6.0 }
	print("First vector: " .. print_vec4(vec1))
	vec2 = { x = 0.5, y = 0.5, z = 1.0, w = 2.0 }
	print("Second vector: " .. print_vec4(vec2))

	vec_add = nc.vec4.add(vec1, vec2)
	print("Vector addition: " .. print_vec4(vec_add))
	assert_vec4(vec_add, 1.5, 2.5, 5.0, 8.0, tolerance)

	vec_sub = nc.vec4.sub(vec1, vec2)
	print("Vector subtraction: " .. print_vec4(vec_sub))
	assert_vec4(vec_sub, 0.5, 1.5, 3.0, 4.0, tolerance)

	vec_mul = nc.vec4.mul(vec1, vec2)
	print("Vector multiplication: " .. print_vec4(vec_mul))
	assert_vec4(vec_mul, 0.5, 1.0, 4.0, 12.0, tolerance)

	vec_div = nc.vec4.div(vec1, vec2)
	print("Vector division: " .. print_vec4(vec_div))
	assert_vec4(vec_div, 2.0, 4.0, 4.0, 3.0, tolerance)

	vec1_unit = nc.vec4.normalized(vec1)
	print("First vector normalized: " .. print_vec4(vec1_unit))

	vec_len = nc.vec4.length(vec1_unit)
	print("Length of normalized vector: " .. vec_len)
	assert_float(vec_len, 1.0, tolerance)

	vec_sqrlen = nc.vec4.sqrlength(vec1_unit)
	print("Squared length of normalized vector: " .. vec_sqrlen)
	assert_float(vec_sqrlen, 1.0, tolerance)
end

test_vec4()


