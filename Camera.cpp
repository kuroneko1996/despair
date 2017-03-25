#include "Camera.h"

const int Camera::CAMERA_WIDTH = 80;
const int Camera::CAMERA_HEIGHT = 40;

Camera::Camera()
{
}

Camera::~Camera()
{
}

bool Camera::move(int targetx, int targety, int maxx, int maxy)
{
	int newx, newy;
	bool moved = false;
	newx = targetx - CAMERA_WIDTH / 2;
	newy = targety - CAMERA_HEIGHT / 2;

	if (newx < 0) newx = 0;
	if (newy < 0) newy = 0;
	//if (x > (maxx - CAMERA_WIDTH - 1)) newx = maxx - CAMERA_WIDTH - 1;
	//if (y > (maxy - CAMERA_HEIGHT - 1)) newy = maxy - CAMERA_HEIGHT - 1;
	
	if (newx != x || newy != y) {
		x = newx;
		y = newy;
		return true;
	}
	return false;
}

bool Camera::to_screen(int objectx, int objecty, int & screenx, int & screeny)
{
	int newx = objectx - x;
	int newy = objecty - y;

	if (newx < 0 || newy < 0 || newx >= CAMERA_WIDTH || newy >= CAMERA_HEIGHT) {
		return false;
	}
	screenx = newx;
	screeny = newy;
	return true;
}

void Camera::to_map(int x, int y, int & mapx, int & mapy)
{
	mapx = this->x + x;
	mapy = this->y + y;
}

