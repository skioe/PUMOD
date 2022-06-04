#ifdef CAMERA_EXPORTS
#define CAMERA_API __declspec(dllexport)
#else
#define CAMERA_API __declspec(dllimport)
#endif

struct SCamera
{
	float x, y, z;
	float xRot, zRot;
};

extern "C" CAMERA_API SCamera camera;
extern "C" CAMERA_API float mapGetHeight(float x, float y);
extern "C" CAMERA_API void cameraApply();
extern "C" CAMERA_API void cameraRotation(float xAngle, float zAngle);
extern "C" CAMERA_API void cameraAutoMouseMove(int centerX, int centerY, float speed);
extern "C" CAMERA_API void cameraMoveDirection(int forwardMove, int rightMove, float speed);
