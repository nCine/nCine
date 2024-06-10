#ifndef CLASS_NCINE_JOYMAPPING
#define CLASS_NCINE_JOYMAPPING

#include <cstdint>
#include <nctl/Array.h>
#include <nctl/StaticArray.h>
#include "InputEvents.h"

namespace ncine {

class IInputManager;
class JoyButtonEvent;
class JoyAxisEvent;
class JoyConnectionEvent;
class IInputEventHandler;
template <class T> class Vector2;
using Vector2f = Vector2<float>;

/// Information about a mapped joystick state (implementation)
class JoyMappedStateImpl : public JoyMappedState
{
  public:
	JoyMappedStateImpl()
	{
		for (unsigned int i = 0; i < JoyMappedState::NumButtons; i++)
			buttons_[i] = false;
		for (unsigned int i = 0; i < JoyMappedState::NumAxes; i++)
			axesValues_[i] = 0.0f;
		lastHatState_ = HatState::CENTERED;
	}

	bool isButtonPressed(ButtonName name) const override
	{
		bool pressed = false;
		if (name != ButtonName::UNKNOWN)
			pressed = buttons_[static_cast<int>(name)];
		return pressed;
	}

	float axisValue(AxisName name) const override
	{
		float value = 0.0f;
		if (name != AxisName::UNKNOWN)
			value = axesValues_[static_cast<int>(name)];
		return value;
	}

  private:
	unsigned char buttons_[JoyMappedState::NumButtons];
	float axesValues_[JoyMappedState::NumAxes];
	unsigned char lastHatState_;

	friend class JoyMapping;
};

class JoyMapping
{
  public:
	JoyMapping();
	~JoyMapping() {}

	void init(const IInputManager *inputManager);
	inline void setHandler(IInputEventHandler *inputEventHandler) { inputEventHandler_ = inputEventHandler; }

	bool addMappingFromString(const char *mappingString);
	void addMappingsFromStrings(const char **mappingStrings);
	void addMappingsFromFile(const char *filename);
	inline unsigned int numMappings() const { return mappings_.size(); }

	void onJoyButtonPressed(const JoyButtonEvent &event);
	void onJoyButtonReleased(const JoyButtonEvent &event);
	void onJoyHatMoved(const JoyHatEvent &event);
	void onJoyAxisMoved(const JoyAxisEvent &event);
	bool onJoyConnected(const JoyConnectionEvent &event);
	void onJoyDisconnected(const JoyConnectionEvent &event);

	bool isJoyMapped(int joyId) const;
	const JoyMappedStateImpl &joyMappedState(int joyId) const;
	void deadZoneNormalize(Vector2f &joyVector, float deadZoneValue) const;

  private:
	static const unsigned int MaxNameLength = 64;

	struct MappedJoystick
	{
		struct Axis
		{
			AxisName name;
			ButtonName positiveButtonName = ButtonName::UNKNOWN;
			ButtonName negativeButtonName = ButtonName::UNKNOWN;
			float min;
			float max;
		};

		static const int MaxNumAxes = 10; // Maximum value encountered in most databases is `a9`
		static const int MaxNumButtons = 36; // Maximum value encountered in most databases is `b35`
		static const int MaxHatButtons = 4; // The four directions

		class Guid
		{
		  public:
			Guid();
			inline Guid(const char *string) { fromString(string); }
			void fromString(const char *string);

			bool operator==(const Guid &guid) const;

		  private:
			static const unsigned int NumComponents = 16;
			uint8_t array_[NumComponents];
		} guid;

		char name[MaxNameLength];
		Axis axes[MaxNumAxes];
		/// Button axes (buttons mapped as axes, like when analog triggers are missing)
		AxisName buttonAxes[MaxNumAxes];
		ButtonName buttons[MaxNumButtons];
		ButtonName hats[MaxHatButtons];

		MappedJoystick();
	};

	static const char *AxesStrings[JoyMappedState::NumAxes];
	static const char *ButtonsStrings[JoyMappedState::NumButtons];

	static const int MaxNumJoysticks = 4;
	static const int InvalidMappingIndex = -1;
	int mappingIndices_[MaxNumJoysticks];
	nctl::Array<MappedJoystick> mappings_;

	static JoyMappedStateImpl nullMappedJoyState_;
	static nctl::StaticArray<JoyMappedStateImpl, MaxNumJoysticks> mappedJoyStates_;
	static JoyMappedButtonEvent mappedButtonEvent_;
	static JoyMappedAxisEvent mappedAxisEvent_;

	const IInputManager *inputManager_;
	IInputEventHandler *inputEventHandler_;

#ifdef __ANDROID__
	MappedJoystick createAndroidDefaultMapping() const;
#endif
	void checkConnectedJoystics();
	int findMappingByGuid(const MappedJoystick::Guid &guid) const;
	int findMappingByName(const char *name) const;
	bool parseMappingFromString(const char *mappingString, MappedJoystick &map);
	bool parsePlatformKeyword(const char *start, const char *end) const;
	bool parsePlatformName(const char *start, const char *end) const;
	int parseAxisName(const char *start, const char *end) const;
	int parseButtonName(const char *start, const char *end) const;
	int parseAxisMapping(const char *start, const char *end, MappedJoystick::Axis &axis) const;
	int parseButtonMapping(const char *start, const char *end) const;
	int parseHatMapping(const char *start, const char *end) const;
	int hatStateToIndex(unsigned char hatState) const;
	void trimSpaces(const char **start, const char **end) const;
};

}

#endif
