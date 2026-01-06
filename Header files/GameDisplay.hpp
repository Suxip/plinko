#pragma once
#include "mixr/ui/glut/GlutDisplay.hpp"
#include "mixr/graphics/BmpTexture.hpp"
#include "mixr/base/Identifier.hpp"

class GameDisplay : public mixr::glut::GlutDisplay
{
	DECLARE_SUBCLASS(GameDisplay, mixr::glut::GlutDisplay)

public:
	GameDisplay();

	void setSRange(float num) { sRange = num; }
	void setGRange(float num) { gRange = num; }
	int getCount() { return count; }
	void setCount(int num) { count = num; }
	void setLoaded(bool boolean) { isLoaded = boolean; }

	void buttonEvent(const int b) final;
	bool event(const int event, mixr::base::Object* const obj = nullptr) final;
	void updateData(const double dt);

	void mouseMotionEvent(const int x, const int y);
	void mouseEvent(const int button, const int state, const int x, const int y) final;
	void passiveMotionEvent(const int x, const int y);

private:
	bool isLoaded = false;

	int count = 1; 
	int ballCount = 1;
	bool ballLeft();
	bool ballRight();
	bool ballDrop();
	bool send();
	void addBall();

	int startX{};         // startX of our last mouse position
	int startY{};         // startY of our last mouse position
	bool dragging{};      // are we dragging the map?
	bool sDragging{};
	bool gDragging{};

	float sRange{};          // range of our speed slider
	float gRange{};          // range of our gravity slider
	
	mixr::base::Identifier* color = new mixr::base::Identifier("red");

	void getDemoBall(mixr::base::Identifier* id);

};

