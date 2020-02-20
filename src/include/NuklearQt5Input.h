#ifndef CLASS_NCINE_NUKLEARQT5INPUT
#define CLASS_NCINE_NUKLEARQT5INPUT

#include "NuklearContext.h"
#undef slots
#include "nuklear.h"

class QEvent;

namespace ncine {

class Qt5Widget;

/// The class that handles Qt5 input for Nuklear
class NuklearQt5Input
{
  public:
	static void init(Qt5Widget *widget);
	static void shutdown();
	static void newFrame();

	static bool event(QEvent *event);

	static inline void setInputEnabled(bool inputEnabled) { inputEnabled_ = inputEnabled; }

  private:
	static bool inputEnabled_;
	static const int NK_QT5_TEXT_MAX = 256;
	static unsigned int text_[NK_QT5_TEXT_MAX];
	static int textLength_;
	static struct nk_vec2 scroll_;

	static Qt5Widget *widget_;

	static void clipboardPaste(nk_handle usr, struct nk_text_edit *edit);
	static void clipboardCopy(nk_handle usr, const char *text, int len);
};

}

#endif
