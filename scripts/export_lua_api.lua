#!/usr/bin/env lua

if ncine == nil then
	ncine = require "libncine"
end

nc = ncine

filename = arg[1] and arg[1] or "nCine_Lua_API.adoc"

function add_entry(t, entry, name)
	if type(entry) == "function" then
		table.insert(t, "=== " .. name .. "()")
	else
		table.insert(t, "* `" .. name .. "`: " .. tostring(entry))
	end
end

l1_keys = {}
doc_lines = {}


for key in pairs(nc) do
	table.insert(l1_keys, key)
end

table.sort(l1_keys)

table.insert(doc_lines, [[
:nofooter:
:toc: left
:toclevels: 3

= nCine Lua API
]])

for key, l1val in pairs(l1_keys) do
	if type(nc[l1val]) == "table" then
		table.insert(doc_lines, "\n")
		table.insert(doc_lines, "== " .. l1val .. "\n")

		l2_keys = {}
		for key in pairs(nc[l1val]) do
			table.insert(l2_keys, key)
		end
		table.sort(l2_keys)

		for key, l2val in pairs(l2_keys) do
			add_entry(doc_lines, nc[l1val][l2val], l2val)
		end
	else	
		add_entry(doc_lines, nc[l1val], l1val)
	end
end

doc = table.concat(doc_lines)

file = io.open(filename, "w")
io.output(file)
for key, line in pairs(doc_lines) do
	io.write(line .. "\n")
end
io.close(file)
