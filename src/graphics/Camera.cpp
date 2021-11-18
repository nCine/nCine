#include "Camera.h"
#include "Application.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

Camera::Camera()
    : updateFrameProjectionMatrix_(0), updateFrameModelMatrix_(0),
      projection_(Matrix4x4f::ortho(0.0f, static_cast<float>(theApplication().width()),
                                    0.0f, static_cast<float>(theApplication().height()),
                                    -1.0f, 1.0f)),
      model_(Matrix4x4f::Identity)
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void Camera::setOrtoProjection(const Rectf &orthoRect, float near, float far)
{
	projection_ = Matrix4x4f::ortho(orthoRect.x, orthoRect.w, orthoRect.y, orthoRect.h, near, far);
	updateFrameProjectionMatrix_ = theApplication().numFrames();
}

}
