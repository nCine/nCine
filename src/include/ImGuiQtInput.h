#ifndef CLASS_NCINE_IMGUIQTINPUT
#define CLASS_NCINE_IMGUIQTINPUT

#include "imgui.h"
#include <QCursor>
#include <nctl/UniquePtr.h>

class QEvent;
class QGamepad;

namespace ncine {

class QtWidget;

/// The class that handles Qt input for ImGui
class ImGuiQtInput
{
  public:
	static void init(QtWidget *widget);
	static void shutdown();
	static void newFrame();
	static bool event(QEvent *event);

	static inline void setInputEnabled(bool inputEnabled) { inputEnabled_ = inputEnabled; }

  private:
	static bool inputEnabled_;
	static bool mousePressed_[5];
	static QCursor mouseCursors_[ImGuiMouseCursor_COUNT];
	static QCursor blankCursor_;

	static QtWidget *widget_;
	static nctl::UniquePtr<QGamepad> gamepad_;

	static void updateMouseData();
	static void updateMouseCursor();
	static void updateGamepads();
};

}

#endif
