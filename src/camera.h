#ifndef __METAHUMAN_CAMERA_H__
#define __METAHUMAN_CAMERA_H__
#include <glm/glm.hpp>

namespace Metahuman {

	struct RotateAxis {
		double pitch;
		double yaw;
		double roll;
		RotateAxis(double pitch, double yaw, double roll) {
			this->pitch = pitch;
			this->yaw = yaw;
			this->roll = roll;
		}
	};

	class Camera {
	private :
		double fov;
		double fovSpeed;
		double nearClip;
		double farClip;
		float aspectRatio;
		
		glm::dvec3 eye;
		glm::dvec3 center;
		glm::dvec3 up;
		RotateAxis axis;
		double radius;  // center로부터 거리

		void updateEyeFromOrbit();
	public :
		Camera();
		~Camera();

		double GetFovScale() const;
		void SetFovSpeed(double fov_speed);
		double GetRadius() const;
		RotateAxis GetAxis() const;
		

		void FovIn();
		void FovOut();
		void ApplyProjection(float aspectRatio);
		void ApplyView();
		void Rotate(double deltaPitch, double deltaYaw, double deltaRoll);
		void Zoom(double delta);
	};
};

#endif//__METAHUMAN_CAMERA_H__
