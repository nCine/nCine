#!/usr/bin/env lua

if ncine == nil then
	ncine = require "libncine"
end

nc = ncine

filename = arg[1] and arg[1] or "nCine_Lua_API.adoc"
doc_lines = {}

function indent(level)
	str = ""
	for i=1, level do
		str = str .. "="
	end
	return str
end

function add_entry(t, entry, name, level)
	if type(entry) == "function" then
		table.insert(t, indent(level) .. " " .. name .. "()")
	else
		table.insert(t, "* `" .. name .. "`: " .. tostring(entry))
	end
end

function recursive_scan(t, doc, level)
	tkeys = {}
	for key in pairs(t) do
		table.insert(tkeys, key)
	end
	table.sort(tkeys)

	for key, val in pairs(tkeys) do
		if type(t[val]) == "table" then
			table.insert(doc, "\n")
			table.insert(doc, indent(level) .. " " .. val .. "\n")

			recursive_scan(t[val], doc, level + 1)
		else
			add_entry(doc, t[val], val, level)
		end
	end
end

table.insert(doc_lines, [[
:nofooter:
:toc: left
:toclevels: 3

= nCine Lua API
]])

recursive_scan(nc, doc_lines, 2)
doc = table.concat(doc_lines)

file = io.open(filename, "w")
io.output(file)
for key, line in pairs(doc_lines) do
	io.write(line .. "\n")
end
io.close(file)
