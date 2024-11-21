#ifndef CLASS_NCINE_CAMERA
#define CLASS_NCINE_CAMERA

#if defined(_WIN32) && (defined(near) || defined(far))
	#undef near
	#undef far
#endif

#include "common_defines.h"
#include "Matrix4x4.h"

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
		float near = -1.0f;
		float far = 1.0f;

		ProjectionValues()
		    : left(0.0f), right(0.0f), top(0.0f), bottom(0.0f) {}

		ProjectionValues(float ll, float rr, float tt, float bb)
		    : left(ll), right(rr), top(tt), bottom(bb) {}
	};

	struct ViewValues
	{
		Vector2f position;
		float rotation;
		float scale;

		ViewValues()
		    : position(0.0f, 0.0f), rotation(0.0f), scale(1.0f) {}

		ViewValues(float xx, float yy, float rr, float ss)
		    : position(xx, yy), rotation(rr), scale(ss) {}
	};

	/// Creates a camera with default matrices
	Camera();

	/// Returns the projection values that are used to create the projection matrix
	inline const ProjectionValues &projectionValues() const { return projectionValues_; }
	/// Returns the view values that are used to create the model matrix
	inline const ViewValues &viewValues() const { return viewValues_; }

	/// Returns the current projection matrix
	inline const Matrix4x4f &projection() const { return projection_; }
	/// Returns the current model matrix
	inline const Matrix4x4f &view() const { return view_; }

	/// Updates the projection matrix using the projection values
	void setOrthoProjection(float left, float right, float top, float bottom);
	/// Updates the projection matrix using the projection values from the structure
	void setOrthoProjection(const ProjectionValues &values);

	/// Updates the model matrix using the view values (with position as a `Vector2f`)
	void setView(const Vector2f &pos, float rotation, float scale);
	/// Updates the model matrix using the view values
	void setView(float x, float y, float rotation, float scale);
	/// Updates the model matrix using the view values from the structure
	void setView(const ViewValues &values);

	/// Returns the last frame when the projection matrix was changed
	inline unsigned long int updateFrameProjectionMatrix() const { return updateFrameProjectionMatrix_; }
	/// Returns the last frame when the model matrix was changed
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
