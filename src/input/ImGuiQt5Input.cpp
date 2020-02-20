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

	const char *clipboardText(void *userData)
	{
		QClipboard *clipboard = QApplication::clipboard();
		const int charsToCopy = clipboard->text().length() < 1024 ? clipboard->text().length() : 1024;
		for (int i = 0; i < charsToCopy; i++)
			clipboardString[i] = static_cast<char>(clipboard->text().at(i).cell());

		return clipboardString;
	}

	void setClipboardText(void *userData, const char *text)
	{
		QClipboard *clipboard = QApplication::clipboard();
		clipboard->setText(text);
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
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; // We can honor GetMouseCursor() values (optional)
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos; // We can honor io.WantSetMousePos requests (optional, rarely used)
	io.BackendPlatformName = "nCine_Qt5";
	io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

	// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
	io.KeyMap[ImGuiKey_Tab] = Qt::Key_Tab - Qt::Key_Escape;
	io.KeyMap[ImGuiKey_LeftArrow] = Qt::Key_Left - Qt::Key_Escape;
	io.KeyMap[ImGuiKey_RightArrow] = Qt::Key_Right - Qt::Key_Escape;
	io.KeyMap[ImGuiKey_UpArrow] = Qt::Key_Up - Qt::Key_Escape;
	io.KeyMap[ImGuiKey_DownArrow] = Qt::Key_Down - Qt::Key_Escape;
	io.KeyMap[ImGuiKey_PageUp] = Qt::Key_PageUp - Qt::Key_Escape;
	io.KeyMap[ImGuiKey_PageDown] = Qt::Key_PageDown - Qt::Key_Escape;
	io.KeyMap[ImGuiKey_Home] = Qt::Key_Home - Qt::Key_Escape;
	io.KeyMap[ImGuiKey_End] = Qt::Key_End - Qt::Key_Escape;
	io.KeyMap[ImGuiKey_Insert] = Qt::Key_Insert - Qt::Key_Escape;
	io.KeyMap[ImGuiKey_Delete] = Qt::Key_Delete - Qt::Key_Escape;
	io.KeyMap[ImGuiKey_Backspace] = Qt::Key_Backspace - Qt::Key_Escape;
	io.KeyMap[ImGuiKey_Space] = Qt::Key_Space;
	io.KeyMap[ImGuiKey_Enter] = Qt::Key_Return - Qt::Key_Escape;
	io.KeyMap[ImGuiKey_Escape] = Qt::Key_Escape - Qt::Key_Escape;
	io.KeyMap[ImGuiKey_KeyPadEnter] = Qt::Key_Enter - Qt::Key_Escape;
	io.KeyMap[ImGuiKey_A] = Qt::Key_A;
	io.KeyMap[ImGuiKey_C] = Qt::Key_C;
	io.KeyMap[ImGuiKey_V] = Qt::Key_V;
	io.KeyMap[ImGuiKey_X] = Qt::Key_X;
	io.KeyMap[ImGuiKey_Y] = Qt::Key_Y;
	io.KeyMap[ImGuiKey_Z] = Qt::Key_Z;

	io.SetClipboardTextFn = setClipboardText;
	io.GetClipboardTextFn = clipboardText;
	io.ClipboardUserData = nullptr;

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
	ImGui::DestroyContext();
	widget_ = nullptr;
}

void ImGuiQt5Input::newFrame()
{
	ImGuiIO &io = ImGui::GetIO();
	IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! It is generally built by the renderer back-end. Missing call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame().");
	io.DeltaTime = theApplication().interval();

	io.DisplaySize = ImVec2(theApplication().width(), theApplication().height());

	updateMousePosAndButtons();
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
			if (event->type() == QEvent::KeyPress && keyEvent->text().length() > 0)
				io.AddInputCharactersUTF8(keyEvent->text().toStdString().c_str());

			int key = keyEvent->key();
			if (key >= Qt::Key_Escape)
				key -= Qt::Key_Escape;
			IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
			io.KeysDown[key] = (event->type() == QEvent::KeyPress);
			io.KeyShift = keyEvent->modifiers().testFlag(Qt::ShiftModifier);
			io.KeyCtrl = keyEvent->modifiers().testFlag(Qt::ControlModifier);
			io.KeyAlt = keyEvent->modifiers().testFlag(Qt::AltModifier);
#ifdef _WIN32
			io.KeySuper = false;
#else
			io.KeySuper = keyEvent->modifiers().testFlag(Qt::MetaModifier);
#endif
			return true;
		}
		case QEvent::MouseButtonPress:
		{
			QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
			io.MousePos = ImVec2(mouseEvent->x(), mouseEvent->y());
			if (mouseEvent->button() == Qt::LeftButton)
				mousePressed_[0] = true;
			else if (mouseEvent->button() == Qt::RightButton)
				mousePressed_[1] = true;
			else if (mouseEvent->button() == Qt::MiddleButton)
				mousePressed_[2] = true;
			else if (mouseEvent->button() == Qt::BackButton)
				mousePressed_[3] = true;
			else if (mouseEvent->button() == Qt::ForwardButton)
				mousePressed_[4] = true;
			return true;
		}
		case QEvent::MouseMove:
		{
			QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
			io.MousePos = ImVec2(mouseEvent->x(), mouseEvent->y());
			if (mouseEvent->buttons() & Qt::LeftButton)
				mousePressed_[0] = true;
			if (mouseEvent->buttons() & Qt::RightButton)
				mousePressed_[1] = true;
			if (mouseEvent->buttons() & Qt::MiddleButton)
				mousePressed_[2] = true;
			if (mouseEvent->buttons() & Qt::BackButton)
				mousePressed_[3] = true;
			if (mouseEvent->buttons() & Qt::ForwardButton)
				mousePressed_[4] = true;
			return true;
		}
		case QEvent::Wheel:
		{
			QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
			if (wheelEvent->angleDelta().x() > 0)
				io.MouseWheelH += 1;
			if (wheelEvent->angleDelta().x() < 0)
				io.MouseWheelH -= 1;
			if (wheelEvent->angleDelta().y() > 0)
				io.MouseWheel += 1;
			if (wheelEvent->angleDelta().y() < 0)
				io.MouseWheel -= 1;
			return true;
		}
		default:
			return false;
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ImGuiQt5Input::updateMousePosAndButtons()
{
	ImGuiIO &io = ImGui::GetIO();

	// Set OS mouse position if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
	if (io.WantSetMousePos)
		widget_->cursor().setPos(static_cast<int>(io.MousePos.x), static_cast<int>(io.MousePos.y));

	const MouseState &mouseState = theApplication().inputManager().mouseState();
	io.MouseDown[0] = mousePressed_[0] || mouseState.isLeftButtonDown();
	io.MouseDown[1] = mousePressed_[1] || mouseState.isRightButtonDown();
	io.MouseDown[2] = mousePressed_[2] || mouseState.isMiddleButtonDown();
	io.MouseDown[3] = mousePressed_[3] || mouseState.isFourthButtonDown();
	io.MouseDown[4] = mousePressed_[4] || mouseState.isFifthButtonDown();
	mousePressed_[0] = mousePressed_[1] = mousePressed_[2] = mousePressed_[3] = mousePressed_[4] = false;
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
	memset(io.NavInputs, 0, sizeof(io.NavInputs));
	if ((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0)
		return;

	const QList<int> gamepads = QGamepadManager::instance()->connectedGamepads();
	if (gamepads.isEmpty())
		return;

	const int deviceId = *gamepads.begin();
	if (gamepad_->deviceId() != deviceId)
		gamepad_->setDeviceId(deviceId);

	io.NavInputs[ImGuiNavInput_Activate] = static_cast<float>(gamepad_->buttonA());
	io.NavInputs[ImGuiNavInput_Cancel] = static_cast<float>(gamepad_->buttonB());
	io.NavInputs[ImGuiNavInput_Menu] = static_cast<float>(gamepad_->buttonX());
	io.NavInputs[ImGuiNavInput_Input] = static_cast<float>(gamepad_->buttonY());
	io.NavInputs[ImGuiNavInput_DpadLeft] = static_cast<float>(gamepad_->buttonLeft());
	io.NavInputs[ImGuiNavInput_DpadRight] = static_cast<float>(gamepad_->buttonRight());
	io.NavInputs[ImGuiNavInput_DpadUp] = static_cast<float>(gamepad_->buttonUp());
	io.NavInputs[ImGuiNavInput_DpadDown] = static_cast<float>(gamepad_->buttonDown());
	io.NavInputs[ImGuiNavInput_FocusPrev] = static_cast<float>(gamepad_->buttonL1());
	io.NavInputs[ImGuiNavInput_FocusNext] = static_cast<float>(gamepad_->buttonR1());
	io.NavInputs[ImGuiNavInput_TweakSlow] = static_cast<float>(gamepad_->buttonL1());
	io.NavInputs[ImGuiNavInput_TweakFast] = static_cast<float>(gamepad_->buttonR1());
	io.NavInputs[ImGuiNavInput_LStickLeft] = -gamepad_->axisLeftX();
	io.NavInputs[ImGuiNavInput_LStickRight] = gamepad_->axisLeftX();
	io.NavInputs[ImGuiNavInput_LStickUp] = -gamepad_->axisLeftY();
	io.NavInputs[ImGuiNavInput_LStickDown] = gamepad_->axisLeftY();

	io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
#endif
}

}
