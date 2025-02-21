#include "ImGuiQt5Input.h"
#include "IInputManager.h"
#include "Application.h"
#include "Qt5Widget.h"

#include <qevent.h>
#include <QApplication>
#include <QClipboard>

#ifdef WITH_QT5GAMEPAD
	#include <QtGamepad/QGamepadManager>
	#include <QtGamepad/QGamepad>
#endif

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

bool ImGuiQt5Input::inputEnabled_ = true;
bool ImGuiQt5Input::mousePressed_[5] = { false, false, false, false, false };
QCursor ImGuiQt5Input::mouseCursors_[ImGuiMouseCursor_COUNT] = {};
QCursor ImGuiQt5Input::blankCursor_;
Qt5Widget *ImGuiQt5Input::widget_ = nullptr;
nctl::UniquePtr<QGamepad> ImGuiQt5Input::gamepad_;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

namespace {

	char clipboardString[1024];

	const char *clipboardText(ImGuiContext *context)
	{
		QClipboard *clipboard = QApplication::clipboard();
		const int charsToCopy = clipboard->text().length() < 1024 ? clipboard->text().length() : 1024;
		for (int i = 0; i < charsToCopy; i++)
			clipboardString[i] = static_cast<char>(clipboard->text().at(i).cell());

		return clipboardString;
	}

	void setClipboardText(ImGuiContext *context, const char *text)
	{
		QClipboard *clipboard = QApplication::clipboard();
		clipboard->setText(text);
	}

	ImGuiKey qtKeyToImGuiKey(int key)
	{
		switch (key)
		{
			case Qt::Key_Tab: return ImGuiKey_Tab;
			case Qt::Key_Left: return ImGuiKey_LeftArrow;
			case Qt::Key_Right: return ImGuiKey_RightArrow;
			case Qt::Key_Up: return ImGuiKey_UpArrow;
			case Qt::Key_Down: return ImGuiKey_DownArrow;
			case Qt::Key_PageUp: return ImGuiKey_PageUp;
			case Qt::Key_PageDown: return ImGuiKey_PageDown;
			case Qt::Key_Home: return ImGuiKey_Home;
			case Qt::Key_End: return ImGuiKey_End;
			case Qt::Key_Insert: return ImGuiKey_Insert;
			case Qt::Key_Delete: return ImGuiKey_Delete;
			case Qt::Key_Backspace: return ImGuiKey_Backspace;
			case Qt::Key_Space: return ImGuiKey_Space;
			case Qt::Key_Enter: return ImGuiKey_Enter;
			case Qt::Key_Escape: return ImGuiKey_Escape;
			case Qt::Key_Apostrophe: return ImGuiKey_Apostrophe;
			case Qt::Key_Comma: return ImGuiKey_Comma;
			case Qt::Key_Minus: return ImGuiKey_Minus;
			case Qt::Key_Period: return ImGuiKey_Period;
			case Qt::Key_Slash: return ImGuiKey_Slash;
			case Qt::Key_Semicolon: return ImGuiKey_Semicolon;
			case Qt::Key_Equal: return ImGuiKey_Equal;
			case Qt::Key_BracketLeft: return ImGuiKey_LeftBracket;
			case Qt::Key_Backslash: return ImGuiKey_Backslash;
			case Qt::Key_BracketRight: return ImGuiKey_RightBracket;
			case Qt::Key_AsciiTilde: return ImGuiKey_GraveAccent;
			case Qt::Key_CapsLock: return ImGuiKey_CapsLock;
			case Qt::Key_ScrollLock: return ImGuiKey_ScrollLock;
			case Qt::Key_NumLock: return ImGuiKey_NumLock;
			case Qt::Key_Print: return ImGuiKey_PrintScreen;
			case Qt::Key_Pause: return ImGuiKey_Pause;
			// case Qt::Key_0: return ImGuiKey_Keypad0;
			// case Qt::Key_1: return ImGuiKey_Keypad1;
			// case Qt::Key_2: return ImGuiKey_Keypad2;
			// case Qt::Key_3: return ImGuiKey_Keypad3;
			// case Qt::Key_4: return ImGuiKey_Keypad4;
			// case Qt::Key_5: return ImGuiKey_Keypad5;
			// case Qt::Key_6: return ImGuiKey_Keypad6;
			// case Qt::Key_7: return ImGuiKey_Keypad7;
			// case Qt::Key_8: return ImGuiKey_Keypad8;
			// case Qt::Key_9: return ImGuiKey_Keypad9;
			// case Qt::Key_Period: return ImGuiKey_KeypadDecimal;
			// case Qt::Key_Slash: return ImGuiKey_KeypadDivide;
			// case Qt::Key_Asterisk: return ImGuiKey_KeypadMultiply;
			// case Qt::Key_Minus: return ImGuiKey_KeypadSubtract;
			case Qt::Key_Plus: return ImGuiKey_KeypadAdd;
			// case Qt::Key_Enter: return ImGuiKey_KeypadEnter;
			// case Qt::Key_Equal: return ImGuiKey_KeypadEqual;
			case Qt::Key_Shift: return ImGuiKey_LeftShift;
			case Qt::Key_Control: return ImGuiKey_LeftCtrl;
			case Qt::Key_Alt: return ImGuiKey_LeftAlt;
			case Qt::Key_Super_L: return ImGuiKey_LeftSuper;
			// case Qt::Key_Shift: return ImGuiKey_RightShift;
			// case Qt::Key_Control: return ImGuiKey_RightCtrl;
			case Qt::Key_AltGr: return ImGuiKey_RightAlt;
			case Qt::Key_Super_R: return ImGuiKey_RightSuper;
			case Qt::Key_Menu: return ImGuiKey_Menu;
			case Qt::Key_0: return ImGuiKey_0;
			case Qt::Key_1: return ImGuiKey_1;
			case Qt::Key_2: return ImGuiKey_2;
			case Qt::Key_3: return ImGuiKey_3;
			case Qt::Key_4: return ImGuiKey_4;
			case Qt::Key_5: return ImGuiKey_5;
			case Qt::Key_6: return ImGuiKey_6;
			case Qt::Key_7: return ImGuiKey_7;
			case Qt::Key_8: return ImGuiKey_8;
			case Qt::Key_9: return ImGuiKey_9;
			case Qt::Key_A: return ImGuiKey_A;
			case Qt::Key_B: return ImGuiKey_B;
			case Qt::Key_C: return ImGuiKey_C;
			case Qt::Key_D: return ImGuiKey_D;
			case Qt::Key_E: return ImGuiKey_E;
			case Qt::Key_F: return ImGuiKey_F;
			case Qt::Key_G: return ImGuiKey_G;
			case Qt::Key_H: return ImGuiKey_H;
			case Qt::Key_I: return ImGuiKey_I;
			case Qt::Key_J: return ImGuiKey_J;
			case Qt::Key_K: return ImGuiKey_K;
			case Qt::Key_L: return ImGuiKey_L;
			case Qt::Key_M: return ImGuiKey_M;
			case Qt::Key_N: return ImGuiKey_N;
			case Qt::Key_O: return ImGuiKey_O;
			case Qt::Key_P: return ImGuiKey_P;
			case Qt::Key_Q: return ImGuiKey_Q;
			case Qt::Key_R: return ImGuiKey_R;
			case Qt::Key_S: return ImGuiKey_S;
			case Qt::Key_T: return ImGuiKey_T;
			case Qt::Key_U: return ImGuiKey_U;
			case Qt::Key_V: return ImGuiKey_V;
			case Qt::Key_W: return ImGuiKey_W;
			case Qt::Key_X: return ImGuiKey_X;
			case Qt::Key_Y: return ImGuiKey_Y;
			case Qt::Key_Z: return ImGuiKey_Z;
			case Qt::Key_F1: return ImGuiKey_F1;
			case Qt::Key_F2: return ImGuiKey_F2;
			case Qt::Key_F3: return ImGuiKey_F3;
			case Qt::Key_F4: return ImGuiKey_F4;
			case Qt::Key_F5: return ImGuiKey_F5;
			case Qt::Key_F6: return ImGuiKey_F6;
			case Qt::Key_F7: return ImGuiKey_F7;
			case Qt::Key_F8: return ImGuiKey_F8;
			case Qt::Key_F9: return ImGuiKey_F9;
			case Qt::Key_F10: return ImGuiKey_F10;
			case Qt::Key_F11: return ImGuiKey_F11;
			case Qt::Key_F12: return ImGuiKey_F12;
			case Qt::Key_F13: return ImGuiKey_F13;
			case Qt::Key_F14: return ImGuiKey_F14;
			case Qt::Key_F15: return ImGuiKey_F15;
			case Qt::Key_F16: return ImGuiKey_F16;
			case Qt::Key_F17: return ImGuiKey_F17;
			case Qt::Key_F18: return ImGuiKey_F18;
			case Qt::Key_F19: return ImGuiKey_F19;
			case Qt::Key_F20: return ImGuiKey_F20;
			case Qt::Key_F21: return ImGuiKey_F21;
			case Qt::Key_F22: return ImGuiKey_F22;
			case Qt::Key_F23: return ImGuiKey_F23;
			case Qt::Key_F24: return ImGuiKey_F24;
			case Qt::Key_Back: return ImGuiKey_AppBack;
			case Qt::Key_Forward: return ImGuiKey_AppForward;
			default: return ImGuiKey_None;
		}
	}

	void updateKeyModifiers(QKeyEvent *keyEvent)
	{
		ImGuiIO &io = ImGui::GetIO();

		io.AddKeyEvent(ImGuiMod_Ctrl, keyEvent->modifiers().testFlag(Qt::ControlModifier));
		io.AddKeyEvent(ImGuiMod_Shift, keyEvent->modifiers().testFlag(Qt::ShiftModifier));
		io.AddKeyEvent(ImGuiMod_Alt, keyEvent->modifiers().testFlag(Qt::AltModifier));
		io.AddKeyEvent(ImGuiMod_Super, keyEvent->modifiers().testFlag(Qt::MetaModifier));
	}

}

void ImGuiQt5Input::init(Qt5Widget *widget)
{
	ASSERT(widget);
	widget_ = widget;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup back-end capabilities flags
	ImGuiIO &io = ImGui::GetIO();
	io.BackendPlatformName = "nCine_Qt5";
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; // We can honor GetMouseCursor() values (optional)
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos; // We can honor io.WantSetMousePos requests (optional, rarely used)
	io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

	ImGuiPlatformIO& platformIo = ImGui::GetPlatformIO();
	platformIo.Platform_SetClipboardTextFn = setClipboardText;
	platformIo.Platform_GetClipboardTextFn = clipboardText;
	platformIo.Platform_ClipboardUserData = nullptr;

	mouseCursors_[ImGuiMouseCursor_Arrow] = QCursor(Qt::ArrowCursor);
	mouseCursors_[ImGuiMouseCursor_TextInput] = QCursor(Qt::IBeamCursor);
	mouseCursors_[ImGuiMouseCursor_ResizeAll] = QCursor(Qt::SizeAllCursor);
	mouseCursors_[ImGuiMouseCursor_ResizeNS] = QCursor(Qt::SizeVerCursor);
	mouseCursors_[ImGuiMouseCursor_ResizeEW] = QCursor(Qt::SizeHorCursor);
	mouseCursors_[ImGuiMouseCursor_ResizeNESW] = QCursor(Qt::SizeBDiagCursor);
	mouseCursors_[ImGuiMouseCursor_ResizeNWSE] = QCursor(Qt::SizeFDiagCursor);
	mouseCursors_[ImGuiMouseCursor_Hand] = QCursor(Qt::OpenHandCursor);
	mouseCursors_[ImGuiMouseCursor_NotAllowed] = QCursor(Qt::ForbiddenCursor);
	blankCursor_ = QCursor(Qt::BlankCursor);

#ifdef WITH_QT5GAMEPAD
	gamepad_ = nctl::makeUnique<QGamepad>(-1);
#endif
}

void ImGuiQt5Input::shutdown()
{
	widget_ = nullptr;

	ImGuiIO &io = ImGui::GetIO();
	io.BackendPlatformName = nullptr;
	io.BackendFlags &= ~(ImGuiBackendFlags_HasMouseCursors | ImGuiBackendFlags_HasSetMousePos | ImGuiBackendFlags_HasGamepad);

	ImGui::DestroyContext();
}

void ImGuiQt5Input::newFrame()
{
	ImGuiIO &io = ImGui::GetIO();
	IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! Missing call to ImGuiDrawing::buildFonts() function?");
	io.DeltaTime = theApplication().frameTime();

	io.DisplaySize = ImVec2(theApplication().width(), theApplication().height());

	updateMouseData();
	updateMouseCursor();

	// Update game controllers (if enabled and available)
	updateGamepads();
}

bool ImGuiQt5Input::event(QEvent *event)
{
	if (inputEnabled_ == false)
		return false;

	ImGuiIO &io = ImGui::GetIO();
	switch (event->type())
	{
		case QEvent::KeyPress:
		case QEvent::KeyRelease:
		{
			QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
			updateKeyModifiers(keyEvent);
			const ImGuiKey key = qtKeyToImGuiKey(keyEvent->key());
			io.AddKeyEvent(key, (event->type() == QEvent::KeyPress));
			// To support legacy indexing (<1.87 user code). Legacy backend uses `key - Qt::Key_Escape` as indices to IsKeyXXX() functions.
			const int keyIndex = (keyEvent->key() >= Qt::Key_Escape) ? keyEvent->key() - Qt::Key_Escape : keyEvent->key();
			if (keyIndex >= 0 && keyIndex < 512)
				io.SetKeyEventNativeData(key, keyEvent->key(), keyEvent->nativeScanCode(), keyIndex);

			if (event->type() == QEvent::KeyPress && keyEvent->text().length() > 0)
				io.AddInputCharactersUTF8(keyEvent->text().toUtf8().constData());

			return true;
		}
		case QEvent::MouseButtonPress:
		case QEvent::MouseButtonRelease:
		{
			QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
			int mouseButton = -1;
			if (mouseEvent->button() == Qt::LeftButton)
				mouseButton = 0;
			else if (mouseEvent->button() == Qt::RightButton)
				mouseButton = 1;
			else if (mouseEvent->button() == Qt::MiddleButton)
				mouseButton = 2;
			else if (mouseEvent->button() == Qt::BackButton)
				mouseButton = 3;
			else if (mouseEvent->button() == Qt::ForwardButton)
				mouseButton = 4;
			if (mouseButton == -1)
				break;
			io.AddMouseButtonEvent(mouseButton, (event->type() == QEvent::MouseButtonPress));
			return true;
		}
		case QEvent::MouseMove:
		{
			QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
			io.AddMousePosEvent(mouseEvent->x(), mouseEvent->y());
			return true;
		}
		case QEvent::Wheel:
		{
			QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
			const float wheelX = (wheelEvent->angleDelta().x() > 0) ? 1.0f : (wheelEvent->angleDelta().x() < 0) ? -1.0f : 0.0f;
			const float wheelY = (wheelEvent->angleDelta().y() > 0) ? 1.0f : (wheelEvent->angleDelta().y() < 0) ? -1.0f : 0.0f;
			io.AddMouseWheelEvent(wheelX, wheelY);
			return true;
		}
		case QEvent::FocusIn:
			io.AddFocusEvent(true);
			return true;
		case QEvent::FocusOut:
			io.AddFocusEvent(false);
			return true;
		default:
			return false;
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ImGuiQt5Input::updateMouseData()
{
	ImGuiIO &io = ImGui::GetIO();

	// Set OS mouse position if requested (rarely used, only when io.ConfigNavMoveSetMousePos is enabled by user)
	if (io.WantSetMousePos && widget_->hasFocus())
		widget_->cursor().setPos(static_cast<int>(io.MousePos.x), static_cast<int>(io.MousePos.y));
}

void ImGuiQt5Input::updateMouseCursor()
{
	ImGuiIO &io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
		return;

	ImGuiMouseCursor imguiCursor = ImGui::GetMouseCursor();
	if (io.MouseDrawCursor || imguiCursor == ImGuiMouseCursor_None)
		widget_->setCursor(blankCursor_);
	else
		widget_->setCursor(mouseCursors_[imguiCursor]);
}

void ImGuiQt5Input::updateGamepads()
{
#ifdef WITH_QT5GAMEPAD
	ImGuiIO &io = ImGui::GetIO();
	if ((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0) // FIXME: Technically feeding gamepad shouldn't depend on this now that they are regular inputs.
		return;

	// Get gamepad
	io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
	const QList<int> gamepads = QGamepadManager::instance()->connectedGamepads();
	if (gamepads.isEmpty())
		return;
	io.BackendFlags |= ImGuiBackendFlags_HasGamepad;

	const int deviceId = *gamepads.begin();
	if (gamepad_->deviceId() != deviceId)
		gamepad_->setDeviceId(deviceId);

	// Update gamepad inputs
	// clang-format off
	#define IM_SATURATE(V)                      (V < 0.0f ? 0.0f : V > 1.0f ? 1.0f : V)
	#define MAP_BUTTON(KEY_NO, BUTTON_NO)       { io.AddKeyEvent(KEY_NO, gamepad_->BUTTON_NO()); }
	#define MAP_ANALOG(KEY_NO, AXIS_NO, V0, V1) { float vn = static_cast<float>(gamepad_->AXIS_NO() - V0) / static_cast<float>(V1 - V0);\
	                                                    vn = IM_SATURATE(vn); io.AddKeyAnalogEvent(KEY_NO, vn > 0.1f, vn); }
	const int thumbDeadZone = 8000; // SDL_gamecontroller.h suggests using this value.
	MAP_BUTTON(ImGuiKey_GamepadStart,           buttonStart);
	MAP_BUTTON(ImGuiKey_GamepadBack,            buttonGuide);
	MAP_BUTTON(ImGuiKey_GamepadFaceLeft,        buttonX);              // Xbox X, PS Square
	MAP_BUTTON(ImGuiKey_GamepadFaceRight,       buttonB);              // Xbox B, PS Circle
	MAP_BUTTON(ImGuiKey_GamepadFaceUp,          buttonY);              // Xbox Y, PS Triangle
	MAP_BUTTON(ImGuiKey_GamepadFaceDown,        buttonA);              // Xbox A, PS Cross
	MAP_BUTTON(ImGuiKey_GamepadDpadLeft,        buttonLeft);
	MAP_BUTTON(ImGuiKey_GamepadDpadRight,       buttonRight);
	MAP_BUTTON(ImGuiKey_GamepadDpadUp,          buttonUp);
	MAP_BUTTON(ImGuiKey_GamepadDpadDown,        buttonDown);
	MAP_BUTTON(ImGuiKey_GamepadL1,              buttonL1);
	MAP_BUTTON(ImGuiKey_GamepadR1,              buttonR1);
	MAP_ANALOG(ImGuiKey_GamepadL2,              buttonL2,  0.0f, 32767);
	MAP_ANALOG(ImGuiKey_GamepadR2,              buttonR2, 0.0f, 32767);
	MAP_BUTTON(ImGuiKey_GamepadL3,              buttonL3);
	MAP_BUTTON(ImGuiKey_GamepadR3,              buttonR3);
	MAP_ANALOG(ImGuiKey_GamepadLStickLeft,      axisLeftX,  -thumbDeadZone, -32768);
	MAP_ANALOG(ImGuiKey_GamepadLStickRight,     axisLeftX,  +thumbDeadZone, +32767);
	MAP_ANALOG(ImGuiKey_GamepadLStickUp,        axisLeftY,  -thumbDeadZone, -32768);
	MAP_ANALOG(ImGuiKey_GamepadLStickDown,      axisLeftY,  +thumbDeadZone, +32767);
	MAP_ANALOG(ImGuiKey_GamepadRStickLeft,      axisRightX, -thumbDeadZone, -32768);
	MAP_ANALOG(ImGuiKey_GamepadRStickRight,     axisRightX, +thumbDeadZone, +32767);
	MAP_ANALOG(ImGuiKey_GamepadRStickUp,        axisRightY, -thumbDeadZone, -32768);
	MAP_ANALOG(ImGuiKey_GamepadRStickDown,      axisRightY, +thumbDeadZone, +32767);
	// clang-format on
	#undef MAP_BUTTON
	#undef MAP_ANALOG
#endif
}

}
