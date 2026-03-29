#ifndef __METAHUMAN_CAMERA_H__
#define __METAHUMAN_CAMERA_H__

namespace Metahuman {
	class Camera {
	private : 
		double fov = 60.0;
		double fovSpeed = 1.0;
	public :
		Camera();
		~Camera();
		void FovIn(void * datas);
		void FovOut(void * datas);
		double GetFovScale() const;
		void SetFovSpeed(double fov_speed);
	};
};

#endif//__METAHUMAN_CAMERA_H__