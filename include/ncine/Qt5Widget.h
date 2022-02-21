#ifndef CLASS_NCINE_QT5WIDGET
#define CLASS_NCINE_QT5WIDGET

#include "common_macros.h"
#include <QOpenGLWidget>

#ifdef _WIN32
	#undef ERROR
#endif

namespace ncine {

class PCApplication;
class IAppEventHandler;

}

class QWindow;

namespace ncine {

/// The Qt5 nCine widget
class DLL_PUBLIC Qt5Widget : public QOpenGLWidget
{
	Q_OBJECT

  public:
	explicit Qt5Widget(nctl::UniquePtr<IAppEventHandler> (*createAppEventHandler)())
	    : Qt5Widget(nullptr, createAppEventHandler, 0, nullptr) {}
	Qt5Widget(nctl::UniquePtr<IAppEventHandler> (*createAppEventHandler)(), int argc, char **argv)
	    : Qt5Widget(nullptr, createAppEventHandler, argc, argv) {}
	Qt5Widget(QWidget *parent, nctl::UniquePtr<IAppEventHandler> (*createAppEventHandler)(), int argc, char **argv);
	~Qt5Widget();

	/// If set to false the widget will stop to automatically update each frame
	inline void setShouldUpdate(bool shouldUpdate) { shouldUpdate_ = shouldUpdate; }

	IAppEventHandler &appEventHandler();

  protected:
	bool event(QEvent *event) override;

	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;

	QSize minimumSizeHint() const override;
	QSize sizeHint() const override;

  private:
	PCApplication &application_;
	nctl::UniquePtr<IAppEventHandler> (*createAppEventHandler_)();
	bool isInitialized_;
	bool shouldUpdate_;

	void shutdown();

	/// Deleted copy constructor
	Qt5Widget(const Qt5Widget &) = delete;
	/// Deleted assignment operator
	Qt5Widget &operator=(const Qt5Widget &) = delete;
};

}

#endif
