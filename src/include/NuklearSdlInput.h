#ifndef CLASS_NCINE_NUKLEARSDLINPUT
#define CLASS_NCINE_NUKLEARSDLINPUT

#include "NuklearContext.h"
#include "nuklear.h"

struct SDL_Window;
union SDL_Event;

namespace ncine {

/// The class that handles SDL2 input for Nuklear
class NuklearSdlInput
{
  public:
	static void init(SDL_Window *window);
	static void shutdown();
	static void newFrame();

	static void inputBegin();
	static void inputEnd();
	static bool processEvent(const SDL_Event *event);

	static inline void setInputEnabled(bool inputEnabled) { inputEnabled_ = inputEnabled; }

  private:
	static bool inputEnabled_;
	static SDL_Window *window_;

	static void clipboardPaste(nk_handle usr, struct nk_text_edit *edit);
	static void clipboardCopy(nk_handle usr, const char *text, int len);
};

}

#endif
