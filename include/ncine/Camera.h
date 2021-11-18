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
	/// Creates a camera with default matrices
	Camera();

	void setOrtoProjection(const Rectf &orthoRect, float near, float far);

	inline const Matrix4x4f &projection() const { return projection_; }
	inline const Matrix4x4f &model() const { return model_; };

	inline unsigned long int updateFrameProjectionMatrix() const { return updateFrameProjectionMatrix_; }
	inline unsigned long int updateFrameModelMatrix() const { return updateFrameModelMatrix_; }

  private:
	/// Last frame when the projection matrix was changed
	unsigned long int updateFrameProjectionMatrix_;
	/// Last frame when the model matrix was changed
	unsigned long int updateFrameModelMatrix_;
	Matrix4x4f projection_;
	Matrix4x4f model_;
};

}

#endif
