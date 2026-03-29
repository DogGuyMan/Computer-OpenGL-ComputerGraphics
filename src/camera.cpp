#include "camera.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif
#include <cmath>

using namespace Metahuman;

// 도 → 라디안
static double toRad(double deg) { return deg * 3.14159265358979 / 180.0; }

Camera::Camera()
	: fov(60.0), fovSpeed(1.0), nearClip(0.1), farClip(100.0), aspectRatio(1.0f)
	, eye(0, 0, 5)
	, center(0, 0, 0)
	, up(0, 1, 0)
	, axis(15.0, 0.0, 0.0), radius(5.0)
{
	updateEyeFromOrbit();
}

Camera::~Camera() {
}

void Camera::updateEyeFromOrbit() {
	double radPitch = toRad(axis.pitch);
	double radYaw = toRad(axis.yaw);
	eye.x = center.x + radius * cos(radPitch) * sin(radYaw);
	eye.y = center.y + radius * sin(radPitch);
	eye.z = center.z + radius * cos(radPitch) * cos(radYaw);
}

void Camera::FovIn() {
	fov -= fovSpeed;
	if(fov < 1.0) fov = 1.0;
}

void Camera::FovOut() {
	fov += fovSpeed;
	if(fov > 179.0) fov = 179.0;
}

double Camera::GetFovScale() const {
	return fov;
}

double Camera::GetRadius() const {
	return radius;
}

void Camera::SetFovSpeed(double fov_speed) {
	fovSpeed = fov_speed;
}

void Camera::ApplyProjection(float ar) {
	aspectRatio = ar;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, aspectRatio, nearClip, farClip);
}

void Camera::ApplyView() {
	updateEyeFromOrbit();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye.x, eye.y, eye.z, 
		center.x, center.y, center.z, 
		up.x, up.y, up.z
	);
}

void Camera::Rotate(double deltaPitch, double deltaYaw, double deltaRoll) {
	axis.pitch += deltaPitch;
	// pitch 제한: 극점 gimbal lock 방지
	if(axis.pitch > 89.0) axis.pitch = 89.0;
	if(axis.pitch < -89.0) axis.pitch = -89.0;

	// yaw는 360 자유 회전
	axis.yaw += deltaYaw;
	axis.roll += deltaRoll;
}

void Camera::Zoom(double delta) {
	radius += delta;
	if(radius < 0.5) radius = 0.5;
	if(radius > 50.0) radius = 50.0;
}
