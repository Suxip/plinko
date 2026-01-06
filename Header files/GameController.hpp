#pragma once

#define _USE_MATH_DEFINES

#include "mixr/models/WorldModel.hpp"
#include "mixr/base/numeric/Number.hpp"
#include "mixr/base/List.hpp"
#include <vector>
#include <math.h>

class GameController final : public mixr::models::WorldModel
{
	DECLARE_SUBCLASS(GameController, mixr::models::WorldModel)

public:
	GameController();

	bool event(const int event, mixr::base::Object* const obj = nullptr) final;

	void updateTC(const double dt = 0.0) final;

private:
	std::vector<std::vector<float>> pegs;

	int score = 0;

	void resetBall();

	bool resetGame();
	bool ballDrop(const double dt);
	bool isBallDrop();
	//bool addBall();
	bool ballLeft();
	bool ballRight();

	void pegCoords();

	void addPeg(float x, float y, int count);

	bool wallCollision(const double dt);
	bool pegCollision(const double dt);
	bool hitDetection();

	void moveBasket(const double dt); // dunno if i should have this many functions, 
	// would work in display too but im thinking bc i want to be able to influence 
	// basket speed later, i should probably keep speed measurement and stuff in here...
	// tho how am I going to keep track of speed? do I want to affect it with my 
	// framerate? no.

	bool isDropped{ false };
	bool isBallReset{ false };
	//basket
	float basketSpeed = 3;
	//float basket_leftx = -4.5;
	//float basket_rightx = 4.5;
	float basket_x = 0; //xposition
	float basket_d = 0; //distance
	float basket_y = -8.5; //yposition
	float basket_r = 1.1;

	float peg_r = 0.4;
	
	float theta;


	//ball
	float ball_x = 0;
	float ball_r = 0.4;
	float ball_y = 0;

	float yi = 8;
	float yf = 8;
	float g = 9.8;
	
	float vx = 0;
	float vy = 0;
	
	float dx = 0;	
	float dy;

	float speed = 0;
	float verticalAngle = M_PI / 2;



	//void printPegCoords();

};

