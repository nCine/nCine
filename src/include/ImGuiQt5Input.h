#ifndef CLASS_NCINE_IMGUIQT5INPUT
#define CLASS_NCINE_IMGUIQT5INPUT

#include "imgui.h"
#include <QCursor>

class QEvent;
class QGamepad;

namespace ncine {

class Qt5Widget;

/// The class that handles Qt5 input for ImGui
class ImGuiQt5Input
{
  public:
	static void init(Qt5Widget *widget);
	static void shutdown();
	static void newFrame();
	static bool event(QEvent *event);

	static inline void setInputEnabled(bool inputEnabled) { inputEnabled_ = inputEnabled; }

  private:
	static bool inputEnabled_;
	static bool mousePressed_[5];
	static QCursor mouseCursors_[ImGuiMouseCursor_COUNT];
	static QCursor blankCursor_;

	static Qt5Widget *widget_;
	static nctl::UniquePtr<QGamepad> gamepad_;

	static void updateMousePosAndButtons();
	static void updateMouseCursor();
	static void updateGamepads();
};

}

#endif
