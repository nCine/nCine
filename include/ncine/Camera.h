#ifndef CLASS_NCINE_CAMERA
#define CLASS_NCINE_CAMERA

#include "common_defines.h"
#include "Matrix4x4.h"
#include "Rect.h"

namespace ncine {

/// The camera class that handles matrices for shaders
class DLL_PUBLIC Camera
{
  public:
	struct ProjectionValues
	{
		float left;
		float right;
		float top;
		float bottom;
		const float near = -1.0f;
		const float far = 1.0f;

		ProjectionValues(float ll, float rr, float tt, float bb)
		    : left(ll), right(rr), top(tt), bottom(bb) {}
	};

	struct ViewValues
	{
		float x;
		float y;
		float rotation;
		float scale;

		ViewValues(float xx, float yy, float rr, float ss)
		    : x(xx), y(yy), rotation(rr), scale(ss) {}
	};

	/// Creates a camera with default matrices
	Camera();

	inline const ProjectionValues &projectionValues() const { return projectionValues_; }
	inline const ViewValues &viewValues() const { return viewValues_; }

	inline const Matrix4x4f &projection() const { return projection_; }
	inline const Matrix4x4f &view() const { return view_; }

	void setOrthoProjection(float left, float right, float top, float bottom);
	void setOrthoProjection(const ProjectionValues &values);

	void setView(float x, float y, float rotation, float scale);
	void setView(const Vector2f &pos, float rotation, float scale);
	void setView(const ViewValues &values);

	inline unsigned long int updateFrameProjectionMatrix() const { return updateFrameProjectionMatrix_; }
	inline unsigned long int updateFrameViewMatrix() const { return updateFrameViewMatrix_; }

  private:
	ProjectionValues projectionValues_;
	ViewValues viewValues_;
	Matrix4x4f projection_;
	Matrix4x4f view_;
	/// Last frame when the projection matrix was changed
	unsigned long int updateFrameProjectionMatrix_;
	/// Last frame when the model matrix was changed
	unsigned long int updateFrameViewMatrix_;
};

}

#endif
