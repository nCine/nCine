#include "Camera.h"
#include "Application.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

Camera::Camera()
    : projectionValues_(0.0f, static_cast<float>(theApplication().width()),
                        0.0f, static_cast<float>(theApplication().height())),
      viewValues_(0.0f, 0.0f, 0.0f, 1.0f),
      projection_(Matrix4x4f::ortho(projectionValues_.left, projectionValues_.right,
                                    projectionValues_.top, projectionValues_.bottom,
                                    projectionValues_.near, projectionValues_.far)),
      view_(Matrix4x4f::Identity),
      updateFrameProjectionMatrix_(0), updateFrameViewMatrix_(0)
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void Camera::setOrthoProjection(float left, float right, float top, float bottom)
{
	projectionValues_.left = left;
	projectionValues_.right = right;
	projectionValues_.top = top;
	projectionValues_.bottom = bottom;

	projection_ = Matrix4x4f::ortho(projectionValues_.left, projectionValues_.right,
	                                projectionValues_.top, projectionValues_.bottom,
	                                projectionValues_.near, projectionValues_.far);
	updateFrameProjectionMatrix_ = theApplication().numFrames();
}

void Camera::setOrthoProjection(const ProjectionValues &values)
{
	setOrthoProjection(values.left, values.right, values.top, values.bottom);
}

void Camera::setView(float x, float y, float rotation, float scale)
{
	viewValues_.x = x;
	viewValues_.y = y;
	viewValues_.rotation = rotation;
	viewValues_.scale = scale;

	// Invert translation as the world is moving opposite to the camera
	view_ = Matrix4x4f::translation(-x, -y, 0.0f);
	// Invert translation as the world is rotating opposite to the camera
	view_.rotateZ(-rotation);
	view_.scale(scale, scale, 1.0f);
	updateFrameViewMatrix_ = theApplication().numFrames();
}

void Camera::setView(const Vector2f &pos, float rotation, float scale)
{
	setView(pos.x, pos.y, rotation, scale);
}

void Camera::setView(const ViewValues &values)
{
	setView(values.x, values.y, values.rotation, values.scale);
}

}
