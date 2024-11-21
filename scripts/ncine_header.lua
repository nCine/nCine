if ncine == nil then
	local names = { "ncine", "ncine_d", "libncine", "libncine_d" }
	for i, name in ipairs(names) do
		local ok, ncine = pcall(require, name)
		if ok then
			break
		end
	end
	needs_start = true
end

if ncine == nil then
	error("nCine library cannot be found!")
end
