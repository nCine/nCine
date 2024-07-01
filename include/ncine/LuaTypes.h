#ifndef CLASS_NCINE_LUATYPES
#define CLASS_NCINE_LUATYPES

#include "common_defines.h"
#include <nctl/Array.h>

struct lua_State;

namespace ncine {

class KeyboardState;
class JoystickState;
class JoyMappedState;

class Viewport;
class Camera;
class Shader;
class ShaderState;

class AppConfiguration;
class Texture;
class SceneNode;
class DrawableNode;
class BaseSprite;
class Sprite;
class MeshSprite;
class AnimatedSprite;
class Font;
class TextNode;

class AudioBuffer;
class AudioBufferPlayer;
class AudioStreamPlayer;
class AudioFilter;
class AudioEffectSlot;
class AudioEffect;
class AudioEffectProperties;

class ParticleSystem;
class ParticleAffector;

/// The userdata types wrapped by Lua
namespace LuaTypes {
	enum UserDataType
	{
		KEYBOARDSTATE = 0,
		JOYSTICKSTATE,
		JOYMAPPEDSTATE,

		VIEWPORT,
		CAMERA,
		SHADER,
		SHADERSTATE,

		TEXTURE,
		SCENENODE,
		DRAWABLENODE,
		BASE_SPRITE,
		SPRITE,
		MESH_SPRITE,
		ANIMATED_SPRITE,
		FONT,
		TEXTNODE,

		AUDIOBUFFER,
		IAUDIOPLAYER,
		AUDIOBUFFER_PLAYER,
		AUDIOSTREAM_PLAYER,
		AUDIO_FILTER,
		AUDIO_EFFECT_SLOT,
		AUDIO_EFFECT,
		AUDIO_EFFECT_PROPERTIES,

		PARTICLE_SYSTEM,
		PARTICLE_AFFECTOR,

		UNKNOWN
	};

	template <class T> inline LuaTypes::UserDataType classToUserDataType(T *) { return LuaTypes::UserDataType::UNKNOWN; }

	template <> inline LuaTypes::UserDataType classToUserDataType<KeyboardState>(KeyboardState *) { return LuaTypes::UserDataType::KEYBOARDSTATE; }
	template <> inline LuaTypes::UserDataType classToUserDataType<JoystickState>(JoystickState *) { return LuaTypes::UserDataType::JOYSTICKSTATE; }
	template <> inline LuaTypes::UserDataType classToUserDataType<JoyMappedState>(JoyMappedState *) { return LuaTypes::UserDataType::JOYMAPPEDSTATE; }

	template <> inline LuaTypes::UserDataType classToUserDataType<Viewport>(Viewport *) { return LuaTypes::UserDataType::VIEWPORT; }
	template <> inline LuaTypes::UserDataType classToUserDataType<Camera>(Camera *) { return LuaTypes::UserDataType::CAMERA; }
	template <> inline LuaTypes::UserDataType classToUserDataType<Shader>(Shader *) { return LuaTypes::UserDataType::SHADER; }
	template <> inline LuaTypes::UserDataType classToUserDataType<ShaderState>(ShaderState *) { return LuaTypes::UserDataType::SHADERSTATE; }

	template <> inline LuaTypes::UserDataType classToUserDataType<Texture>(Texture *) { return LuaTypes::UserDataType::TEXTURE; }
	template <> inline LuaTypes::UserDataType classToUserDataType<SceneNode>(SceneNode *) { return LuaTypes::UserDataType::SCENENODE; }
	template <> inline LuaTypes::UserDataType classToUserDataType<DrawableNode>(DrawableNode *) { return LuaTypes::UserDataType::DRAWABLENODE; }
	template <> inline LuaTypes::UserDataType classToUserDataType<BaseSprite>(BaseSprite *) { return LuaTypes::UserDataType::BASE_SPRITE; }
	template <> inline LuaTypes::UserDataType classToUserDataType<Sprite>(Sprite *) { return LuaTypes::UserDataType::SPRITE; }
	template <> inline LuaTypes::UserDataType classToUserDataType<MeshSprite>(MeshSprite *) { return LuaTypes::UserDataType::MESH_SPRITE; }
	template <> inline LuaTypes::UserDataType classToUserDataType<AnimatedSprite>(AnimatedSprite *) { return LuaTypes::UserDataType::ANIMATED_SPRITE; }
	template <> inline LuaTypes::UserDataType classToUserDataType<Font>(Font *) { return LuaTypes::UserDataType::FONT; }
	template <> inline LuaTypes::UserDataType classToUserDataType<TextNode>(TextNode *) { return LuaTypes::UserDataType::TEXTNODE; }

	template <> inline LuaTypes::UserDataType classToUserDataType<AudioBuffer>(AudioBuffer *) { return LuaTypes::UserDataType::AUDIOBUFFER; }
	template <> inline LuaTypes::UserDataType classToUserDataType<IAudioPlayer>(IAudioPlayer *) { return LuaTypes::UserDataType::IAUDIOPLAYER; }
	template <> inline LuaTypes::UserDataType classToUserDataType<AudioBufferPlayer>(AudioBufferPlayer *) { return LuaTypes::UserDataType::AUDIOBUFFER_PLAYER; }
	template <> inline LuaTypes::UserDataType classToUserDataType<AudioStreamPlayer>(AudioStreamPlayer *) { return LuaTypes::UserDataType::AUDIOSTREAM_PLAYER; }
	template <> inline LuaTypes::UserDataType classToUserDataType<AudioFilter>(AudioFilter *) { return LuaTypes::UserDataType::AUDIO_FILTER; }
	template <> inline LuaTypes::UserDataType classToUserDataType<AudioEffectSlot>(AudioEffectSlot *) { return LuaTypes::UserDataType::AUDIO_EFFECT_SLOT; }
	template <> inline LuaTypes::UserDataType classToUserDataType<AudioEffect>(AudioEffect *) { return LuaTypes::UserDataType::AUDIO_EFFECT; }
	template <> inline LuaTypes::UserDataType classToUserDataType<AudioEffectProperties>(AudioEffectProperties *) { return LuaTypes::UserDataType::AUDIO_EFFECT_PROPERTIES; }

	template <> inline LuaTypes::UserDataType classToUserDataType<ParticleSystem>(ParticleSystem *) { return LuaTypes::UserDataType::PARTICLE_SYSTEM; }
	template <> inline LuaTypes::UserDataType classToUserDataType<ParticleAffector>(ParticleAffector *) { return LuaTypes::UserDataType::PARTICLE_AFFECTOR; }

	template <class T> inline const char *classToName(T *) { return "unknown"; }

	template <> inline const char *classToName<KeyboardState>(KeyboardState *) { return "KeyboardState"; }
	template <> inline const char *classToName<JoystickState>(JoystickState *) { return "JoystickState"; }
	template <> inline const char *classToName<JoyMappedState>(JoyMappedState *) { return "JoyMappedState"; }

	template <> inline const char *classToName<Viewport>(Viewport *) { return "Viewport"; }
	template <> inline const char *classToName<Camera>(Camera *) { return "Camera"; }
	template <> inline const char *classToName<Shader>(Shader *) { return "Shader"; }
	template <> inline const char *classToName<ShaderState>(ShaderState *) { return "ShaderState"; }

	template <> inline const char *classToName<Texture>(Texture *) { return "Texture"; }
	template <> inline const char *classToName<SceneNode>(SceneNode *) { return "SceneNode"; }
	template <> inline const char *classToName<DrawableNode>(DrawableNode *) { return "DrawableNode"; }
	template <> inline const char *classToName<BaseSprite>(BaseSprite *) { return "BaseSprite"; }
	template <> inline const char *classToName<Sprite>(Sprite *) { return "Sprite"; }
	template <> inline const char *classToName<MeshSprite>(MeshSprite *) { return "MeshSprite"; }
	template <> inline const char *classToName<AnimatedSprite>(AnimatedSprite *) { return "AnimatedSprite"; }
	template <> inline const char *classToName<Font>(Font *) { return "Font"; }
	template <> inline const char *classToName<TextNode>(TextNode *) { return "TextNode"; }

	template <> inline const char *classToName<AudioBuffer>(AudioBuffer *) { return "AudioBuffer"; }
	template <> inline const char *classToName<IAudioPlayer>(IAudioPlayer *) { return "IAudioPlayer"; }
	template <> inline const char *classToName<AudioBufferPlayer>(AudioBufferPlayer *) { return "AudioBufferPlayer"; }
	template <> inline const char *classToName<AudioStreamPlayer>(AudioStreamPlayer *) { return "AudioStreamPlayer"; }
	template <> inline const char *classToName<AudioFilter>(AudioFilter *) { return "AudioFilter"; }
	template <> inline const char *classToName<AudioEffectSlot>(AudioEffectSlot *) { return "AudioEffectSlot"; }
	template <> inline const char *classToName<AudioEffect>(AudioEffect *) { return "AudioEffect"; }
	template <> inline const char *classToName<AudioEffectProperties>(AudioEffectProperties *) { return "AudioEffectProperties"; }

	template <> inline const char *classToName<ParticleSystem>(ParticleSystem *) { return "ParticleSystem"; }
	template <> inline const char *classToName<ParticleAffector>(ParticleAffector *) { return "ParticleAffector"; }

	inline const char *userDataTypeToName(LuaTypes::UserDataType type)
	{
		switch (type)
		{
			case LuaTypes::UserDataType::UNKNOWN: return "unknown";

			case LuaTypes::UserDataType::KEYBOARDSTATE: return "KeyboardState";
			case LuaTypes::UserDataType::JOYSTICKSTATE: return "JoystickState";
			case LuaTypes::UserDataType::JOYMAPPEDSTATE: return "JoyMappedState";

			case LuaTypes::UserDataType::VIEWPORT: return "Viewport";
			case LuaTypes::UserDataType::CAMERA: return "Camera";
			case LuaTypes::UserDataType::SHADER: return "Shader";
			case LuaTypes::UserDataType::SHADERSTATE: return "ShaderState";

			case LuaTypes::UserDataType::TEXTURE: return "Texture";
			case LuaTypes::UserDataType::SCENENODE: return "SceneNode";
			case LuaTypes::UserDataType::DRAWABLENODE: return "DrawableNode";
			case LuaTypes::UserDataType::BASE_SPRITE: return "BaseSprite";
			case LuaTypes::UserDataType::SPRITE: return "Sprite";
			case LuaTypes::UserDataType::MESH_SPRITE: return "MeshSprite";
			case LuaTypes::UserDataType::ANIMATED_SPRITE: return "AnimatedSprite";
			case LuaTypes::UserDataType::FONT: return "Font";
			case LuaTypes::UserDataType::TEXTNODE: return "TextNode";

			case LuaTypes::UserDataType::AUDIOBUFFER: return "AudioBuffer";
			case LuaTypes::UserDataType::IAUDIOPLAYER: return "IAudioPlayer";
			case LuaTypes::UserDataType::AUDIOBUFFER_PLAYER: return "AudioBufferPlayer";
			case LuaTypes::UserDataType::AUDIOSTREAM_PLAYER: return "AudioStreamPlayer";
			case LuaTypes::UserDataType::AUDIO_FILTER: return "AudioFilter";
			case LuaTypes::UserDataType::AUDIO_EFFECT_SLOT: return "AudioEffectSlot";
			case LuaTypes::UserDataType::AUDIO_EFFECT: return "AudioEffect";
			case LuaTypes::UserDataType::AUDIO_EFFECT_PROPERTIES: return "AudioEffectProperties";

			case LuaTypes::UserDataType::PARTICLE_SYSTEM: return "ParticleSystem";
			case LuaTypes::UserDataType::PARTICLE_AFFECTOR: return "ParticleAffector";
		}

		return "unknown";
	}
}

}

#endif
