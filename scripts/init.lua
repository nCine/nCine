nc = ncine

function ncine.on_pre_init(cfg)
	cfg.x_res = 1280
	cfg.y_res = 720
	cfg.window_title = "nCine Lua test"
	return cfg
end

function load()
	if nc.ANDROID then
		texture_file = "smoke_256_ETC2.ktx"
	else
		texture_file = "smoke_256.png"
	end

	particleSys = {
		texture = texture_file,
		count = 128,
		x = nc.application.get_width() * 0.5,
		y = nc.application.get_height() * 0.5
	}

	return particleSys
end
