#include "camera.h"

using namespace Metahuman;

Camera::Camera() {
	
}

Camera::~Camera() {

}

inline void Camera::FovIn(void * datas) {
	fov += fovSpeed;
}

inline  void Camera::FovOut(void * datas) {
	fov -= fovSpeed;
}

double Camera::GetFovScale() const {
	return fov;
}

void Camera::SetFovSpeed(double fov_speed) {
	fovSpeed = fov_speed;
}