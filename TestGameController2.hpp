#pragma once
#include "mixr/models/WorldModel.hpp"
#include "Ball.hpp"
#include <vector>

class TestGameController2 final : public mixr::models::WorldModel
{
	DECLARE_SUBCLASS(TestGameController2, mixr::models::WorldModel)

public:
	TestGameController2();

	void updateTC(const double dt);

	bool event(const int event, mixr::base::Object* const obj = nullptr) final;

private:
	std::vector<std::vector<float>> pegs;

	void pegCoords();
	void addPeg(float x, float y, int count);

	bool ballLeft();
	bool ballRight();
	bool isBallDrop();
	bool isBallAdded();

	Ball* currentBall;

	bool isDropped;
	int ballCount;
	std::vector<Ball*> balls;

};


