#pragma once


class Camera
{
public:
	static const int CAMERA_WIDTH;
	static const int CAMERA_HEIGHT;
	int x = 0;
	int y = 0;
	static Camera& getInstance() {
		static Camera singleton;
		return singleton;
	}
	bool move(int newx, int newy, int maxx, int maxy);
	bool to_screen(int x, int y, int& screen_x, int& screen_y);
	void to_map(int x, int y, int& mapx, int& mapy);
private:
	Camera();
	~Camera();
	Camera(const Camera&); // disable copy constructor
	Camera& operator=(const Camera&); // disable assignment
};

