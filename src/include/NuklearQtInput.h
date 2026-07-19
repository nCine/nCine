#ifndef CLASS_NCINE_NUKLEARQTINPUT
#define CLASS_NCINE_NUKLEARQTINPUT

#if defined(slots)
	#undef slots
#endif

#include "NuklearContext.h"
#include "nuklear.h"

class QEvent;

namespace ncine {

class QtWidget;

/// The class that handles Qt input for Nuklear
class NuklearQtInput
{
  public:
	static void init(QtWidget *widget);
	static void shutdown();
	static void newFrame();

	static bool event(QEvent *event);

	static inline void setInputEnabled(bool inputEnabled) { inputEnabled_ = inputEnabled; }

  private:
	static bool inputEnabled_;
	static const int NK_QT_TEXT_MAX = 256;
	static unsigned int text_[NK_QT_TEXT_MAX];
	static int textLength_;
	static struct nk_vec2 scroll_;

	static QtWidget *widget_;

	static void clipboardPaste(nk_handle usr, struct nk_text_edit *edit);
	static void clipboardCopy(nk_handle usr, const char *text, int len);
};

}

#endif
