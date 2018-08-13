nc = ncine

function execute(objects, variation_index)
	local color1 =  {r = 1.0, g = 0.0, b = 0.0}
	local color2 =  {r = 0.0, g = 1.0, b = 0.0}
	local color3 =  {r = 0.0, g = 0.0, b = 1.0}

	if variation_index == 1 then
		local temp = color1
		color1 = color2
		color2 = color3
		color3 = temp
	elseif variation_index == 2 then
		local temp = color3
		color3 = color2
		color2 = color1
		color1 = temp
	end

	color1.a = 0.0
	color2.a = 0.5
	color3.a = 1.0

	nc.particle_system.clear_affectors(objects.particlesys)
	nc.particle_system.add_color_affector(objects.particlesys, {
		{0.0, color1},
		{0.5, color2},
		{1.0, color3}
	})
	nc.particle_system.add_size_affector(objects.particlesys, 0.75, {{0.0, 1.0}, {0.4, 0.5}, {1.0, 0.2}})
end
