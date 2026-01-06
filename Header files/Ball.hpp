#pragma once
#include "mixr/graphics/Shapes.hpp"

class Ball final : public mixr::graphics::Circle
{
	DECLARE_SUBCLASS(Ball, mixr::graphics::Circle)

public:
	Ball();

	bool event(const int event, mixr::base::Object* const obj = nullptr) final;
	
	void updateData(const double dt = 0.0) final;

	float getX() { return x; }
	float getYi() { return yi; }
	float getYf() { return yf; }
	float getRad() { return radius; }
	float getDx() { return dx; }
	float getDy() { return dy; }
	float getVx() { return vx; }
	float getVy() { return vy; }
	float getSpeed() { return speed; }

	void setX(float num) { x = num; }
	void setYi(float num) { yi = num; }
	void setYf(float num) { yf = num; }
	void setRad(float num) { radius = num; }
	void setDx(float num) { dx = num; }
	void setDy(float num) { dy = num; }
	void setVx(float num) { vx = num; }
	void setVy(float num) { vy = num; }
	void setSpeed(float num) { speed = num; }

	void setTheta(float num) { theta = num; }
	float getTheta() { return theta; }

	void setCount(int num) { count = num; }
	int getCount() { return count; }

	bool isDropped{ false };
	bool isBallReset{ false };
	bool hitBasket{ false };
private:
	float x = 0.0;
	float yi = 8;
	float yf = 8;

	float radius = 0.4;
	
	float dy = 0;
	float dx = 0;

	float vx = 0;
	float vy = 0;

	float speed = 0;

	float theta = 0;

	int count = 0;
};

