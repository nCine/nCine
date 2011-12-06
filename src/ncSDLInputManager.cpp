#include "ncSDLInputManager.h"
#include "ncIInputEventHandler.h"
#include "ncServiceLocator.h"
#include "ncFileLogger.h"
#include "ncApplication.h"
#include <SDL/SDL.h> // for SDL_WasInit()

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncSDLInputManager::ncSDLInputManager()
{
	if (SDL_WasInit(SDL_INIT_VIDEO) == 0)
	{
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (char *)"ncSDLInputManager::ncSDLInputManager - SDL video subsystem is not initialized");
		exit(-1);
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncSDLInputManager::ParseEvent(const SDL_Event &event)
{
	if (m_pInputEventHandler == NULL)
		return;

	switch (event.type) {
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			m_keyboardEvent.sym = ncKeySym(event.key.keysym.sym);
			m_keyboardEvent.mod = event.key.keysym.mod;
			m_keyboardEvent.unicode = ncKeyMod(event.key.keysym.unicode);
			break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			m_mouseEvent.x = event.button.x;
			m_mouseEvent.y = ncApplication::Height()-event.button.y;
			m_mouseEvent.button = event.button.button;
			break;
		case SDL_MOUSEMOTION:
			m_mouseState.x = event.motion.x;
			m_mouseState.y = ncApplication::Height()-event.motion.y;
			m_mouseState.buttons = event.motion.state;
			break;
	}

	switch (event.type) {
		case SDL_KEYDOWN:
			m_pInputEventHandler->OnKeyPressed(m_keyboardEvent);
			break;
		case SDL_KEYUP:
			m_pInputEventHandler->OnKeyReleased(m_keyboardEvent);
			break;
		case SDL_MOUSEBUTTONDOWN:
			m_pInputEventHandler->OnMouseButtonPressed(m_mouseEvent);
			break;
		case SDL_MOUSEBUTTONUP:
			m_pInputEventHandler->OnMouseButtonReleased(m_mouseEvent);
			break;
		case SDL_MOUSEMOTION:
			m_pInputEventHandler->OnMouseMoved(m_mouseState);
			break;
	}
}

