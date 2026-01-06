#pragma once

#define _USE_MATH_DEFINES

#include "mixr/models/WorldModel.hpp"
#include "mixr/base/numeric/Number.hpp"
#include "mixr/graphics/Page.hpp"
#include "mixr/base/List.hpp"
#include "Ball.hpp"
#include <vector>
#include <math.h>

class TestGameController final : public mixr::models::WorldModel
{
	DECLARE_SUBCLASS(TestGameController, mixr::models::WorldModel)

public:
	TestGameController();

	bool event(const int event, mixr::base::Object* const obj = nullptr) final;

	void updateTC(const double dt = 0.0) final;

private:
	std::vector<std::vector<float>> pegs;

	int score = 0;

	void resetBall(Ball* ball);

	bool isBallAdded();
	bool resetGame();
	bool isBallDrop();
	//bool addBall();
	bool ballLeft();
	bool ballRight();

	void pegCoords();

	void addPeg(float x, float y, int count);

	void ballDrop(Ball* ball, const double dt);
	void wallCollision(Ball* ball, const double dt);
	void pegCollision(Ball* ball, const double dt);
	bool hitDetection(Ball* ball);

	void moveBasket(const double dt);

	Ball* getBall();

	std::vector<Ball*> balls;
	int ballCount = 1; // 

	bool isDropped{ false };
	bool isBallReset{ false };
	bool speedChanged{ false };

	//basket
	float basketSpeed = 3;

	float basketX = 0; //xposition
	float basketD = 0; //distance
	float basketY = -8.5; //yposition
	float basketRad = 1.7; //half dist between the two widest points

	float pegRad = 0.4;
	float g = 9.8;
	float verticalAngle = M_PI / 2;
	float dampen = 0.8;

	Ball* currentBall;

	SendData sd;

	bool changeGravity(const mixr::base::Number* const num);
	bool changeSpeed(const mixr::base::Number* const num);
	bool changeDampen(const mixr::base::Number* const num);

	mixr::graphics::Page* getPage();
	
	float gNumber = 0;
	float sNumber = 0;

};

