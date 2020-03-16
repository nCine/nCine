#!/usr/bin/env lua

if ncine == nil then
	ncine = require "libncine"
	needs_start = true
end

nc = ncine

function ncine.on_pre_init(cfg)
	cfg.resolution = {x = 1280, y = 720}
	cfg.window_title = "nCine Lua test"

	--cfg.console_log_level = nc.log_level.OFF
	return cfg
end

function ncine.on_init()

	if nc.ANDROID then
		font_tex_file = "DroidSans32_256_ETC2.ktx"
		texture_file = "texture2_ETC2.ktx"
		texture2_file = "texture3_ETC2.ktx"
		texture3_file = "smoke_256_ETC2.ktx"
	else
		font_tex_file = "DroidSans32_256.png"
		texture_file = "texture2.png"
		texture2_file = "texture3.png"
		texture3_file = "smoke_256.png"
	end

	local rootnode = nc.application.rootnode()

	local screen = nc.application.screen_dimensions()
	pos_ = {x = screen.x * 0.5, y = screen.y * 0.5}
	angle_ = 0

	font_ = nc.font.new(nc.fs.get_datapath().."fonts/DroidSans32_256.fnt",
	                    nc.fs.get_datapath().."fonts/"..font_tex_file)
	textnode_ = nc.textnode.new(rootnode, font_, 256)
	nc.textnode.set_string(textnode_, "apptest_lua")
	nc.textnode.set_layer(textnode_, 100)
	nc.textnode.set_position(textnode_, screen.x * 0.25, screen.y * 0.25)
	nc.textnode.set_color(textnode_, 1.0, 0.0, 0.0, 1.0)

	local color = nc.textnode.get_color(textnode_)
	color.b = 1.0
	nc.textnode.set_color(textnode_, color)

	texture_ = nc.texture.new(nc.fs.get_datapath().."textures/"..texture_file)
	texture2_ = nc.texture.new(nc.fs.get_datapath().."textures/"..texture2_file)
	texture3_ = nc.texture.new(nc.fs.get_datapath().."textures/"..texture3_file)

	sprite_ = nc.sprite.new(rootnode, texture2_, screen.x * 0.2, screen.y * 0.5)
	meshsprite_ = nc.mesh_sprite.new(rootnode, texture_, screen.x * 0.8, screen.y * 0.5)
	nc.mesh_sprite.create_vertices_from_texels(
		meshsprite_, {{x = 0, y = 0}, {x = 128, y = 0}, {x = 0, y = 128}, {x = 128, y = 128}}, nc.texture_cut_mode.RESIZE
	)

	local tex_rect3 = {x = 0, y = 0, w = nc.texture.get_width(texture3_), h = nc.texture.get_height(texture3_)}
	particlesys_ = nc.particle_system.new(rootnode, 128, texture3_, tex_rect3)
	nc.particle_system.set_position(particlesys_, screen.x * 0.5, screen.y * 0.5)
	nc.particle_system.add_color_affector(particlesys_, {
		{0.0, {r = 1.0, g = 0.0, b = 0.0, a = 1.0}},
		{0.5, {r = 0.0, g = 1.0, b = 0.0, a = 0.5}},
		{1.0, {r = 0.0, g = 0.0, b = 1.0, a = 0.0}}
	})
	nc.particle_system.add_size_affector(particlesys_, 0.5, {{0.0, 0.25}, {0.45, 0.35}, {1.0, 0.75}})

	local settings = nc.application.get_rendering_settings()
	print("Rendering settings")
	for key, value in pairs(settings) do
		print("  "..key .. ": " .. tostring(value))
	end

	nc.log.info("on_init() Lua function terminated")
end

function ncine.on_frame_start()
	angle_ = angle_ + 100 * nc.application.interval()

	local newpos = {x = 0, y = 0}
	newpos.x = pos_.x + 200 * math.sin(math.rad(angle_))
	newpos.y = pos_.y + 100 * math.sin(math.rad(angle_ + 90))

	nc.sprite.set_position(meshsprite_, newpos.x, newpos.y)
	nc.sprite.set_position(meshsprite_, newpos)

	local initParticles = {amount = 16, life = {0.25, 0.35},
	                       position = {{x = -5, y = -5}, {x = 5, y = 5}},
	                       velocity = {{x = -10, y = 200}, {x = 10, y = 200}}}
	nc.particle_system.emit_particles(particlesys_, initParticles)
end

function ncine.on_shutdown()
	nc.particle_system.delete(particlesys_)

	nc.mesh_sprite.delete(meshsprite_)
	nc.sprite.delete(sprite_)

	nc.texture.delete(texture3_)
	nc.texture.delete(texture2_)
	nc.texture.delete(texture_)

	nc.textnode.delete(textnode_)
	nc.font.delete(font_)
end

function ncine.on_key_released(event)
	if event.sym == nc.keysym.LEFT then
		pos_.x = pos_.x - 20
	elseif event.sym == nc.keysym.RIGHT then
		pos_.x = pos_.x + 20
	elseif event.sym == nc.keysym.Q or event.sym == nc.keysym.ESCAPE then
		nc.application.quit()
	end
end

function ncine.on_touch_down(event)
	pos_.x = event[0].x
	pos_.y = event[0].y
end

function ncine.on_touch_move(event)
	nc.particle_system.set_position(particlesys_, event[0].x, event[0].y)
end

function ncine.on_mouse_button_released(event)
	if event.button == nc.mouse_button.LEFT then
		pos_ = {x = event.x, y = event.y}
		nc.particle_system.set_position(particlesys_, event.x, event.y)
	elseif event.button == nc.mouse_button.RIGHT then
		nc.sprite.set_position(sprite_, event.x, event.y)
	end
end

function ncine.on_mouse_moved(state)
	if state.left_pressed then
		nc.particle_system.set_position(particlesys_, state.x, state.y)
	end
end

function ncine.on_scroll_input(event)
	pos_.y = pos_.y + event.y * 5
end

function ncine.on_joymapped_button_released(event)
	if event.button == nc.joy_button.START then
		nc.application.quit()
	end
end

if needs_start then
	ncine.start()
end

