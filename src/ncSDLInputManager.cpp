#include "ncSDLInputManager.h"
#include "ncIInputEventHandler.h"
#include "ncServiceLocator.h"
#include "ncFileLogger.h"
#include "ncApplication.h"
#include <SDL/SDL.h> // for SDL_WasInit()

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

ncIInputEventHandler* ncIInputManager::s_pInputEventHandler = NULL;
ncSDLMouseState ncSDLInputManager::s_mouseState;
ncSDLMouseEvent ncSDLInputManager::s_mouseEvent;
ncSDLKeyboardState ncSDLInputManager::s_keyboardState;
ncKeyboardEvent	ncSDLInputManager::s_keyboardEvent;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncSDLInputManager::ParseEvent(const SDL_Event &event)
{
	if (s_pInputEventHandler == NULL)
		return;

	switch (event.type) {
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			s_keyboardEvent.sym = ncKeySym(event.key.keysym.sym);
			s_keyboardEvent.mod = event.key.keysym.mod;
			s_keyboardEvent.unicode = ncKeyMod(event.key.keysym.unicode);
			break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			s_mouseEvent.x = event.button.x;
			s_mouseEvent.y = ncApplication::Height()-event.button.y;
			s_mouseEvent.button = event.button.button;
			break;
		case SDL_MOUSEMOTION:
			s_mouseState.x = event.motion.x;
			s_mouseState.y = ncApplication::Height()-event.motion.y;
			s_mouseState.buttons = event.motion.state;
			break;
	}

	switch (event.type) {
		case SDL_KEYDOWN:
			s_pInputEventHandler->OnKeyPressed(s_keyboardEvent);
			break;
		case SDL_KEYUP:
			s_pInputEventHandler->OnKeyReleased(s_keyboardEvent);
			break;
		case SDL_MOUSEBUTTONDOWN:
			s_pInputEventHandler->OnMouseButtonPressed(s_mouseEvent);
			break;
		case SDL_MOUSEBUTTONUP:
			s_pInputEventHandler->OnMouseButtonReleased(s_mouseEvent);
			break;
		case SDL_MOUSEMOTION:
			s_pInputEventHandler->OnMouseMoved(s_mouseState);
			break;
	}
}

