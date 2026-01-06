#pragma once
#include "mixr/simulation/Station.hpp"
#include "GameDisplay.hpp"

class Station : public mixr::simulation::Station
{
	DECLARE_SUBCLASS(Station, mixr::simulation::Station)

public:
	Station();
	
	GameDisplay* getDisplay();

	bool event(const int event, mixr::base::Object* const obj = nullptr) final;

private:

	//bool ballLeft();
	//bool ballRight();

};

