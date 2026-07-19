#include "common_macros.h"
#include <nctl/CString.h>
#include "QtInputManager.h"
#include "QtWidget.h"
#include "IInputEventHandler.h"
#include "PCApplication.h"
#include "JoyMapping.h"

#include <qevent.h>
#include <QWidget>
#include <QMimeData>

#ifdef WITH_QT5GAMEPAD
	#include <QtGamepad/QGamepadManager>
	#include <QtGamepad/QGamepad>
#endif

#ifdef WITH_IMGUI
	#include "ImGuiQtInput.h"
#endif

#ifdef WITH_NUKLEAR
	#include "NuklearQtInput.h"
#endif

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

const int IInputManager::MaxNumJoysticks = 4;
const unsigned short int IInputManager::MaxVibrationValue = 0;

TouchEvent QtInputManager::touchEvent_;
QtMouseState QtInputManager::mouseState_;
MouseEvent QtInputManager::mouseEvent_;
QtScrollEvent QtInputManager::scrollEvent_;
QtKeyboardState QtInputManager::keyboardState_;
KeyboardEvent QtInputManager::keyboardEvent_;
TextInputEvent QtInputManager::textInputEvent_;
QtJoystickState QtInputManager::nullJoystickState_;

#ifdef WITH_QT5GAMEPAD
QtJoystickState QtInputManager::joystickStates_[MaxNumJoysticks];
JoyButtonEvent QtInputManager::joyButtonEvent_;
JoyHatEvent QtInputManager::joyHatEvent_;
JoyAxisEvent QtInputManager::joyAxisEvent_;
JoyConnectionEvent QtInputManager::joyConnectionEvent_;
const float QtJoystickState::AxisEventTolerance = 0.001f;
#endif

namespace {

	MouseButton qtToNcineMouseButton(int button)
	{
		if (button == Qt::LeftButton)
			return MouseButton::LEFT;
		else if (button == Qt::MiddleButton)
			return MouseButton::MIDDLE;
		else if (button == Qt::RightButton)
			return MouseButton::RIGHT;
		else if (button == Qt::BackButton)
			return MouseButton::FOURTH;
		else if (button == Qt::ForwardButton)
			return MouseButton::FIFTH;
		else
			return MouseButton::LEFT;
	}

	bool checkMouseButton(int buttonState, MouseButton button)
	{
		switch (button)
		{
			case MouseButton::LEFT: return ((buttonState & Qt::LeftButton) != 0);
			case MouseButton::MIDDLE: return ((buttonState & Qt::MiddleButton) != 0);
			case MouseButton::RIGHT: return ((buttonState & Qt::RightButton) != 0);
			case MouseButton::FOURTH: return ((buttonState & Qt::BackButton) != 0);
			case MouseButton::FIFTH: return ((buttonState & Qt::ForwardButton) != 0);
			default: return false;
		}
	}

	void mouseEventPos(const QMouseEvent *event, int &x, int &y)
	{
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
		const QPointF pos = event->position();
		x = qRound(pos.x());
		y = qRound(pos.y());
#else
		x = event->x();
		y = event->y();
#endif
	}

}

///////////////////////////////////////////////////////////
// QtMouseState
///////////////////////////////////////////////////////////

QtMouseState::QtMouseState()
    : currentStateIndex_(0), buttonStates_{Qt::NoButton, Qt::NoButton} {}

bool QtMouseState::isButtonDown(MouseButton button) const
{
	return checkMouseButton(buttonStates_[currentStateIndex_], button);
}

bool QtMouseState::isButtonPressed(MouseButton button) const
{
	const unsigned int prevStateIndex = (currentStateIndex_ == 0 ? 1 : 0);
	return (checkMouseButton(buttonStates_[currentStateIndex_], button) == true &&
	        checkMouseButton(buttonStates_[prevStateIndex], button) == false);
}

bool QtMouseState::isButtonReleased(MouseButton button) const
{
	const unsigned int prevStateIndex = (currentStateIndex_ == 0 ? 1 : 0);
	return (checkMouseButton(buttonStates_[currentStateIndex_], button) == false &&
	        checkMouseButton(buttonStates_[prevStateIndex], button) == true);
}

void QtMouseState::copyButtonStateToPrev()
{
	const unsigned int prevStateIndex = (currentStateIndex_ == 0 ? 1 : 0);
	buttonStates_[prevStateIndex] = buttonStates_[currentStateIndex_];
	currentStateIndex_ = prevStateIndex;
}

///////////////////////////////////////////////////////////
// QtKeyboardState
///////////////////////////////////////////////////////////

QtKeyboardState::QtKeyboardState()
    : currentStateIndex_(0)
{
	memset(keys_[0], 0, NumKeys * sizeof(unsigned char));
	memset(keys_[1], 0, NumKeys * sizeof(unsigned char));
}

bool QtKeyboardState::isKeyDown(KeySym key) const
{
	const unsigned int keyIndex = static_cast<unsigned int>(key);
	if (key == KeySym::UNKNOWN)
		return false;
	else
		return keys_[currentStateIndex_][keyIndex] != 0;
}

bool QtKeyboardState::isKeyPressed(KeySym key) const
{
	const unsigned int prevStateIndex = (currentStateIndex_ == 0 ? 1 : 0);
	const unsigned int keyIndex = static_cast<unsigned int>(key);
	if (key == KeySym::UNKNOWN)
		return false;
	else
		return keys_[currentStateIndex_][keyIndex] != 0 && keys_[prevStateIndex][keyIndex] == 0;
}

bool QtKeyboardState::isKeyReleased(KeySym key) const
{
	const unsigned int prevStateIndex = (currentStateIndex_ == 0 ? 1 : 0);
	const unsigned int keyIndex = static_cast<unsigned int>(key);
	if (key == KeySym::UNKNOWN)
		return false;
	else
		return keys_[currentStateIndex_][keyIndex] == 0 && keys_[prevStateIndex][keyIndex] != 0;
}

void QtKeyboardState::copyKeyStateToPrev()
{
	const unsigned int prevStateIndex = (currentStateIndex_ == 0 ? 1 : 0);
	memcpy(keys_[prevStateIndex], keys_[currentStateIndex_], NumKeys * sizeof(unsigned char));
	currentStateIndex_ = prevStateIndex;
}

///////////////////////////////////////////////////////////
// QtJoystickState
///////////////////////////////////////////////////////////

#ifdef WITH_QT5GAMEPAD
bool gamepadButtonState(const QGamepad &gamepad, int buttonId)
{
	switch (buttonId)
	{
		case 0: return gamepad.buttonL1();
		case 1: return gamepad.buttonL3();
		case 2: return gamepad.buttonR1();
		case 3: return gamepad.buttonR3();
		case 4: return gamepad.buttonA();
		case 5: return gamepad.buttonB();
		case 6: return gamepad.buttonCenter();
		case 7: return gamepad.buttonGuide();
		case 8: return gamepad.buttonSelect();
		case 9: return gamepad.buttonStart();
		case 10: return gamepad.buttonX();
		case 11: return gamepad.buttonY();
		default: return false;
	}
}

QtJoystickState::QtJoystickState()
{
	memset(buttonState_[0], 0, NumButtons * sizeof(unsigned char));
	memset(buttonState_[1], 0, NumButtons * sizeof(unsigned char));

	hatState_ = HatState::CENTERED;
	for (unsigned int i = 0; i < NumAxes; i++)
		axesValuesState_[i] = 0.0f;
}

bool QtJoystickState::isButtonDown(int buttonId) const
{
	return gamepadButtonState(*gamepad_, buttonId);
}

bool QtJoystickState::isButtonPressed(int buttonId) const
{
	ASSERT(buttonId < NumButtons);
	const unsigned int prevStateIndex = (currentStateIndex_ == 0 ? 1 : 0);
	bool isPressed = false;
	if (buttonId >= 0 && buttonId < NumButtons)
		isPressed = (buttonState_[currentStateIndex_][buttonId] == 1 && buttonState_[prevStateIndex][buttonId] == 0);
	return isPressed;
}

bool QtJoystickState::isButtonReleased(int buttonId) const
{
	ASSERT(buttonId < NumButtons);
	const unsigned int prevStateIndex = (currentStateIndex_ == 0 ? 1 : 0);
	bool isReleased = false;
	if (buttonId >= 0 && buttonId < NumButtons)
		isReleased = (buttonState_[currentStateIndex_][buttonId] == 0 && buttonState_[prevStateIndex][buttonId] == 1);
	return isReleased;
}

unsigned char QtJoystickState::hatState(int hatId) const
{
	unsigned char state = HatState::CENTERED;

	if (hatId == 0)
	{
		if (gamepad_->buttonUp())
			state += HatState::UP;
		else if (gamepad_->buttonDown())
			state += HatState::DOWN;

		if (gamepad_->buttonRight())
			state += HatState::RIGHT;
		else if (gamepad_->buttonLeft())
			state += HatState::LEFT;
	}

	return state;
}

short int QtJoystickState::axisValue(int axisId) const
{
	switch (axisId)
	{
		case 0: return static_cast<short int>(gamepad_->axisLeftX() * IInputManager::MaxAxisValue);
		case 1: return static_cast<short int>(gamepad_->axisLeftY() * IInputManager::MaxAxisValue);
		case 2: return static_cast<short int>(gamepad_->axisRightX() * IInputManager::MaxAxisValue);
		case 3: return static_cast<short int>(gamepad_->axisRightY() * IInputManager::MaxAxisValue);
		case 4: return static_cast<short int>((2.0 * gamepad_->buttonL2() - 1.0) * IInputManager::MaxAxisValue);
		case 5: return static_cast<short int>((2.0 * gamepad_->buttonR2() - 1.0) * IInputManager::MaxAxisValue);
		default: return 0U;
	}
}

float QtJoystickState::axisNormValue(int axisId) const
{
	switch (axisId)
	{
		case 0: return static_cast<float>(gamepad_->axisLeftX());
		case 1: return static_cast<float>(gamepad_->axisLeftY());
		case 2: return static_cast<float>(gamepad_->axisRightX());
		case 3: return static_cast<float>(gamepad_->axisRightY());
		case 4: return static_cast<float>(2.0 * gamepad_->buttonL2() - 1.0);
		case 5: return static_cast<float>(2.0 * gamepad_->buttonR2() - 1.0);
		default: return 0.0f;
	}
}

void QtJoystickState::copyButtonStateToPrev()
{
	const unsigned int prevStateIndex = (currentStateIndex_ == 0 ? 1 : 0);
	memcpy(buttonState_[prevStateIndex], buttonState_[currentStateIndex_], NumButtons * sizeof(unsigned char));
	currentStateIndex_ = prevStateIndex;
}

void QtJoystickState::resetPrevButtonState()
{
	const unsigned int prevStateIndex = (currentStateIndex_ == 0 ? 1 : 0);
	memset(buttonState_[prevStateIndex], 0, NumButtons * sizeof(unsigned char));
}
#endif

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

QtInputManager::QtInputManager(QtWidget &widget)
    : widget_(widget)
{
#ifdef WITH_QT5GAMEPAD
	for (int i = 0; i < MaxNumJoysticks; i++)
		joystickStates_[i].gamepad_ = nctl::makeUnique<QGamepad>(-1);
#endif

	joyMapping_.init(this);

#ifdef WITH_IMGUI
	ImGuiQtInput::init(&widget);
#endif

#ifdef WITH_NUKLEAR
	NuklearQtInput::init(&widget);
#endif
}

QtInputManager::~QtInputManager()
{
#ifdef WITH_NUKLEAR
	NuklearQtInput::shutdown();
#endif

#ifdef WITH_IMGUI
	ImGuiQtInput::shutdown();
#endif
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

#ifdef WITH_QT5GAMEPAD
void QtInputManager::updateJoystickStates()
{
	// Compacting the array of Qt connected gamepads
	int notConnectedIndex = 0;
	int connectedIndex = 0;
	while (notConnectedIndex < MaxNumJoysticks)
	{
		QGamepad &notConnected = *joystickStates_[notConnectedIndex].gamepad_;
		QGamepad &connected = *joystickStates_[connectedIndex].gamepad_;
		if (notConnected.isConnected() && connected.isConnected() == false)
		{
			const int notConnectedId = notConnected.deviceId();
			notConnected.setDeviceId(connected.deviceId());
			connected.setDeviceId(notConnectedId);

			connectedIndex++;
		}

		notConnectedIndex++;
	}

	if (inputEventHandler_)
	{
		for (int i = 0; i < MaxNumJoysticks; i++)
		{
			if (joystickStates_[i].gamepad_->deviceId() > -1 &&
			    joystickStates_[i].gamepad_->isConnected() == false)
			{
				joystickStates_[i].resetPrevButtonState();
				joystickStates_[i].gamepad_->setDeviceId(-1);
				joyConnectionEvent_.joyId = i;
				inputEventHandler_->onJoyDisconnected(joyConnectionEvent_);
				joyMapping_.onJoyDisconnected(joyConnectionEvent_);
			}
		}
	}

	const QList<int> gamepads = QGamepadManager::instance()->connectedGamepads();
	int index = 0;
	for (int deviceId : gamepads)
	{
		const int oldDeviceId = joystickStates_[index].gamepad_->deviceId();

		if (inputEventHandler_ && oldDeviceId != deviceId)
		{
			joystickStates_[index].gamepad_->setDeviceId(deviceId);
			joyConnectionEvent_.joyId = index;
			joyMapping_.onJoyConnected(joyConnectionEvent_);
			inputEventHandler_->onJoyConnected(joyConnectionEvent_);
		}

		index++;
		if (index >= MaxNumJoysticks)
			break;
	}

	for (int joyId = 0; joyId < MaxNumJoysticks; joyId++)
	{
		QtJoystickState &state = joystickStates_[joyId];
		if (state.gamepad_ == nullptr)
			continue;

		for (unsigned int buttonId = 0; buttonId < QtJoystickState::NumButtons; buttonId++)
		{
			const bool newButtonState = state.isButtonDown(buttonId);
			const bool oldButtonState = state.buttonState_[state.currentStateIndex_][buttonId];

			if (oldButtonState != newButtonState)
			{
				state.buttonState_[state.currentStateIndex_][buttonId] = newButtonState;
				if (inputEventHandler_ != nullptr)
				{
					joyButtonEvent_.joyId = joyId;
					joyButtonEvent_.buttonId = buttonId;
					if (newButtonState)
					{
						joyMapping_.onJoyButtonPressed(joyButtonEvent_);
						inputEventHandler_->onJoyButtonPressed(joyButtonEvent_);
					}
					else
					{
						joyMapping_.onJoyButtonReleased(joyButtonEvent_);
						inputEventHandler_->onJoyButtonReleased(joyButtonEvent_);
					}
				}
			}
		}

		const unsigned char newHatState = state.hatState(0);
		if (state.hatState_ != newHatState)
		{
			state.hatState_ = newHatState;
			if (inputEventHandler_ != nullptr)
			{
				joyHatEvent_.joyId = joyId;
				joyHatEvent_.hatId = 0;
				joyHatEvent_.hatState = newHatState;
				joyMapping_.onJoyHatMoved(joyHatEvent_);
				inputEventHandler_->onJoyHatMoved(joyHatEvent_);
			}
		}

		for (unsigned int axisId = 0; axisId < QtJoystickState::NumAxes; axisId++)
		{
			const float newAxisValue = state.axisNormValue(axisId);
			if (fabsf(state.axesValuesState_[axisId] - newAxisValue) > QtJoystickState::AxisEventTolerance)
			{
				state.axesValuesState_[axisId] = newAxisValue;
				if (inputEventHandler_)
				{
					joyAxisEvent_.joyId = joyId;
					joyAxisEvent_.axisId = axisId;
					joyAxisEvent_.value = static_cast<short int>(newAxisValue * MaxAxisValue);
					joyAxisEvent_.normValue = newAxisValue;
					joyMapping_.onJoyAxisMoved(joyAxisEvent_);
					inputEventHandler_->onJoyAxisMoved(joyAxisEvent_);
				}
			}
		}
	}
}
#endif

void QtInputManager::copyButtonStatesToPrev()
{
	mouseState_.copyButtonStateToPrev();
	keyboardState_.copyKeyStateToPrev();

#ifdef WITH_QT5GAMEPAD
	for (unsigned int joyId = 0; joyId < MaxNumJoysticks; joyId++)
		joystickStates_[joyId].copyButtonStateToPrev();
	joyMapping_.copyButtonStateToPrev();
#endif
}

bool QtInputManager::shouldQuitOnRequest()
{
	bool shouldQuit = true;

	if (inputEventHandler_)
		shouldQuit = inputEventHandler_->onQuitRequest();

	return shouldQuit;
}

bool QtInputManager::event(QEvent *event)
{
#ifdef WITH_IMGUI
	ImGuiQtInput::event(event);
#endif

#ifdef WITH_NUKLEAR
	NuklearQtInput::event(event);
#endif

	switch (event->type())
	{
		case QEvent::KeyPress:
			keyPressEvent(static_cast<QKeyEvent *>(event));
			return true;
		case QEvent::KeyRelease:
			keyReleaseEvent(static_cast<QKeyEvent *>(event));
			return true;
		case QEvent::MouseButtonPress:
			mousePressEvent(static_cast<QMouseEvent *>(event));
			return true;
		case QEvent::MouseButtonRelease:
			mouseReleaseEvent(static_cast<QMouseEvent *>(event));
			return true;
		case QEvent::MouseMove:
			mouseMoveEvent(static_cast<QMouseEvent *>(event));
			return true;
		case QEvent::TouchBegin:
			touchBeginEvent(static_cast<QTouchEvent *>(event));
			return true;
		case QEvent::TouchUpdate:
			touchUpdateEvent(static_cast<QTouchEvent *>(event));
			return true;
		case QEvent::TouchEnd:
			touchEndEvent(static_cast<QTouchEvent *>(event));
			return true;
		case QEvent::Wheel:
			wheelEvent(static_cast<QWheelEvent *>(event));
			return true;
		case QEvent::DragEnter:
			static_cast<QDragEnterEvent *>(event)->acceptProposedAction();
			return true;
		case QEvent::Drop:
			dropEvent(static_cast<QDropEvent *>(event));
			return true;
		default:
			return false;
	}
}

void QtInputManager::keyPressEvent(QKeyEvent *event)
{
	if (inputEventHandler_)
	{
		keyboardEvent_.scancode = static_cast<int>(event->nativeScanCode());
		keyboardEvent_.sym = QtKeys::keySymValueToEnum(event->key());
		keyboardEvent_.mod = QtKeys::keyModMaskToEnumMask(event->modifiers());
		if (keyboardEvent_.sym != KeySym::UNKNOWN)
		{
			const unsigned int keySym = static_cast<unsigned int>(keyboardEvent_.sym);
			keyboardState_.keys_[keyboardState_.currentStateIndex_][keySym] = 1;
		}
		inputEventHandler_->onKeyPressed(keyboardEvent_);

		if (event->text().length() > 0)
		{
			nctl::strncpy(textInputEvent_.text, event->text().toUtf8().constData(), 4);
			inputEventHandler_->onTextInput(textInputEvent_);
		}
	}
}

void QtInputManager::keyReleaseEvent(QKeyEvent *event)
{
	if (inputEventHandler_)
	{
		keyboardEvent_.scancode = static_cast<int>(event->nativeScanCode());
		keyboardEvent_.sym = QtKeys::keySymValueToEnum(event->key());
		keyboardEvent_.mod = QtKeys::keyModMaskToEnumMask(event->modifiers());
		if (keyboardEvent_.sym != KeySym::UNKNOWN)
		{
			const unsigned int keySym = static_cast<unsigned int>(keyboardEvent_.sym);
			keyboardState_.keys_[keyboardState_.currentStateIndex_][keySym] = 0;
		}
		inputEventHandler_->onKeyReleased(keyboardEvent_);
	}
}

void QtInputManager::mousePressEvent(QMouseEvent *event)
{
	if (inputEventHandler_)
	{
		int x = 0;
		int y = 0;
		mouseEventPos(event, x, y);

		mouseEvent_.x = x;
		mouseEvent_.y = theApplication().heightInt() - y;
		mouseEvent_.button = qtToNcineMouseButton(event->button());
		mouseState_.buttonStates_[mouseState_.currentStateIndex_] = event->buttons();
		inputEventHandler_->onMouseButtonPressed(mouseEvent_);
	}
}

void QtInputManager::mouseReleaseEvent(QMouseEvent *event)
{
	if (inputEventHandler_)
	{
		int x = 0;
		int y = 0;
		mouseEventPos(event, x, y);

		mouseEvent_.x = x;
		mouseEvent_.y = theApplication().heightInt() - y;
		mouseEvent_.button = qtToNcineMouseButton(event->button());
		mouseState_.buttonStates_[mouseState_.currentStateIndex_] = event->buttons();
		inputEventHandler_->onMouseButtonReleased(mouseEvent_);
	}
}

void QtInputManager::mouseMoveEvent(QMouseEvent *event)
{
	if (inputEventHandler_)
	{
		int x = 0;
		int y = 0;
		mouseEventPos(event, x, y);

		mouseState_.x = x;
		mouseState_.y = theApplication().heightInt() - y;
		mouseState_.buttonStates_[mouseState_.currentStateIndex_] = event->buttons();
		inputEventHandler_->onMouseMoved(mouseState_);
	}
}

void QtInputManager::touchBeginEvent(QTouchEvent *event)
{
	if (inputEventHandler_)
	{
		updateTouchEvent(event);
		inputEventHandler_->onTouchDown(touchEvent_);
	}
}

void QtInputManager::touchUpdateEvent(QTouchEvent *event)
{
	if (inputEventHandler_)
	{
		const unsigned int previousCount = touchEvent_.count;
		updateTouchEvent(event);
		if (previousCount < touchEvent_.count)
			inputEventHandler_->onPointerDown(touchEvent_);
		else if (previousCount > touchEvent_.count)
			inputEventHandler_->onPointerUp(touchEvent_);
		else if (previousCount == touchEvent_.count)
			inputEventHandler_->onTouchMove(touchEvent_);
	}
}

void QtInputManager::touchEndEvent(QTouchEvent *event)
{
	if (inputEventHandler_)
	{
		updateTouchEvent(event);
		inputEventHandler_->onTouchUp(touchEvent_);
	}
}

void QtInputManager::wheelEvent(QWheelEvent *event)
{
	if (inputEventHandler_)
	{
		scrollEvent_.x = event->angleDelta().x() / 120.0f;
		scrollEvent_.y = event->angleDelta().y() / 120.0f;
		inputEventHandler_->onScrollInput(scrollEvent_);
	}
}

void QtInputManager::dropEvent(QDropEvent *event)
{
	if (inputEventHandler_)
	{
		const QMimeData *mimeData = event->mimeData();
		if (mimeData->hasUrls())
		{
			const QList<QUrl> urlList = mimeData->urls();
			int destIndex = 0;
			for (int srcIndex = 0; srcIndex < urlList.size(); srcIndex++)
			{
				const QString text = urlList[srcIndex].toLocalFile();
				// Skip long paths instead of truncating them
				if (text.length() > DropEvent::MaxPathLength - 1)
					continue;

				for (int i = 0; i < text.length(); i++)
					dropEvent_.paths[destIndex][i] = static_cast<char>(text.at(i).toLatin1());
				destIndex++;

				if (destIndex >= DropEvent::MaxNumPaths)
					break;
			}
			dropEvent_.numPaths = destIndex;

			if (dropEvent_.numPaths > 0)
				inputEventHandler_->onFilesDropped(dropEvent_);
		}
	}
}

#ifdef WITH_QT5GAMEPAD
bool QtInputManager::isJoyPresent(int joyId) const
{
	ASSERT(joyId >= 0);
	ASSERT_MSG_X(joyId < int(MaxNumJoysticks), "joyId is %d and the maximum is %u", joyId, MaxNumJoysticks - 1);
	if (joyId < 0 || joyId >= int(MaxNumJoysticks))
		return false;

	return joystickStates_[joyId].gamepad_->isConnected();
}

const char *QtInputManager::joyName(int joyId) const
{
	if (isJoyPresent(joyId))
		return joystickStates_[joyId].name_;
	else
		return nullptr;
}

const JoystickState &QtInputManager::joystickState(int joyId) const
{
	if (isJoyPresent(joyId))
		return joystickStates_[joyId];
	else
		return nullJoystickState_;
}
#endif

void QtInputManager::setMouseCursorMode(MouseCursorMode mode)
{
	if (mode != mouseCursorMode_)
	{
		switch (mode)
		{
			case MouseCursorMode::NORMAL:
				widget_.unsetCursor();
				widget_.releaseMouse();
				break;
			case MouseCursorMode::HIDDEN:
				widget_.setCursor(QCursor(Qt::BlankCursor));
				widget_.releaseMouse();
				break;
			case MouseCursorMode::DISABLED:
				widget_.grabMouse(QCursor(Qt::BlankCursor));
				break;
		}

		// Handling ImGui cursor changes
		IInputManager::setMouseCursorMode(mode);

		mouseCursorMode_ = mode;
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////

void QtInputManager::updateTouchEvent(const QTouchEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	const QList<QEventPoint> &touchPoints = event->points();
#else
	const QList<QTouchEvent::TouchPoint> &touchPoints = event->touchPoints();
#endif

	touchEvent_.count = touchPoints.size();
	for (unsigned int i = 0; i < touchEvent_.count && i < TouchEvent::MaxPointers; i++)
	{
		TouchEvent::Pointer &pointer = touchEvent_.pointers[i];
		const QTouchEvent::TouchPoint &touchPoint = touchPoints.at(i);

#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
		const QPointF pos = touchPoint.position();
		const int x = qRound(pos.x());
		const int y = qRound(pos.y());
#else
		const int x = touchPoint.pos().x();
		const int y = touchPoint.pos().y();
#endif

		pointer.id = touchPoint.id();
		pointer.x = x;
		pointer.y = theApplication().height() - y;
		pointer.pressure = touchPoint.pressure();
	}
}

}
