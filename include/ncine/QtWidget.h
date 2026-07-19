#ifndef CLASS_NCINE_QTNWIDGET
#define CLASS_NCINE_QTNWIDGET

#include "common_macros.h"
#include <QOpenGLWidget>

#if defined(_WIN32) && defined(ERROR)
	#undef ERROR
#endif

namespace ncine {

class PCApplication;
class IAppEventHandler;

}

class QWindow;

namespace ncine {

/// The Qt nCine widget
class DLL_PUBLIC QtWidget : public QOpenGLWidget
{
	Q_OBJECT

  public:
	explicit QtWidget(nctl::UniquePtr<IAppEventHandler> (*createAppEventHandler)())
	    : QtWidget(nullptr, createAppEventHandler, 0, nullptr) {}
	QtWidget(nctl::UniquePtr<IAppEventHandler> (*createAppEventHandler)(), int argc, char **argv)
	    : QtWidget(nullptr, createAppEventHandler, argc, argv) {}
	QtWidget(QWidget *parent, nctl::UniquePtr<IAppEventHandler> (*createAppEventHandler)(), int argc, char **argv);
	~QtWidget();

	/// If set to false the widget will stop to automatically update each frame
	inline void setShouldUpdate(bool shouldUpdate) { shouldUpdate_ = shouldUpdate; }

	IAppEventHandler &appEventHandler();

	/// Makes the widget's rendering context the current OpenGL rendering context
	void makeCurrent();

  protected:
	bool event(QEvent *event) override;

	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;

	QSize minimumSizeHint() const override;
	QSize sizeHint() const override;

  private slots:
	/// The function slot called at each `frameSwapped` signal
	void autoUpdate();
	/// The function slot called at each `screenAdded` or `screenRemoved` signal
	void screenConfigurationChange(QScreen *);

  private:
	PCApplication &application_;
	nctl::UniquePtr<IAppEventHandler> (*createAppEventHandler_)();
	bool isInitialized_;
	bool shouldUpdate_;

	void shutdown();

	/// Hiding the original method to replace it with the custom one that resets the FBO binding
	using QOpenGLWidget::makeCurrent;

	/// Deleted copy constructor
	QtWidget(const QtWidget &) = delete;
	/// Deleted assignment operator
	QtWidget &operator=(const QtWidget &) = delete;
};

}

#endif
