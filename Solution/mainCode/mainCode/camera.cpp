#include "pch.h"

float playerHeight = 3.5;

SCamera camera{ 50, 50, playerHeight, 0, 0 };

void cameraApply()
{
	glRotatef(-camera.xRot, 1, 0, 0);
	glRotatef(-camera.zRot, 0, 0, 1);
	glTranslatef(-camera.x, -camera.y, -camera.z);
}

void cameraRotation(float xAngle, float zAngle)
{
	camera.zRot += zAngle;
	if (camera.zRot < 0) camera.zRot += 360;
	if (camera.zRot > 360) camera.zRot -= 360;

	camera.xRot += xAngle;
	if (camera.xRot < 0) camera.xRot = 0;
	if (camera.xRot > 180) camera.xRot = 180;
}

void cameraAutoMouseMove(int centerX, int centerY, float speed)
{
	POINT cur;
	POINT base = { centerX, centerY };
	GetCursorPos(&cur);
	cameraRotation((base.y - cur.y) * speed, (base.x - cur.x) * speed);
	SetCursorPos(base.x, base.y);
}

void cameraMoveDirection(int forwardMove, int rightMove, float speed)
{
	float angle = -camera.zRot / 180 * M_PI;

	if (forwardMove > 0)
		angle += rightMove > 0 ? M_PI_4 : (rightMove < 0 ? -M_PI_4 : 0);
	if (forwardMove < 0)
		angle += M_PI + (rightMove > 0 ? -M_PI_4 : (rightMove < 0 ? M_PI_4 : 0));
	if (forwardMove == 0)
	{
		angle += rightMove > 0 ? M_PI_2 : -M_PI_2;
		if (rightMove == 0) speed = 0;
	}

	if (speed != 0)
	{
		camera.x += sin(angle) * speed;
		camera.y += cos(angle) * speed;
		camera.z = mapGetHeight(camera.x, camera.y) + playerHeight;
	}

}
