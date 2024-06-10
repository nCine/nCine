#include "JoyMapping.h"
#include "IInputManager.h"
#include <cstring> // for memcpy()
#include <cstdlib> // for strtoul()
#include "IInputEventHandler.h"
#include "IFile.h"
#include "Vector2.h"
#include <nctl/algorithms.h>

namespace ncine {

namespace {

#include "JoyMappingDb.h"

}

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

const unsigned int JoyMapping::MaxNameLength;

const char *JoyMapping::AxesStrings[JoyMappedState::NumAxes] = {
	"leftx",
	"lefty",
	"rightx",
	"righty",
	"lefttrigger",
	"righttrigger"
};

const char *JoyMapping::ButtonsStrings[JoyMappedState::NumButtons] = {
	"a",
	"b",
	"x",
	"y",
	"back",
	"guide",
	"start",
	"leftstick",
	"rightstick",
	"leftshoulder",
	"rightshoulder",
	"dpup",
	"dpdown",
	"dpleft",
	"dpright",
	"misc1",
	"paddle1",
	"paddle2",
	"paddle3",
	"paddle4"
};

JoyMappedStateImpl JoyMapping::nullMappedJoyState_;
nctl::StaticArray<JoyMappedStateImpl, JoyMapping::MaxNumJoysticks> JoyMapping::mappedJoyStates_(nctl::StaticArrayMode::EXTEND_SIZE);
JoyMappedButtonEvent JoyMapping::mappedButtonEvent_;
JoyMappedAxisEvent JoyMapping::mappedAxisEvent_;

///////////////////////////////////////////////////////////
// CONSTRUCTORS AND DESTRUCTOR
///////////////////////////////////////////////////////////

JoyMapping::MappedJoystick::MappedJoystick()
{
	name[0] = '\0';

	for (unsigned int i = 0; i < MaxNumAxes; i++)
		axes[i].name = AxisName::UNKNOWN;
	for (unsigned int i = 0; i < MaxNumAxes; i++)
		buttonAxes[i] = AxisName::UNKNOWN;
	for (unsigned int i = 0; i < MaxNumButtons; i++)
		buttons[i] = ButtonName::UNKNOWN;
	for (unsigned int i = 0; i < MaxHatButtons; i++)
		hats[i] = ButtonName::UNKNOWN;
}

JoyMapping::MappedJoystick::Guid::Guid()
{
	for (unsigned int i = 0; i < NumComponents; i++)
		array_[i] = 0;
}

JoyMapping::JoyMapping()
    : mappings_(256), inputManager_(nullptr), inputEventHandler_(nullptr)
{
	for (unsigned int i = 0; i < MaxNumJoysticks; i++)
		mappingIndices_[i] = InvalidMappingIndex;

#ifdef __ANDROID__
	// Add the Android default system mapping at index 0
	mappings_.emplaceBack(createAndroidDefaultMapping());
	ASSERT(mappings_.size() == 1); // at index 0
#endif

	unsigned int numStrings = 0;

	// Add mappings from the database, without searching for duplicates
	const char **mappingStrings = ControllerMappings;
	while (*mappingStrings)
	{
		numStrings++;
		MappedJoystick mapping;
		const bool parsed = parseMappingFromString(*mappingStrings, mapping);
		if (parsed)
			mappings_.pushBack(mapping);
		mappingStrings++;
	}

	LOGI_X("Parsed %u strings for %u mappings", numStrings, mappings_.size());
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void JoyMapping::MappedJoystick::Guid::fromString(const char *string)
{
	uint32_t *guid32 = reinterpret_cast<uint32_t *>(array_);

	// Handling special type strings first
	unsigned int type = 0;
	if (strncmp(string, "default", 7) == 0)
		type = 1;
	else if (strncmp(string, "xinput", 6) == 0)
		type = 2;
	else if (strncmp(string, "hidapi", 6) == 0)
		type = 3;

	if (type != 0)
	{
		guid32[0] = UINT32_MAX; // Bus & CRC16 fields
		guid32[1] = type;
		guid32[2] = 0;
		guid32[3] = 0;
	}
	else
	{
		char component[9];

		uint32_t *guid32 = reinterpret_cast<uint32_t *>(array_);
		const unsigned length32 = sizeof(array_) / sizeof(uint32_t);

		unsigned int offset = 0;
		for (unsigned int i = 0; i < length32; i++)
		{
			memcpy(component, string + offset, 8);
			component[8] = '\0';
			guid32[i] = strtoul(component, nullptr, 16);
			offset += 8;
		}
	}
}

bool JoyMapping::MappedJoystick::Guid::operator==(const Guid &guid) const
{
	return (memcmp(array_, guid.array_, NumComponents * sizeof(uint8_t)) == 0);
}

void JoyMapping::init(const IInputManager *inputManager)
{
	ASSERT(inputManager);
	inputManager_ = inputManager;
	checkConnectedJoystics();
}

bool JoyMapping::addMappingFromString(const char *mappingString)
{
	ASSERT(mappingString);

	MappedJoystick newMapping;
	const bool parsed = parseMappingFromString(mappingString, newMapping);
	if (parsed)
	{
		int index = findMappingByGuid(newMapping.guid);
		// if GUID is not found then mapping has to be added, not replaced
		if (index < 0)
			index = mappings_.size();
		mappings_[index] = newMapping;
	}
	checkConnectedJoystics();

	return parsed;
}

void JoyMapping::addMappingsFromStrings(const char **mappingStrings)
{
	ASSERT(mappingStrings);

	while (*mappingStrings)
	{
		MappedJoystick newMapping;
		const bool parsed = parseMappingFromString(*mappingStrings, newMapping);
		if (parsed)
		{
			int index = findMappingByGuid(newMapping.guid);
			// if GUID is not found then mapping has to be added, not replaced
			if (index < 0)
				index = mappings_.size();
			mappings_[index] = newMapping;
		}
		mappingStrings++;
	}

	checkConnectedJoystics();
}

void JoyMapping::addMappingsFromFile(const char *filename)
{
	nctl::UniquePtr<IFile> fileHandle = IFile::createFileHandle(filename);
	fileHandle->open(IFile::OpenMode::READ);

	if (fileHandle->isOpened() == false)
		return;

	const long int fileSize = fileHandle->size();
	unsigned int fileLine = 0;

	nctl::UniquePtr<char[]> fileBuffer = nctl::makeUnique<char[]>(fileSize + 1);
	fileHandle->read(fileBuffer.get(), fileSize);
	fileHandle.reset(nullptr);
	fileBuffer[fileSize] = '\0';

	unsigned int numParsed = 0;
	const char *buffer = fileBuffer.get();
	do
	{
		fileLine++;

		MappedJoystick newMapping;
		const bool parsed = parseMappingFromString(buffer, newMapping);
		if (parsed)
		{
			numParsed++;
			int index = findMappingByGuid(newMapping.guid);
			// if GUID is not found then mapping has to be added, not replaced
			if (index < 0)
				index = mappings_.size();
			mappings_[index] = newMapping;
		}

	} while (strchr(buffer, '\n') && (buffer = strchr(buffer, '\n') + 1) < fileBuffer.get() + fileSize);

	LOGI_X("Joystick mapping file parsed: %u mappings in %u lines", numParsed, fileLine);

	fileBuffer.reset(nullptr);

	checkConnectedJoystics();
}

void JoyMapping::onJoyButtonPressed(const JoyButtonEvent &event)
{
	if (inputEventHandler_ == nullptr)
		return;

	const int mappingIndex = mappingIndices_[event.joyId];
	const bool mappingIsValid = (mappingIndex != InvalidMappingIndex && event.buttonId >= 0 &&
	                             event.buttonId < static_cast<int>(MappedJoystick::MaxNumButtons));

	if (mappingIsValid)
	{
		mappedButtonEvent_.joyId = event.joyId;
		mappedButtonEvent_.buttonName = mappings_[mappingIndex].buttons[event.buttonId];
		if (mappedButtonEvent_.buttonName != ButtonName::UNKNOWN)
		{
			const int buttonId = static_cast<int>(mappedButtonEvent_.buttonName);
			mappedJoyStates_[event.joyId].buttons_[buttonId] = true;
			inputEventHandler_->onJoyMappedButtonPressed(mappedButtonEvent_);
		}
		else
		{
			// Check if the button is mapped as an axis
			const AxisName axisName = mappings_[mappingIndex].buttonAxes[event.buttonId];
			if (axisName != AxisName::UNKNOWN)
			{
				mappedAxisEvent_.joyId = event.joyId;
				mappedAxisEvent_.axisName = axisName;
				mappedAxisEvent_.value = 1.0f;

				mappedJoyStates_[event.joyId].axesValues_[static_cast<int>(axisName)] = mappedAxisEvent_.value;
				inputEventHandler_->onJoyMappedAxisMoved(mappedAxisEvent_);
			}
		}
	}
}

void JoyMapping::onJoyButtonReleased(const JoyButtonEvent &event)
{
	if (inputEventHandler_ == nullptr)
		return;

	const int mappingIndex = mappingIndices_[event.joyId];
	const bool mappingIsValid = (mappingIndex != InvalidMappingIndex && event.buttonId >= 0 &&
	                             event.buttonId < static_cast<int>(MappedJoystick::MaxNumButtons));

	if (mappingIsValid)
	{
		mappedButtonEvent_.joyId = event.joyId;
		mappedButtonEvent_.buttonName = mappings_[mappingIndex].buttons[event.buttonId];
		if (mappedButtonEvent_.buttonName != ButtonName::UNKNOWN)
		{
			const int buttonId = static_cast<int>(mappedButtonEvent_.buttonName);
			mappedJoyStates_[event.joyId].buttons_[buttonId] = false;
			inputEventHandler_->onJoyMappedButtonReleased(mappedButtonEvent_);
		}
		else
		{
			// Check if the button is mapped as an axis
			const AxisName axisName = mappings_[mappingIndex].buttonAxes[event.buttonId];
			if (axisName != AxisName::UNKNOWN)
			{
				mappedAxisEvent_.joyId = event.joyId;
				mappedAxisEvent_.axisName = axisName;
				mappedAxisEvent_.value = 0.0f;

				mappedJoyStates_[event.joyId].axesValues_[static_cast<int>(axisName)] = mappedAxisEvent_.value;
				inputEventHandler_->onJoyMappedAxisMoved(mappedAxisEvent_);
			}
		}
	}
}

void JoyMapping::onJoyHatMoved(const JoyHatEvent &event)
{
	if (inputEventHandler_ == nullptr)
		return;

	const int mappingIndex = mappingIndices_[event.joyId];
	// Only the first gamepad hat is mapped
	const bool mappingIsValid = (mappingIndex != InvalidMappingIndex && event.hatId == 0);

	const unsigned char oldHatState = mappedJoyStates_[event.joyId].lastHatState_;
	const unsigned char newHatState = event.hatState;
	if (mappingIsValid && oldHatState != newHatState)
	{
		mappedButtonEvent_.joyId = event.joyId;

		const unsigned char firstHatValue = HatState::UP;
		const unsigned char lastHatValue = HatState::LEFT;
		for (unsigned char hatValue = firstHatValue; hatValue <= lastHatValue; hatValue *= 2)
		{
			if ((oldHatState & hatValue) != (newHatState & hatValue))
			{
				const int hatIndex = hatStateToIndex(hatValue);

				mappedButtonEvent_.buttonName = mappings_[mappingIndex].hats[hatIndex];
				if (mappedButtonEvent_.buttonName != ButtonName::UNKNOWN)
				{
					const int buttonId = static_cast<int>(mappedButtonEvent_.buttonName);
					if (newHatState & hatValue)
					{
						mappedJoyStates_[event.joyId].buttons_[buttonId] = true;
						inputEventHandler_->onJoyMappedButtonPressed(mappedButtonEvent_);
					}
					else
					{
						mappedJoyStates_[event.joyId].buttons_[buttonId] = false;
						inputEventHandler_->onJoyMappedButtonReleased(mappedButtonEvent_);
					}
				}
			}
			mappedJoyStates_[event.joyId].lastHatState_ = event.hatState;
		}
	}
}

void JoyMapping::onJoyAxisMoved(const JoyAxisEvent &event)
{
	if (inputEventHandler_ == nullptr)
		return;

	const int mappingIndex = mappingIndices_[event.joyId];
	const bool mappingIsValid = (mappingIndex != InvalidMappingIndex && event.axisId >= 0 &&
	                             event.axisId < static_cast<int>(MappedJoystick::MaxNumAxes));

	if (mappingIsValid)
	{
		const MappedJoystick::Axis &axis = mappings_[mappingIndex].axes[event.axisId];

		// Regular axis
		if (mappedAxisEvent_.axisName != AxisName::UNKNOWN)
		{
			mappedAxisEvent_.joyId = event.joyId;
			mappedAxisEvent_.axisName = axis.name;
			const float value = (event.normValue + 1.0f) * 0.5f;
			mappedAxisEvent_.value = axis.min + value * (axis.max - axis.min);
			mappedJoyStates_[event.joyId].axesValues_[static_cast<int>(axis.name)] = mappedAxisEvent_.value;
			inputEventHandler_->onJoyMappedAxisMoved(mappedAxisEvent_);
		}

		// Axis mapped as button
		if (axis.positiveButtonName != ButtonName::UNKNOWN || axis.negativeButtonName != ButtonName::UNKNOWN)
		{
			mappedButtonEvent_.joyId = event.joyId;
			mappedButtonEvent_.buttonName = axis.positiveButtonName;
			const int buttonId = static_cast<int>(mappedButtonEvent_.buttonName);

			const bool prevState = mappedJoyStates_[event.joyId].buttons_[buttonId];
			const bool newState = (axis.positiveButtonName != ButtonName::UNKNOWN) ?
			            (event.value >= IInputManager::RightStickDeadZone) :
			            (event.value <= -IInputManager::RightStickDeadZone);

			if (newState != prevState)
			{
				mappedJoyStates_[event.joyId].buttons_[buttonId] = newState;
				if (newState)
					inputEventHandler_->onJoyMappedButtonPressed(mappedButtonEvent_);
				else
					inputEventHandler_->onJoyMappedButtonReleased(mappedButtonEvent_);
			}
		}
	}
}

bool JoyMapping::onJoyConnected(const JoyConnectionEvent &event)
{
	const char *joyName = inputManager_->joyName(event.joyId);
	const char *joyGuid = inputManager_->joyGuid(event.joyId);
	int &mappingIndex = mappingIndices_[event.joyId];

	mappingIndex = InvalidMappingIndex;

	if (joyGuid != nullptr)
	{
		MappedJoystick::Guid guid(joyGuid);
		const int index = findMappingByGuid(guid);
		if (index != InvalidMappingIndex)
		{
			mappingIndex = index;
			LOGI_X("Joystick mapping found for \"%s\" with GUID \"%s\" (%d) at index %d", joyName, joyGuid, event.joyId, index);
		}
	}

#if defined(__ANDROID__)
	if (mappingIndex == InvalidMappingIndex)
	{
		const int index = 0;
		mappingIndex = index;
		LOGI_X("Joystick mapping not found for \"%s\" (%d), using Android default mapping at index %d", joyName, event.joyId, index);
	}
#elif !defined(__EMSCRIPTEN__)
	// Skip searching by name on Android as it can lead to incorrect mapping
	if (mappingIndex == InvalidMappingIndex)
	{
		const int index = findMappingByName(joyName);
		if (index != InvalidMappingIndex)
		{
			mappingIndex = index;
			LOGI_X("Joystick mapping found for \"%s\" (%d) at index %d", joyName, event.joyId, index);
		}
	}

	if (mappingIndex == InvalidMappingIndex)
	{
	#if defined(__linux__) && !defined(__ANDROID__)
		const bool excluded = (strstr(joyName, "Razer ") && (strstr(joyName, "Keyboard") || strstr(joyName, "DeathAdder")));
	#else
		const bool excluded = false;
	#endif
		if (excluded == false)
		{
			MappedJoystick::Guid xinputGuid("xinput");
			const int index = findMappingByGuid(xinputGuid);
			if (index != InvalidMappingIndex)
			{
				mappingIndex = index;
				LOGI_X("Joystick mapping not found for \"%s\" (%d), using XInput at index %d", joyName, event.joyId, index);
			}
		}
	}
#endif

	return (mappingIndex != InvalidMappingIndex);
}

void JoyMapping::onJoyDisconnected(const JoyConnectionEvent &event)
{
#ifdef WITH_SDL
	// Compacting the array of mapping indices
	for (int i = event.joyId; i < MaxNumJoysticks - 1; i++)
		mappingIndices_[i] = mappingIndices_[i + 1];
	mappingIndices_[MaxNumJoysticks - 1] = InvalidMappingIndex;
#else
	mappingIndices_[event.joyId] = InvalidMappingIndex;
#endif
}

bool JoyMapping::isJoyMapped(int joyId) const
{
	bool result = false;

	if (joyId >= 0 && joyId < MaxNumJoysticks)
		result = mappingIndices_[joyId] != InvalidMappingIndex;

	return result;
}

const JoyMappedStateImpl &JoyMapping::joyMappedState(int joyId) const
{
	if (joyId < 0 || joyId > MaxNumJoysticks)
		return nullMappedJoyState_;
	else
		return mappedJoyStates_[joyId];
}

void JoyMapping::deadZoneNormalize(Vector2f &joyVector, float deadZoneValue) const
{
	deadZoneValue = nctl::clamp(deadZoneValue, 0.0f, 1.0f);

	if (joyVector.length() <= deadZoneValue)
		joyVector = Vector2f::Zero;
	else
	{
		float normalizedLength = (joyVector.length() - deadZoneValue) / (1.0f - deadZoneValue);
		normalizedLength = nctl::clamp(normalizedLength, 0.0f, 1.0f);
		joyVector = joyVector.normalize() * normalizedLength;
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

#ifdef __ANDROID__
const int NumAndroidButtons = 15;
const ButtonName AndroidButtonNameMapping[NumAndroidButtons] = {
	ButtonName::A,
	ButtonName::B,
	ButtonName::X,
	ButtonName::Y,
	ButtonName::LBUMPER,
	ButtonName::RBUMPER,
	ButtonName::LSTICK,
	ButtonName::RSTICK,
	ButtonName::START,
	ButtonName::BACK,
	ButtonName::GUIDE
};

const int NumAndroidAxes = 6;
const AxisName AndroidAxisNameMapping[NumAndroidAxes] = {
	AxisName::LX,
	AxisName::LY,
	AxisName::RX,
	AxisName::RY,
	AxisName::LTRIGGER,
	AxisName::RTRIGGER
};

const int NumAndroidDpadButtons = 4;
const ButtonName AndroidDpadButtonNameMapping[NumAndroidDpadButtons] = {
	ButtonName::DPAD_UP,
	ButtonName::DPAD_RIGHT,
	ButtonName::DPAD_DOWN,
	ButtonName::DPAD_LEFT
};

JoyMapping::MappedJoystick JoyMapping::createAndroidDefaultMapping() const
{
	MappedJoystick mapping;
	strncpy(mapping.name, "android", MaxNameLength);

	for (unsigned int i = 0; i < NumAndroidAxes; i++)
	{
		mapping.axes[i].name = AndroidAxisNameMapping[i];
		mapping.axes[i].min = -1.0f;
		mapping.axes[i].max = 1.0f;
	}

	for (unsigned int i = 0; i < NumAndroidButtons; i++)
		mapping.buttons[i] = AndroidButtonNameMapping[i];

	for (unsigned int i = 0; i < NumAndroidDpadButtons; i++)
		mapping.hats[i] = AndroidDpadButtonNameMapping[i];

	return mapping;
}
#endif

void JoyMapping::checkConnectedJoystics()
{
	for (int i = 0; i < MaxNumJoysticks; i++)
	{
		if (inputManager_->isJoyPresent(i))
		{
			JoyConnectionEvent event;
			event.joyId = i;
			onJoyConnected(event);
		}
	}
}

int JoyMapping::findMappingByGuid(const MappedJoystick::Guid &guid) const
{
	int index = InvalidMappingIndex;

	const unsigned int size = mappings_.size();
	for (unsigned int i = 0; i < size; i++)
	{
		if (mappings_[i].guid == guid)
		{
			index = static_cast<int>(i);
			break;
		}
	}

	return index;
}

int JoyMapping::findMappingByName(const char *name) const
{
	int index = InvalidMappingIndex;

	const unsigned int size = mappings_.size();
	for (unsigned int i = 0; i < size; i++)
	{
		if (strncmp(mappings_[i].name, name, MaxNameLength) == 0)
		{
			index = static_cast<int>(i);
			break;
		}
	}

	return index;
}

bool JoyMapping::parseMappingFromString(const char *mappingString, MappedJoystick &map)
{
	// Early out if the string is empty or a comment
	if (mappingString[0] == '\0' ||
	    mappingString[0] == '\n' ||
	    mappingString[0] == '#')
	{
		return false;
	}

	const char *end = mappingString + strlen(mappingString);
	const char *subStartUntrimmed = mappingString;
	const char *subEndUntrimmed = strchr(subStartUntrimmed, ',');

	const char *subStart = subStartUntrimmed;
	const char *subEnd = subEndUntrimmed;
	trimSpaces(&subStart, &subEnd);

	if (subEndUntrimmed == nullptr)
	{
		LOGE("Invalid mapping string");
		return false;
	}
	unsigned int subLength = static_cast<unsigned int>(subEnd - subStart);

	const unsigned int GuidNumCharacters = 32;
	if (subLength > GuidNumCharacters)
	{
		LOGE_X("GUID length is %u instead of %u characters", subLength, GuidNumCharacters);
		return false;
	}
	map.guid.fromString(subStart);

	subStartUntrimmed = subEndUntrimmed + 1; // GUID plus the following ',' character
	subEndUntrimmed = strchr(subStartUntrimmed, ',');
	if (subEndUntrimmed == nullptr)
	{
		LOGE("Invalid mapping string");
		return false;
	}
	subStart = subStartUntrimmed;
	subEnd = subEndUntrimmed;
	trimSpaces(&subStart, &subEnd);

	subLength = static_cast<unsigned int>(subEnd - subStart);
	memcpy(map.name, subStart, nctl::min(subLength, MaxNameLength));
	map.name[nctl::min(subLength, MaxNameLength)] = '\0';

	subStartUntrimmed = subEndUntrimmed + 1; // name plus the following ',' character
	subEndUntrimmed = strchr(subStartUntrimmed, ',');
	while (subStartUntrimmed < end && *subStartUntrimmed != '\n')
	{
		subStart = subStartUntrimmed;
		subEnd = subEndUntrimmed;
		trimSpaces(&subStart, &subEnd);

		const char *subMid = strchr(subStart, ':');
		if (subMid == nullptr || subEnd == nullptr)
		{
			LOGE("Invalid mapping string");
			return false;
		}

		const bool keywordFound = parsePlatformKeyword(subStart, subMid);
		if (keywordFound)
		{
			const bool thisPlatform = parsePlatformName(subMid + 1, subEnd);
			if (thisPlatform == false)
				return false;
		}
		else
		{
			const int axisIndex = parseAxisName(subStart, subMid);
			if (axisIndex != -1)
			{
				MappedJoystick::Axis axis;
				axis.name = static_cast<AxisName>(axisIndex);
				const int axisMapping = parseAxisMapping(subMid + 1, subEnd, axis);
				if (axisMapping != -1 && axisMapping < MappedJoystick::MaxNumAxes)
					map.axes[axisMapping] = axis;
				else
				{
					// The same parsing method for buttons will be used for button axes
					const int buttonAxisMapping = parseButtonMapping(subMid + 1, subEnd);
					if (buttonAxisMapping != -1 && buttonAxisMapping < MappedJoystick::MaxNumAxes)
						map.buttonAxes[buttonAxisMapping] = static_cast<AxisName>(axisIndex);
				}
			}
			else
			{
				const int buttonIndex = parseButtonName(subStart, subMid);
				if (buttonIndex != -1)
				{
					const int buttonMapping = parseButtonMapping(subMid + 1, subEnd);
					if (buttonMapping != -1 && buttonMapping < MappedJoystick::MaxNumButtons)
						map.buttons[buttonMapping] = static_cast<ButtonName>(buttonIndex);
					else
					{
						const int hatMapping = parseHatMapping(subMid + 1, subEnd);
						if (hatMapping != -1 && hatMapping < MappedJoystick::MaxHatButtons)
							map.hats[hatMapping] = static_cast<ButtonName>(buttonIndex);
						else
						{
							const unsigned int DebugMapStringMaxLength = 8;
							char debugMapString[DebugMapStringMaxLength];
							const int debugMapStringLength = subEnd - (subMid + 1);
							const int debugMapStringClampedLength = (debugMapStringLength < DebugMapStringMaxLength - 1)
							        ? debugMapStringLength : DebugMapStringMaxLength - 1;
							debugMapString[debugMapStringClampedLength] = '\0';

							MappedJoystick::Axis axis;
							const int axisMapping = parseAxisMapping(subMid + 1, subEnd, axis);
							if (axisMapping != -1 && axisMapping < MappedJoystick::MaxNumAxes)
							{
								if (axis.max > 0.0f)
									map.axes[axisMapping].positiveButtonName = static_cast<ButtonName>(buttonIndex);
								else if (axis.max < 0.0f)
									map.axes[axisMapping].negativeButtonName = static_cast<ButtonName>(buttonIndex);
								else
								{
									memcpy(debugMapString, subMid + 1, debugMapStringClampedLength);
									LOGI_X("Unsupported axis value \"%s\" for button mapping in \"%s\"", debugMapString, map.name);
								}
							}
							else if (subMid + 1 < subEnd)
							{
								// Sometimes it is empty
								memcpy(debugMapString, subMid + 1, debugMapStringClampedLength);
								LOGI_X("Unsupported mapping source \"%s\" in \"%s\"", debugMapString, map.name);
							}
						}
					}
				}
			}
		}

		subStartUntrimmed = subEndUntrimmed + 1;
		if (subStartUntrimmed < end)
		{
			subEndUntrimmed = strchr(subStartUntrimmed, ',');
			if (subEndUntrimmed == nullptr)
				subEndUntrimmed = end;
		}
	}

	return true;
}

bool JoyMapping::parsePlatformKeyword(const char *start, const char *end) const
{
	bool keywordFound = false;

	if (strncmp(start, "platform", end - start) == 0)
		keywordFound = true;

	return keywordFound;
}

bool JoyMapping::parsePlatformName(const char *start, const char *end) const
{
	bool thisPlatform = false;

#if defined(_WIN32)
	const char *platformName = "Windows";
#elif defined(__APPLE__)
	const char *platformName = "Mac OS X";
#elif defined(__ANDROID__)
	const char *platformName = "Android";
#else
	const char *platformName = "Linux";
#endif

	if (strncmp(start, platformName, end - start) == 0)
		thisPlatform = true;

	return thisPlatform;
}

int JoyMapping::parseAxisName(const char *start, const char *end) const
{
	int axisIndex = -1;

	for (unsigned int i = 0; i < JoyMappedState::NumAxes; i++)
	{
		if (strncmp(start, AxesStrings[i], end - start) == 0)
		{
			axisIndex = i;
			break;
		}
	}

	return axisIndex;
}

int JoyMapping::parseButtonName(const char *start, const char *end) const
{
	int buttonIndex = -1;

	for (unsigned int i = 0; i < JoyMappedState::NumButtons; i++)
	{
		if (strncmp(start, ButtonsStrings[i], end - start) == 0)
		{
			buttonIndex = i;
			break;
		}
	}

	return buttonIndex;
}

int JoyMapping::parseAxisMapping(const char *start, const char *end, MappedJoystick::Axis &axis) const
{
	int axisMapping = -1;

	axis.max = 1.0f;
	axis.min = -1.0f;

	if (end - start <= 5 && (start[0] == 'a' || start[1] == 'a'))
	{
		const char *digits = &start[1];

		if (axis.name == AxisName::LTRIGGER || axis.name == AxisName::RTRIGGER)
		{
			axis.min = 0.0f;
			axis.max = 1.0f;
		}

		if (start[0] == '+')
		{
			axis.min = 0.0f;
			axis.max = 1.0f;
			digits++;
		}
		else if (start[0] == '-')
		{
			axis.min = 0.0f;
			axis.max = -1.0f;
			digits++;
		}

		axisMapping = atoi(digits);

		if (end[-1] == '~')
		{
			const float temp = axis.min;
			axis.min = axis.max;
			axis.max = temp;
		}
	}

	return axisMapping;
}

int JoyMapping::parseButtonMapping(const char *start, const char *end) const
{
	int buttonMapping = -1;

	if (end - start <= 3 && start[0] == 'b')
		buttonMapping = atoi(&start[1]);

	return buttonMapping;
}

int JoyMapping::parseHatMapping(const char *start, const char *end) const
{
	int hatMapping = -1;

	int parsedHatMapping = -1;
	if (end - start <= 4 && start[0] == 'h')
		parsedHatMapping = atoi(&start[3]);

	// `h0.0` is not considered a valid mapping
	if (parsedHatMapping > 0)
		hatMapping = hatStateToIndex(parsedHatMapping);

	return hatMapping;
}

int JoyMapping::hatStateToIndex(unsigned char hatState) const
{
	int hatIndex = -1;

	switch (hatState)
	{
		case 1: hatIndex = 0; break;
		case 2: hatIndex = 1; break;
		case 4: hatIndex = 2; break;
		case 8: hatIndex = 3; break;
		default: hatIndex = -1; break;
	}

	return hatIndex;
}

void JoyMapping::trimSpaces(const char **start, const char **end) const
{
	while (**start == ' ' || **start == '\t')
		(*start)++;

	(*end)--;
	while (**end == ' ' || **end == '\t')
		(*end)--;
	(*end)++;
}

}
