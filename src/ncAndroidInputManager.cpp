#include "ncAndroidInputManager.h"
#include "ncIInputEventHandler.h"
#include "ncApplication.h"

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncAndroidInputManager::ParseEvent(const AInputEvent* event)
{
	if (m_pInputEventHandler == NULL)
		return;

	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY)
	{
		m_keyboardEvent.sym = ncKeySym(AKeyEvent_getKeyCode(event));
		m_keyboardEvent.mod = AKeyEvent_getMetaState(event);

		switch(AKeyEvent_getAction(event))
		{
			case AKEY_EVENT_ACTION_DOWN:
				m_pInputEventHandler->OnKeyPressed(m_keyboardEvent);
				break;
			case AKEY_EVENT_ACTION_UP:
				m_pInputEventHandler->OnKeyReleased(m_keyboardEvent);
				break;
			case AKEY_EVENT_ACTION_MULTIPLE:
				break;
		}
	}
	else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
	{
		m_touchEvent.count = AMotionEvent_getPointerCount(event);
		m_touchEvent.id = AMotionEvent_getPointerId(event, 0);
		m_touchEvent.x = AMotionEvent_getX(event, 0);
		m_touchEvent.y = ncApplication::Height() - AMotionEvent_getY(event, 0);
		m_touchEvent.id2 = AMotionEvent_getPointerId(event, 1);
		m_touchEvent.x2 = AMotionEvent_getX(event, 1);
		m_touchEvent.y2 = ncApplication::Height() - AMotionEvent_getY(event, 1);

		switch(AMotionEvent_getAction(event))
		{
			case AMOTION_EVENT_ACTION_DOWN:
				m_pInputEventHandler->OnTouchDown(m_touchEvent);
				break;
			case AMOTION_EVENT_ACTION_UP:
				m_pInputEventHandler->OnTouchUp(m_touchEvent);
				break;
			case AMOTION_EVENT_ACTION_MOVE:
				m_pInputEventHandler->OnTouchMove(m_touchEvent);
				break;
			case AMOTION_EVENT_ACTION_POINTER_DOWN:
				m_pInputEventHandler->OnSecondaryTouchDown(m_touchEvent);
				break;
			case AMOTION_EVENT_ACTION_POINTER_UP:
				m_pInputEventHandler->OnSecondaryTouchUp(m_touchEvent);
				break;
		}
	}
}

