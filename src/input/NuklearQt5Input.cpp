#include "NuklearQt5Input.h"
#include "Qt5Widget.h"
#include "IInputManager.h"
#include "Application.h"

#include <qevent.h>
#include <QApplication>
#include <QClipboard>

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

bool NuklearQt5Input::inputEnabled_ = true;
Qt5Widget *NuklearQt5Input::widget_ = nullptr;

unsigned int NuklearQt5Input::text_[NK_QT5_TEXT_MAX];
int NuklearQt5Input::textLength_;
struct nk_vec2 NuklearQt5Input::scroll_;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void NuklearQt5Input::init(Qt5Widget *widget)
{
	ASSERT(widget);
	widget_ = widget;

	NuklearContext::init();
	NuklearContext::ctx_.clip.copy = clipboardCopy;
	NuklearContext::ctx_.clip.paste = clipboardPaste;
	NuklearContext::ctx_.clip.userdata = nk_handle_ptr(nullptr);
}

void NuklearQt5Input::shutdown()
{
	NuklearContext::shutdown();
	widget_ = nullptr;
}

void NuklearQt5Input::newFrame()
{
	NuklearContext::width_ = theApplication().width();
	NuklearContext::height_ = theApplication().height();
	NuklearContext::displayWidth_ = NuklearContext::width_;
	NuklearContext::displayHeight_ = NuklearContext::height_;

	NuklearContext::fbScale_.x = static_cast<float>(NuklearContext::displayWidth_) / static_cast<float>(NuklearContext::width_);
	NuklearContext::fbScale_.y = static_cast<float>(NuklearContext::displayHeight_) / static_cast<float>(NuklearContext::height_);

	nk_context *ctx = NuklearContext::context();

	nk_input_begin(ctx);
	for (int i = 0; i < textLength_; ++i)
		nk_input_unicode(ctx, text_[i]);

	/* optional grabbing behavior */
	if (ctx->input.mouse.grab)
		widget_->grabMouse(QCursor(Qt::BlankCursor));
	else if (ctx->input.mouse.ungrab)
	{
		widget_->unsetCursor();
		widget_->releaseMouse();
	}

	const KeyboardState &keyState = theApplication().inputManager().keyboardState();
	nk_input_key(ctx, NK_KEY_DEL, keyState.isKeyDown(KeySym::DELETE));
	nk_input_key(ctx, NK_KEY_ENTER, keyState.isKeyDown(KeySym::RETURN));
	nk_input_key(ctx, NK_KEY_TAB, keyState.isKeyDown(KeySym::TAB));
	nk_input_key(ctx, NK_KEY_BACKSPACE, keyState.isKeyDown(KeySym::BACKSPACE));
	nk_input_key(ctx, NK_KEY_UP, keyState.isKeyDown(KeySym::UP));
	nk_input_key(ctx, NK_KEY_DOWN, keyState.isKeyDown(KeySym::DOWN));
	nk_input_key(ctx, NK_KEY_TEXT_START, keyState.isKeyDown(KeySym::HOME));
	nk_input_key(ctx, NK_KEY_TEXT_END, keyState.isKeyDown(KeySym::END));
	nk_input_key(ctx, NK_KEY_SCROLL_START, keyState.isKeyDown(KeySym::HOME));
	nk_input_key(ctx, NK_KEY_SCROLL_END, keyState.isKeyDown(KeySym::END));
	nk_input_key(ctx, NK_KEY_SCROLL_DOWN, keyState.isKeyDown(KeySym::PAGEDOWN));
	nk_input_key(ctx, NK_KEY_SCROLL_UP, keyState.isKeyDown(KeySym::PAGEUP));
	nk_input_key(ctx, NK_KEY_SHIFT, keyState.isKeyDown(KeySym::LSHIFT) || keyState.isKeyDown(KeySym::RSHIFT));

	if (keyState.isKeyDown(KeySym::LCTRL) || keyState.isKeyDown(KeySym::RCTRL))
	{
		nk_input_key(ctx, NK_KEY_COPY, keyState.isKeyDown(KeySym::C));
		nk_input_key(ctx, NK_KEY_PASTE, keyState.isKeyDown(KeySym::V));
		nk_input_key(ctx, NK_KEY_CUT, keyState.isKeyDown(KeySym::X));
		nk_input_key(ctx, NK_KEY_TEXT_UNDO, keyState.isKeyDown(KeySym::Z));
		nk_input_key(ctx, NK_KEY_TEXT_REDO, keyState.isKeyDown(KeySym::R));
		nk_input_key(ctx, NK_KEY_TEXT_WORD_LEFT, keyState.isKeyDown(KeySym::LEFT));
		nk_input_key(ctx, NK_KEY_TEXT_WORD_RIGHT, keyState.isKeyDown(KeySym::RIGHT));
		nk_input_key(ctx, NK_KEY_TEXT_LINE_START, keyState.isKeyDown(KeySym::B));
		nk_input_key(ctx, NK_KEY_TEXT_LINE_END, keyState.isKeyDown(KeySym::E));
	}
	else
	{
		nk_input_key(ctx, NK_KEY_LEFT, keyState.isKeyDown(KeySym::LEFT));
		nk_input_key(ctx, NK_KEY_RIGHT, keyState.isKeyDown(KeySym::RIGHT));
		nk_input_key(ctx, NK_KEY_COPY, 0);
		nk_input_key(ctx, NK_KEY_PASTE, 0);
		nk_input_key(ctx, NK_KEY_CUT, 0);
		nk_input_key(ctx, NK_KEY_SHIFT, 0);
	}

	const MouseState &mouseState = theApplication().inputManager().mouseState();
	const int x = mouseState.x;
	const int y = theApplication().height() - mouseState.y;
	nk_input_motion(ctx, x, y);

	if (ctx->input.mouse.grabbed)
	{
		widget_->cursor().setPos(static_cast<int>(ctx->input.mouse.prev.x), static_cast<int>(ctx->input.mouse.prev.y));
		ctx->input.mouse.pos.x = ctx->input.mouse.prev.x;
		ctx->input.mouse.pos.y = ctx->input.mouse.prev.y;
	}

	nk_input_button(ctx, NK_BUTTON_LEFT, x, y, mouseState.isLeftButtonDown());
	nk_input_button(ctx, NK_BUTTON_MIDDLE, x, y, mouseState.isLeftButtonDown());
	nk_input_button(ctx, NK_BUTTON_RIGHT, x, y, mouseState.isRightButtonDown());
	nk_input_scroll(ctx, scroll_);
	nk_input_end(ctx);
	textLength_ = 0;
	scroll_ = nk_vec2(0.0f, 0.0f);
}

bool NuklearQt5Input::event(QEvent *event)
{
	if (inputEnabled_ == false)
		return false;

	nk_context *ctx = NuklearContext::context();
	switch (event->type())
	{
		case QEvent::KeyPress:
		{
			QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

			/* text input */
			if (keyEvent->text().length() > 0 && textLength_ < NK_QT5_TEXT_MAX)
				text_[textLength_++] = keyEvent->text().data()->unicode();
			return true;
		}
		case QEvent::MouseButtonDblClick:
		{
			QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
			const int x = mouseEvent->x(), y = mouseEvent->y();
			if (mouseEvent->button() == Qt::LeftButton)
				nk_input_button(ctx, NK_BUTTON_DOUBLE, x, y, true);
			return true;
		}
		case QEvent::Wheel:
		{
			/* mouse wheel */
			QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
			scroll_.x += wheelEvent->angleDelta().x() / 60.0f;
			scroll_.y += wheelEvent->angleDelta().y() / 60.0f;
			return true;
		}
		default:
			return false;
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void NuklearQt5Input::clipboardPaste(nk_handle usr, struct nk_text_edit *edit)
{
	QClipboard *clipboard = QApplication::clipboard();
	const char *text = clipboard->text().toStdString().c_str();
	if (text)
		nk_textedit_paste(edit, text, nk_strlen(text));
}

void NuklearQt5Input::clipboardCopy(nk_handle usr, const char *text, int len)
{
	if (len == 0)
		return;

	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(text);
}

}
