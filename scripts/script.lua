#!/usr/bin/env lua
require ("ncine_header")
nc = ncine

function nc.on_pre_init(cfg)
	cfg.resolution = {x = 1280, y = 720}
	cfg.window_title = "nCine Lua test"

	--cfg.console_log_level = nc.log_level.OFF
	--cfg.resizable = true
	return cfg
end

function nc.on_init()
	if nc.ANDROID then
		font_tex_file = "DroidSans32_256_ETC2.ktx"
		texture_file = "texture2_ETC2.ktx"
		texture2_file = "texture3_ETC2.ktx"
		texture3_file = "smoke_256_ETC2.ktx"
		spritesheet_file = "spritesheet.webp"
	else
		font_tex_file = "DroidSans32_256.png"
		texture_file = "texture2.png"
		texture2_file = "texture3.png"
		texture3_file = "smoke_256.png"
		spritesheet_file = "spritesheet.png"
	end
	sound_file = "coins.ogg"

	local rootnode = nc.application.get_rootnode()
	local resolution = nc.application.get_resolution()
	pos_ = {x = resolution.x * 0.5, y = resolution.y * 0.5}
	angle_ = 0

	font_ = nc.font.new(nc.fs.get_data_path().."fonts/DroidSans32_256.fnt",
	                    nc.fs.get_data_path().."fonts/"..font_tex_file)
	textnode_ = nc.textnode.new(rootnode, font_, 256)
	nc.textnode.set_string(textnode_, "This is script.lua, press V to turn blur on and off.\nhttps://ncine.github.io")
	nc.textnode.set_layer(textnode_, 100)
	nc.textnode.set_position(textnode_, resolution.x * 0.25, resolution.y * 0.1)
	nc.textnode.set_color(textnode_, 0.6, 0.77, 1.0, 1.0)

	local color = nc.textnode.get_color(textnode_)
	color.b = 0.24
	nc.textnode.set_color(textnode_, color)

	texture_ = nc.texture.new(nc.fs.get_data_path().."textures/"..texture_file)
	texture2_ = nc.texture.new(nc.fs.get_data_path().."textures/"..texture2_file)
	texture3_ = nc.texture.new(nc.fs.get_data_path().."textures/"..texture3_file)
	spritesheet_ = nc.texture.new(nc.fs.get_data_path().."textures/"..spritesheet_file)

	local resolution_int = { x = math.floor(resolution.x), y = math.floor(resolution.y) }
	vp_texture0_ = nc.texture.new_init("Ping texture", nc.tex_format.RGB8, 1, resolution_int)
	vp_texture1_ = nc.texture.new_init("Pong texture", nc.tex_format.RGB8, 1, resolution_int)
	scene_viewport_ = nc.viewport.new()
	nc.viewport.set_texture(scene_viewport_, 0, vp_texture0_)
	ping_viewport_ = nc.viewport.new()
	nc.viewport.set_texture(ping_viewport_, 0, vp_texture1_)
	pong_viewport_ = nc.viewport.new()
	nc.viewport.set_texture(pong_viewport_, 0, vp_texture0_)

	local sprite_blur_fs = [[
#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D uTexture;
uniform vec2 uResolution;
uniform vec2 uDirection;
in vec2 vTexCoords;
out vec4 fragColor;

void main()
{
	vec2 uv = vec2(gl_FragCoord.xy / uResolution.xy);
	vec4 color = vec4(0.0);
	vec2 off1 = vec2(1.3846153846) * uDirection;
	vec2 off2 = vec2(3.2307692308) * uDirection;

	color += texture(uTexture, uv) * 0.2270270270;
	color += texture(uTexture, uv + (off1 / uResolution)) * 0.3162162162;
	color += texture(uTexture, uv - (off1 / uResolution)) * 0.3162162162;
	color += texture(uTexture, uv + (off2 / uResolution)) * 0.0702702703;
	color += texture(uTexture, uv - (off2 / uResolution)) * 0.0702702703;

	fragColor = color;
}
]]

	blur_shader_ = nc.shader.new()
	nc.shader.load_from_memory_default_vertex(
		blur_shader_, "SeparableBlur_Shader", nc.shader_introspection.ENABLED, nc.shader_default_vertex.SPRITE, sprite_blur_fs
	)
	assert(nc.shader.is_linked(blur_shader_), "Blur shader is not linked")

	vp_pingsprite_ = nc.sprite.new(nil, vp_texture0_, resolution.x * 0.5, resolution.y * 0.5)
	vp_pingshaderstate_ = nc.shaderstate.new(vp_pingsprite_, blur_shader_)
	nc.shaderstate.set_uniform_float_value2(vp_pingshaderstate_, "", "uResolution", resolution.x, resolution.y)

	vp_pongsprite_ = nc.sprite.new(nil, vp_texture1_, resolution.x * 0.5, resolution.y * 0.5)
	vp_pongshaderstate_ = nc.shaderstate.new(vp_pongsprite_, blur_shader_)
	nc.shaderstate.set_uniform_float_value2(vp_pongshaderstate_, "", "uResolution", resolution.x, resolution.y)

	nc.viewport.set_rootnode(scene_viewport_, rootnode)
	nc.viewport.set_rootnode(ping_viewport_, vp_pingsprite_)
	nc.viewport.set_rootnode(pong_viewport_, vp_pongsprite_)

	with_viewport_ = false
	setup_viewport()

	sprite_ = nc.sprite.new(rootnode, texture2_, resolution.x * 0.2, resolution.y * 0.5)
	meshsprite_ = nc.mesh_sprite.new(rootnode, texture_, resolution.x * 0.8, resolution.y * 0.5)
	nc.mesh_sprite.create_vertices_from_texels(
		meshsprite_, {{x = 0, y = 0}, {x = 128, y = 0}, {x = 0, y = 128}, {x = 128, y = 128}}, nc.texture_cut_mode.RESIZE
	)

	local tex_rect3 = {x = 0, y = 0, w = nc.texture.get_width(texture3_), h = nc.texture.get_height(texture3_)}
	particlesys_ = nc.particle_system.new(rootnode, 128, texture3_, tex_rect3)
	nc.particle_system.set_position(particlesys_, resolution.x * 0.5, resolution.y * 0.5)
	nc.particle_system.add_color_affector(particlesys_, {
		{0.0, {r = 1.0, g = 0.0, b = 0.0, a = 1.0}},
		{0.5, {r = 0.0, g = 1.0, b = 0.0, a = 0.5}},
		{1.0, {r = 0.0, g = 0.0, b = 1.0, a = 0.0}}
	})
	nc.particle_system.add_size_affector(particlesys_, 0.5, {{0.0, 0.25}, {0.45, 0.35}, {1.0, 0.75}})

	animated_sprite_ = nc.animated_sprite.new(rootnode, spritesheet_, resolution.x * 0.5, resolution.y * 0.5)
	local animation = {
		frame_duration = 0.12,
		loop_mode = nc.loop_mode.ENABLED,
		rewind_mode = nc.rewind_mode.FROM_START,
		rect_size = {x = 48, y = 48},
		source_rect = {x = 0, y = 0, w = nc.texture.get_width(spritesheet_), h = nc.texture.get_height(spritesheet_)}
	}
	nc.animated_sprite.add_animation(animated_sprite_, animation)

	audio_buffer_ = nc.audio_buffer.new(nc.fs.get_data_path().."sounds/"..sound_file)
	player_ = nc.audio_buffer_player.new(audio_buffer_)
	pause_ = false

	local settings = nc.application.get_rendering_settings()
	print("Rendering settings")
	for key, value in pairs(settings) do
		print("  "..key .. ": " .. tostring(value))
	end

	nc.log.info("on_init() Lua function terminated")
end

function nc.on_frame_start()
	nc.particle_system.set_particles_update_enabled(particlesys_, not pause_)
	if pause_ then
		return
	end

	local interval = nc.application.get_interval()
	angle_ = angle_ + 100 * interval

	local newpos = {x = 0, y = 0}
	newpos.x = pos_.x + 200 * math.sin(math.rad(angle_))
	newpos.y = pos_.y + 100 * math.sin(math.rad(angle_ + 90))
	nc.sprite.set_position(meshsprite_, newpos)

	local init_particles = {amount = 16, life = {0.25, 0.35},
	                       position = {{x = -5, y = -5}, {x = 5, y = 5}},
	                       velocity = {{x = -10, y = 200}, {x = 10, y = 200}}}
	nc.particle_system.emit_particles(particlesys_, init_particles)

	local velocity = {x = 0, y = 0}
	local keystate = nc.input.key_state()
	if nc.input.is_key_down(keystate, nc.keysym.LEFT) then
		velocity.x = -1
	elseif nc.input.is_key_down(keystate, nc.keysym.RIGHT) then
		velocity.x = 1
	end
	if nc.input.is_key_down(keystate, nc.keysym.DOWN) then
		velocity.y = -1
	elseif nc.input.is_key_down(keystate, nc.keysym.UP) then
		velocity.y = 1
	end

	if nc.vec2.sqrlength(velocity) > 0 then
		velocity = nc.vec2.normalized(velocity)
		nc.animated_sprite.set_paused(animated_sprite_, false)
	else
		nc.animated_sprite.set_paused(animated_sprite_, true)
		nc.animated_sprite.set_frame(animated_sprite_, 0)
	end
	local animated_sprite_pos = nc.animated_sprite.get_position(animated_sprite_)
	animated_sprite_pos.x = animated_sprite_pos.x + velocity.x * 100 * interval
	animated_sprite_pos.y = animated_sprite_pos.y + velocity.y * 100 * interval
	nc.animated_sprite.set_position(animated_sprite_, animated_sprite_pos)

	if nc.input.is_key_down(keystate, nc.keysym.LEFT) and nc.input.is_key_down(keystate, nc.keysym.DOWN) then
		nc.animated_sprite.set_rotation(animated_sprite_, 270 + 45)
	elseif nc.input.is_key_down(keystate, nc.keysym.RIGHT) and nc.input.is_key_down(keystate, nc.keysym.DOWN) then
		nc.animated_sprite.set_rotation(animated_sprite_, 90 - 45)
	elseif nc.input.is_key_down(keystate, nc.keysym.LEFT) and nc.input.is_key_down(keystate, nc.keysym.UP) then
		nc.animated_sprite.set_rotation(animated_sprite_, 270 - 45)
	elseif nc.input.is_key_down(keystate, nc.keysym.RIGHT) and nc.input.is_key_down(keystate, nc.keysym.UP) then
		nc.animated_sprite.set_rotation(animated_sprite_, 90 + 45)
	elseif nc.input.is_key_down(keystate, nc.keysym.LEFT) then
		nc.animated_sprite.set_rotation(animated_sprite_, 270)
	elseif nc.input.is_key_down(keystate, nc.keysym.RIGHT) then
		nc.animated_sprite.set_rotation(animated_sprite_, 90)
	elseif nc.input.is_key_down(keystate, nc.keysym.DOWN) then
		nc.animated_sprite.set_rotation(animated_sprite_, 0)
	elseif nc.input.is_key_down(keystate, nc.keysym.UP) then
		nc.animated_sprite.set_rotation(animated_sprite_, 180)
	end
end

function nc.on_draw_viewport(viewport)
	if viewport == ping_viewport_ then
		nc.shaderstate.set_uniform_float_value2(vp_pingshaderstate_, "", "uDirection", 1.0, 0.0)
		nc.shaderstate.set_uniform_float_value2(vp_pongshaderstate_, "", "uDirection", 0.0, 1.0)
	end
end

function nc.on_resize_window(width, height)
	nc.texture.init(vp_texture0_, "Ping texture", nc.tex_format.RGB8, 1, width, height)
	nc.texture.init(vp_texture1_, "Pong texture", nc.tex_format.RGB8, 1, width, height)

	nc.viewport.remove_all_textures(scene_viewport_);
	nc.viewport.set_texture(scene_viewport_, 0, vp_texture0_);
	nc.viewport.remove_all_textures(ping_viewport_);
	nc.viewport.set_texture(ping_viewport_, 0, vp_texture1_);
	nc.viewport.remove_all_textures(pong_viewport_);
	nc.viewport.set_texture(pong_viewport_, 0, vp_texture0_);

	nc.sprite.reset_texture(vp_pingsprite_)
	nc.sprite.reset_texture(vp_pongsprite_)

	nc.sprite.set_position(vp_pingsprite_, width * 0.5, height * 0.5)
	nc.sprite.set_position(vp_pongsprite_, width * 0.5, height * 0.5)

	nc.shaderstate.set_uniform_float_value2(vp_pingshaderstate_, "", "uResolution", width, height)
	nc.shaderstate.set_uniform_float_value2(vp_pongshaderstate_, "", "uResolution", width, height)
end

function nc.on_shutdown()
	-- For a fast reload to work correctly, every object should be set to `nil` after deletion
	nc.audio_buffer_player.delete(player_)
	player_ = nil
	nc.audio_buffer.delete(audio_buffer_)
	audio_buffer_ = nil

	nc.animated_sprite.delete(animated_sprite_)
	animated_sprite_ = nil
	nc.particle_system.delete(particlesys_)
	particlesys_ = nil
	nc.mesh_sprite.delete(meshsprite_)
	meshsprite_ = nil
	nc.sprite.delete(sprite_)
	sprite_ = nil

	nc.shaderstate.delete(vp_pongshaderstate_)
	vp_pongshaderstate_ = nil
	nc.sprite.delete(vp_pongsprite_)
	vp_pongsprite_ = nil
	nc.shaderstate.delete(vp_pingshaderstate_)
	vp_pingshaderstate_ = nil
	nc.sprite.delete(vp_pingsprite_)
	vp_pingsprite_ = nil
	nc.shader.delete(blur_shader_)
	blur_shader_ = nil

	nc.viewport.delete(pong_viewport_)
	pong_viewport_ = nil
	nc.viewport.delete(ping_viewport_)
	ping_viewport_ = nil
	nc.viewport.delete(scene_viewport_)
	scene_viewport_ = nil
	nc.texture.delete(vp_texture1_)
	vp_texture1_ = nil
	nc.texture.delete(vp_texture0_)
	vp_texture0_ = nil

	nc.texture.delete(spritesheet_)
	spritesheet_ = nil
	nc.texture.delete(texture3_)
	texture3_ = nil
	nc.texture.delete(texture2_)
	texture2_ = nil
	nc.texture.delete(texture_)
	texture_  = nil

	nc.textnode.delete(textnode_)
	textnode_ = nil
	nc.font.delete(font_)
	font_ = nil
end

function nc.on_key_released(event)
	if event.sym == nc.keysym.V then
		with_viewport_ = not with_viewport_
		setup_viewport()
	elseif event.sym == nc.keysym.P then
		pause_ = not pause_;
	elseif event.sym == nc.keysym.F then
		nc.gfx_device.set_fullscreen(not nc.gfx_device.is_fullscreen())
		if nc.gfx_device.is_fullscreen() == false then
			nc.gfx_device.set_window_size(nc.application.get_app_configuration().resolution)
		end
	elseif event.sym == nc.keysym.F5 or event.sym == nc.keysym.N5 then
		-- Don't call `reload_script()` if the function is not available, like when using the Lua interpreter directly
		if reload_script then
			reload_script()
		end
	elseif event.mod == nc.keymod.CTRL and event.sym == nc.keysym.H then
		-- Don't call `show_gui()` if the function is not available, like when using the Lua interpreter directly
		if show_gui and is_gui_hidden then
			show_gui(is_gui_hidden())
		end
	elseif event.sym == nc.keysym.ESCAPE then
		nc.application.quit()
	end
end

function nc.on_touch_down(event)
	pos_.x = event[0].x
	pos_.y = event[0].y

	nc.audio_buffer_player.stop(player_)
	nc.audio_buffer_player.play(player_)
end

function nc.on_touch_move(event)
	nc.particle_system.set_position(particlesys_, event[0].x, event[0].y)
end

function nc.on_mouse_button_released(event)
	if event.button == nc.mouse_button.LEFT then
		pos_ = {x = event.x, y = event.y}
		nc.particle_system.set_position(particlesys_, event.x, event.y)
	elseif event.button == nc.mouse_button.RIGHT then
		nc.sprite.set_position(sprite_, event.x, event.y)
	end

	nc.audio_buffer_player.stop(player_)
	nc.audio_buffer_player.play(player_)
end

function nc.on_mouse_moved(state)
	if state.is_left_down then
		nc.particle_system.set_position(particlesys_, state.x, state.y)
	end
end

function nc.on_scroll_input(event)
	pos_.y = pos_.y + event.y * 5
end

function nc.on_joymapped_button_released(event)
	if event.button == nc.joy_button.A then
		with_viewport_ = not with_viewport_
		setup_viewport()
	elseif event.button == nc.joy_button.START then
		pause_ = not pause_
	elseif event.button == nc.joy_button.GUIDE then
		nc.application.quit()
	end
end

function setup_viewport()
	nc.viewport.clear_chain()

	if with_viewport_ then
		for i = 1, 2 do
			nc.viewport.push_back_chain(pong_viewport_)
			nc.viewport.push_back_chain(ping_viewport_)
		end
		nc.viewport.push_back_chain(scene_viewport_)

		nc.viewport.set_rootnode(nc.application.get_screen_viewport(), vp_pongsprite_)
	else
		nc.viewport.set_rootnode(nc.application.get_screen_viewport(), nc.application.get_rootnode())
	end
end

require("ncine_footer")
