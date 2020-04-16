#include "apptest_scene.h"
#include <ncine/Application.h>
#include <ncine/Texture.h>
#include <ncine/Sprite.h>
#include "apptest_datapath.h"

#include <ncine/config.h>

#if NCINE_WITH_IMGUI
	#include <ncine/imgui.h>
#endif

#if NCINE_WITH_NUKLEAR
	#define NK_IMPLEMENTATION
	#include <ncine/NuklearContext.h>
	#include <ncine/nuklear.h>
#endif

#if NCINE_WITH_QT5
	#include <QApplication>
	#include <QPushButton>
	#include <QCheckBox>
	#include <QRadioButton>
	#include <QLabel>
	#include <QSlider>
	#include <QColorDialog>
	#include <QLineEdit>
	#include <QVBoxLayout>
	#include <ncine/Qt5Widget.h>
#endif

namespace {

#ifdef __ANDROID__
const char *Texture1File = "texture1_ETC2.ktx";
const char *Texture2File = "texture2_ETC2.ktx";
const char *Texture3File = "texture3_ETC2.ktx";
const char *Texture4File = "texture4_ETC2.ktx";
#else
const char *Texture1File = "texture1.png";
const char *Texture2File = "texture2.png";
const char *Texture3File = "texture3.png";
const char *Texture4File = "texture4.png";
#endif

#if NCINE_WITH_IMGUI || NCINE_WITH_NUKLEAR || NCINE_WITH_QT5
	#define HAS_GUI (1)
#endif

bool paused = false;
#ifdef HAS_GUI
enum
{
	SLOW,
	FAST
};

int speed = SLOW;
const float MinSpriteScale = 0.25f;
const float MaxSpriteScale = 2.0f;
float spriteScale = 0.5f;
nc::Colorf bgColor(0.10f, 0.18f, 0.24f, 1.0f);
const int MaxBufferLength = 256;
char textBuffer[MaxBufferLength] = { 0 };
#endif

#if NCINE_WITH_QT5
QCheckBox *pauseCheck = nullptr;
QRadioButton *slowRadioButton = nullptr;
QRadioButton *fastRadioButton = nullptr;
QLabel *angleLabel = nullptr;
QSlider *scaleSlider = nullptr;
QColor buttonBackground(1.0f, 1.0f, 1.0f);
QPushButton *colorButton = nullptr;
QLineEdit *lineEdit = nullptr;

void quit()
{
	nc::theApplication().quit();
}

void setPaused(int state)
{
	paused = state;
}

void setSlowSpeed()
{
	speed = SLOW;
}

void setFastSpeed()
{
	speed = FAST;
}

void setSpriteScale(int value)
{
	const float step = (MaxSpriteScale - MinSpriteScale) / 100.0f;
	spriteScale = MinSpriteScale + value * step;
}

void setBackgroundColor()
{
	QColor oldColor = QColor::fromRgbF(bgColor.r(), bgColor.g(), bgColor.b());
	QColor newColor = QColorDialog::getColor(oldColor, nullptr, "Pick a new background color");
	bgColor.set(newColor.redF(), newColor.greenF(), newColor.blueF(), 1.0f);
}

void setText(const QString &text)
{
	const int charsToCopy = text.length() < MaxBufferLength - 1 ? text.length() : MaxBufferLength - 1;
	for (int i = 0; i < charsToCopy; i++)
		textBuffer[i] = static_cast<char>(text.at(i).cell());
	textBuffer[charsToCopy] = '\0';
}
#endif

}

nctl::UniquePtr<nc::IAppEventHandler> createAppEventHandler()
{
	return nctl::makeUnique<MyEventHandler>();
}

void MyEventHandler::onPreInit(nc::AppConfiguration &config)
{
	setDataPath(config);
}

void MyEventHandler::onInit()
{
	angle_ = 0.0f;

	nc::SceneNode &rootNode = nc::theApplication().rootNode();

	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture1File)).data()));
	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture2File)).data()));
	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture3File)).data()));
	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture4File)).data()));

	const float width = nc::theApplication().width();
	for (unsigned int i = 0; i < NumSprites; i++)
	{
		sprites_.pushBack(nctl::makeUnique<nc::Sprite>(&rootNode, textures_[i % NumTextures].get(), width * 0.15f + width * 0.1f * i, 0.0f));
		sprites_.back()->setScale(0.5f);
	}
}

void MyEventHandler::onFrameStart()
{
	const float height = nc::theApplication().height();

	if (paused == false)
	{
		angle_ += 1.0f * nc::theApplication().interval();
		if (angle_ > 360.0f)
			angle_ -= 360.0f;
	}

	for (unsigned int i = 0; i < NumSprites; i++)
	{
		sprites_[i]->y = height * 0.3f + fabsf(sinf(angle_ + 5.0f * i)) * (height * (0.25f + 0.02f * i));
		sprites_[i]->setRotation(angle_ * 20.0f);
	}

#if NCINE_WITH_IMGUI
	static char imguiTextInput[MaxBufferLength];
	static nctl::String auxString;

	ImGui::SetWindowPos(ImVec2(650, 50), ImGuiCond_FirstUseEver);
	ImGui::SetWindowSize(ImVec2(325, 200), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("ImGui Interface"))
	{
		if (ImGui::Button("Quit"))
			nc::theApplication().quit();
		ImGui::SameLine();
		ImGui::Checkbox("Paused", &paused);

		ImGui::RadioButton("Slow", &speed, SLOW);
		ImGui::SameLine();
		ImGui::RadioButton("Fast", &speed, FAST);
		auxString.format("Angle: %.0f", angle_);
		ImGui::ProgressBar(angle_ / 360.0f, ImVec2(0.0f, 0.0f), auxString.data());

		ImGui::SliderFloat("Sprite Scale", &spriteScale, MinSpriteScale, MaxSpriteScale);
		ImGui::ColorEdit3("Background", bgColor.data());
		ImGui::InputText("Text Input", textBuffer, MaxBufferLength);
		ImGui::InputText("Unlinked Input", imguiTextInput, MaxBufferLength);

		ImGui::End();
	}
#endif

#if NCINE_WITH_NUKLEAR
	static char nuklearTextInput[MaxBufferLength];
	nk_context *ctx = nc::NuklearContext::context();

	if (nk_begin(ctx, "Nuklear Interface", nk_rect(150, 50, 230, 275),
	             NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
	{
		nk_layout_row_static(ctx, 30, 80, 2);
		if (nk_button_label(ctx, "Quit"))
			nc::theApplication().quit();

		int intPaused = paused;
		nk_checkbox_label(ctx, "Paused", &intPaused);
		paused = intPaused;

		nk_layout_row_dynamic(ctx, 30, 2);
		if (nk_option_label(ctx, "Slow", speed == SLOW))
			speed = SLOW;
		if (nk_option_label(ctx, "Fast", speed == FAST))
			speed = FAST;
		nk_layout_row_dynamic(ctx, 15, 1);
		nk_size progressValue = static_cast<nk_size>(angle_);
		nk_progress(ctx, &progressValue, 360, NK_FIXED);

		nk_layout_row_dynamic(ctx, 25, 1);
		nk_property_float(ctx, "Sprite Scale:", MinSpriteScale, &spriteScale, MaxSpriteScale, 0.1f, 0.01f);

		nk_layout_row_dynamic(ctx, 20, 1);
		nk_label(ctx, "Background:", NK_TEXT_LEFT);
		nk_layout_row_dynamic(ctx, 25, 1);
		nk_colorf bg = { bgColor.r(), bgColor.g(), bgColor.b(), bgColor.a() };
		if (nk_combo_begin_color(ctx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(ctx), 400)))
		{
			nk_layout_row_dynamic(ctx, 120, 1);
			bg = nk_color_picker(ctx, bg, NK_RGB);
			nk_layout_row_dynamic(ctx, 25, 1);
			bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f, 0.005f);
			bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f, 0.005f);
			bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f, 0.005f);
			nk_combo_end(ctx);
		}
		bgColor.set(bg.r, bg.g, bg.b, bg.a);

		nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, textBuffer, sizeof(textBuffer) - 1, nk_filter_default);
		nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, nuklearTextInput, sizeof(nuklearTextInput) - 1, nk_filter_default);

		nk_end(ctx);
	}
#endif

#if NCINE_WITH_QT5
	static nctl::String labelString;

	if (pauseCheck)
		pauseCheck->setChecked(paused);

	if (slowRadioButton && fastRadioButton)
	{
		slowRadioButton->setChecked(speed == SLOW);
		fastRadioButton->setChecked(speed == FAST);
	}

	labelString.format("Angle: %.0f", angle_);
	angleLabel->setText(labelString.data());

	if (scaleSlider)
	{
		const int sliderValue = static_cast<int>(100.0f * (spriteScale - MinSpriteScale) / (MaxSpriteScale - MinSpriteScale));
		scaleSlider->setValue(sliderValue);
	}

	if (colorButton)
	{
		QColor color = QColor::fromRgbF(bgColor.r(), bgColor.g(), bgColor.b());
		if (color != buttonBackground)
		{
			colorButton->setStyleSheet("background-color: " + color.name());
			buttonBackground = color;
		}
	}

	if (lineEdit && lineEdit->isModified() == false)
		lineEdit->setText(textBuffer);
#endif

#ifdef HAS_GUI
	if (paused == false && speed == FAST)
		angle_ += 1.0f * nc::theApplication().interval();

	for (unsigned int i = 0; i < NumSprites; i++)
		sprites_[i]->setScale(spriteScale);

	nc::theApplication().gfxDevice().setClearColor(bgColor);
#endif
}

#ifdef __ANDROID__
void MyEventHandler::onTouchDown(const nc::TouchEvent &event)
{
	#ifndef HAS_GUI
	paused = true;
	#endif
}

void MyEventHandler::onTouchUp(const nc::TouchEvent &event)
{
	#ifndef HAS_GUI
	paused = false;
	#endif
}
#endif

void MyEventHandler::onMouseButtonPressed(const nc::MouseEvent &event)
{
#ifndef HAS_GUI
	if (event.isLeftButton())
		paused = true;
#endif
}

void MyEventHandler::onMouseButtonReleased(const nc::MouseEvent &event)
{
#ifndef HAS_GUI
	if (event.isLeftButton())
		paused = false;
#endif
}

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KeySym::ESCAPE || event.sym == nc::KeySym::Q)
		nc::theApplication().quit();
	else if (event.sym == nc::KeySym::SPACE)
	{
		const bool isSuspended = nc::theApplication().isSuspended();
		nc::theApplication().setSuspended(!isSuspended);
	}
}

#if NCINE_WITH_QT5

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	QPushButton *quitButton = new QPushButton;
	quitButton->setText("Quit");
	QObject::connect(quitButton, &QPushButton::clicked, quit);

	pauseCheck = new QCheckBox;
	pauseCheck->setText("Paused");
	QObject::connect(pauseCheck, &QCheckBox::stateChanged, setPaused);

	QHBoxLayout *firstLineLayout = new QHBoxLayout(nullptr);
	firstLineLayout->addWidget(quitButton);
	firstLineLayout->addWidget(pauseCheck);

	slowRadioButton = new QRadioButton;
	slowRadioButton->setText("Slow");
	QObject::connect(slowRadioButton, &QRadioButton::clicked, setSlowSpeed);
	fastRadioButton = new QRadioButton;
	fastRadioButton->setText("Fast");
	QObject::connect(fastRadioButton, &QRadioButton::clicked, setFastSpeed);

	QHBoxLayout *secondLineLayout = new QHBoxLayout(nullptr);
	secondLineLayout->addWidget(slowRadioButton);
	secondLineLayout->addWidget(fastRadioButton);

	angleLabel = new QLabel;

	scaleSlider = new QSlider;
	scaleSlider->setOrientation(Qt::Horizontal);
	scaleSlider->setRange(0, 100);
	scaleSlider->setToolTip("Sprite Scale");
	QObject::connect(scaleSlider, &QSlider::sliderMoved, setSpriteScale);

	colorButton = new QPushButton;
	colorButton->setText("Background");
	QObject::connect(colorButton, &QRadioButton::clicked, setBackgroundColor);

	lineEdit = new QLineEdit;
	lineEdit->setToolTip("Text Input");
	QObject::connect(lineEdit, &QLineEdit::textEdited, setText);

	QLineEdit *unlinkedLineEdit = new QLineEdit;
	unlinkedLineEdit->setToolTip("Unlinked Text Input");

	QVBoxLayout *dummyLayout = new QVBoxLayout(nullptr);
	dummyLayout->addStretch(1);

	QVBoxLayout *vLayout = new QVBoxLayout(nullptr);
	vLayout->addLayout(firstLineLayout);
	vLayout->addLayout(secondLineLayout);
	vLayout->addWidget(angleLabel);
	vLayout->addWidget(scaleSlider);
	vLayout->addWidget(colorButton);
	vLayout->addWidget(lineEdit);
	vLayout->addWidget(unlinkedLineEdit);
	vLayout->addLayout(dummyLayout);

	ncine::Qt5Widget *ncWidget = new nc::Qt5Widget(createAppEventHandler);
	const nc::AppConfiguration appCfg = nc::theApplication().appConfiguration();
	QWidget window;
	window.setWindowTitle(appCfg.windowTitle.data());
	window.setWindowIcon(QIcon((nc::fs::dataPath() + appCfg.windowIconFilename).data()));

	QHBoxLayout *hLayout = new QHBoxLayout;
	hLayout->addLayout(vLayout);
	hLayout->addWidget(ncWidget);

	QWidget *w = new QWidget(&window);
	w->setLayout(hLayout);
	const int qt5GuiWidth = vLayout->sizeHint().width();
	nc::theApplication().gfxDevice().setResolution(appCfg.resolution.x - qt5GuiWidth, appCfg.resolution.y);
	const int screenWidth = nc::theApplication().gfxDevice().currentVideoMode().width;
	const int screenHeight = nc::theApplication().gfxDevice().currentVideoMode().height;
	window.setGeometry((screenWidth - appCfg.resolution.x) / 2, (screenHeight - appCfg.resolution.y) / 2, appCfg.resolution.x, appCfg.resolution.y);
	window.setMinimumSize(appCfg.resolution.x, appCfg.resolution.y);
	window.setMaximumSize(appCfg.resolution.x, appCfg.resolution.y);

	window.show();

	return app.exec();
}

#endif
