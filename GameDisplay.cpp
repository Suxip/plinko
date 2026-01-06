#include "GameDisplay.hpp"
#include "Station.hpp"
#include "mixr/base/PairStream.hpp"
#include "mixr/base/Pair.hpp"
#include "mixr/graphics/Polygon.hpp"
#include "mixr/graphics/BmpTexture.hpp"
#include "mixr/graphics/Shapes.hpp"
#include "mixr/graphics/Page.hpp"
#include "Ball.hpp"
#include <GL/glut.h>

IMPLEMENT_SUBCLASS(GameDisplay, "GameDisplay")
EMPTY_DELETEDATA(GameDisplay)
EMPTY_SLOTTABLE(GameDisplay)

BEGIN_EVENT_HANDLER(GameDisplay)
	ON_EVENT(LEFT_ARROW_KEY, ballLeft)
	ON_EVENT(RIGHT_ARROW_KEY, ballRight)
	ON_EVENT(SPACE_BAR, ballDrop)
	ON_EVENT(ENTER_KEY, send)

END_EVENT_HANDLER()

GameDisplay::GameDisplay() 
{
	STANDARD_CONSTRUCTOR()
};

void GameDisplay::copyData(const GameDisplay& org, const bool)
{
	BaseClass::copyData(org);

	startX = org.startX;
	startY = org.startY;
	dragging = org.dragging;
}

//each time updateData is called, I want the next texture to be set until it reaches the end and then i want it to start from the beginning
void GameDisplay::updateData(const double dt)
{
	BaseClass::updateData(dt);

	mixr::base::Pair* obj = subpage()->findByName("background");

	if (obj != nullptr) {
		
		mixr::graphics::Polygon* poly = dynamic_cast<mixr::graphics::Polygon*>(obj->object()); 
		
		if (isLoaded == false) {
			mixr::graphics::BmpTexture* texture = new mixr::graphics::BmpTexture();

			texture->setTexturePath("../../mixr-data/textures/pusheen_strawberry"); // i wonder if doing this works: MIXR_DATA_TEXTURES_PATH

			std::string filename = "resized" + std::to_string(count) + ".bmp";
			mixr::base::String name(filename.c_str());
			texture->setTextureFileName(name);

			mixr::base::PairStream* pairstream = getTextures();    // Returns a ptr to the list of textures

			std::string slotname = "tex" + std::to_string(count + 1); // changed to count+1 bc now i have tex1
			mixr::base::String slot(slotname.c_str());
			//std::cout << slot;

			mixr::base::Pair* pair = new mixr::base::Pair(slot, texture);    // Returns a ptr to the list of textures

			pairstream->put(pair);

			loadTextures(); // apparently you do need this??? apparently this is importnat well i'll be. without it it didn't run. i'm guessing it does some sort of loading of the new pairstream once these new pairs have been added? order might not be right - should go in for loop?

			poly->setTexture(count + 1); // changed to count+1 bc now i have tex1
		}
		else {
			poly->setTexture(count + 1); // changed to count+1 bc now i have tex1
		}
	}

}


bool GameDisplay::send() {
	Station* station = dynamic_cast<Station*>(container());
	station->send("controller", 4003);
	return true;

}

void GameDisplay::buttonEvent(const int b) {
	Station* station = dynamic_cast<Station*>(container());

	switch (b) {
	case 3001:
		newSubpage("gameScreen", nullptr);
		break;
	// reset
	case 3002:
		//std::cout << "rest/go back to home button";
		//newSubpage("homeScreen", nullptr);
		station->send("controller", 4000);
		break;
	// add ball
	case 3003:
		addBall();
		station->send("controller", 4001);
		break;
	// change color
	case 3004:
		//std::cout << "change color";
		//station->send("controller", 4002);
		newSubpage("colorScreen", nullptr);
		break;
	case 3005:
		color = new mixr::base::Identifier("red");
		getDemoBall(color);
		break;
	case 3006:
		color = new mixr::base::Identifier("blue");
		getDemoBall(color);
		break;
	case 3007:
		color = new mixr::base::Identifier("cyan");
		getDemoBall(color);
		break;
	case 3008:
		color = new mixr::base::Identifier("green");
		getDemoBall(color);
		break;
	}
}

void GameDisplay::addBall() {
	Ball* ball = new Ball();

	ball->setColor(color);
	ball->setRadius(0.4);
	ball->setFilled(true);
	
	std::string slotname = "ball" + std::to_string(ballCount); 
	mixr::base::String slot(slotname.c_str());
	mixr::base::Pair* pair = new mixr::base::Pair(slot, ball);   

	ball->lcTranslate(0, 8);
	subpage()->addComponent(pair);

	ballCount++; 
}

bool GameDisplay::ballLeft() {
	std::string slotname = "ball" + std::to_string(ballCount);// changed
	mixr::base::String slot(slotname.c_str());// changed

	mixr::base::Pair* pair = subpage()->findByName(slot);// changed
	if (pair != nullptr) {
		mixr::graphics::Circle* ball = dynamic_cast<mixr::graphics::Circle*>(pair->object());
		ball->lcTranslate(-1, 0, 0);
	}

	Station* station = dynamic_cast<Station*>(container()); //what is a component again? difference with pair?
	station->send("controller", 4004);
	return true;
}

bool GameDisplay::ballRight() {
	std::string slotname = "ball" + std::to_string(ballCount);// changed
	mixr::base::String slot(slotname.c_str());// changed

	mixr::base::Pair* pair = subpage()->findByName(slot);// changed
	if (pair != nullptr) {
		mixr::graphics::Circle* ball = dynamic_cast<mixr::graphics::Circle*>(pair->object());
		ball->lcTranslate(1, 0, 0);
	}

	Station* station = dynamic_cast<Station*>(container()); 
	station->send("controller", 4005);
	return true;
}

bool GameDisplay::ballDrop() {
	Station* station = dynamic_cast<Station*>(container()); 
	station->send("controller", 4002);
	return true;
}

void GameDisplay::passiveMotionEvent(const int x, const int y)
{
	BaseClass::passiveMotionEvent(x, y);
	if (!dragging) {
		startX = x;
		startY = y;
	}
}


void GameDisplay::mouseMotionEvent(const int x, const int y)
{
	if (dragging) {

		auto* page = subpage();

		if (page->findByName("sthumb") != nullptr && sDragging) {
			mixr::graphics::Polygon* sthumb = dynamic_cast<mixr::graphics::Polygon*>(page->findByName("sthumb")->object());

			float dx = x - startX;
			float dy = y - startY;

			dx = dx * 20 / 800;

			//std::cout << "x: " << x << std::endl << "y: " << y << std::endl;
			//std::cout << "startX: " << startX << std::endl << "startY: " << startY << std::endl;
			//std::cout << "dx: " << dx << std::endl << "dy: " << dy << std::endl;
			//std::cout << "sRange: " << sRange << std::endl;

			//if (dx > 0 && sRange <= 4.8) {
			//	sthumb->lcTranslate(dx, 0);  // so has to be one small number in the x direction
			//	sRange += dx;
			//}
			//else if (dx < 0 && sRange >= 0) {
			//	sthumb->lcTranslate(-dx, 0);
			//	sRange -= dx;
			//}

			if (dx + sRange <= 4.9 && sRange + dx >= 0) {
				sthumb->lcTranslate(dx, 0);
				sRange += dx;
			}

			Station* station = dynamic_cast<Station*>(container());
			SendData sd;
			station->send("controller", 4008, sRange, sd);
		}

		if (page->findByName("gthumb") != nullptr && gDragging) {
			mixr::graphics::Polygon* gthumb = dynamic_cast<mixr::graphics::Polygon*>(page->findByName("gthumb")->object());

			float dx = x - startX; // pixels
			float dy = y - startY; // pixels

			dx = dx * 20 / 800;

			// one tiny move of the mouse results in one pixel in this case
			std::cout << "x: " << x << std::endl << "y: " << y << std::endl;
			std::cout << "startX: " << startX << std::endl << "startY: " << startY << std::endl;
			std::cout << "dx: " << dx << std::endl << "dy: " << dy << std::endl;
			std::cout << "gRange: " << gRange << std::endl;

			if (dx + gRange <= 4.9 && gRange + dx >= 0) {
				gthumb->lcTranslate(dx, 0);  // so has to be one small number in the x direction
				gRange += dx;
			}
			Station* station = dynamic_cast<Station*>(container());
			SendData sd;
			station->send("controller", 4007, gRange, sd);
		}
		startX = x;
		startY = y;
	}
	setMouse(x, y);
}

void GameDisplay::mouseEvent(const int button, const int state, const int x, const int y)
{
	BaseClass::mouseEvent(button, state, x, y);

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) { dragging = false; }
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && !dragging) { 
		gDragging = false;
		sDragging = false;

		//speed
		if (x >= 40 && x <= 240 && y >= 105 && y <= 135) {
			sDragging = true;
		}
		//gravity
		else if (x >= 40 && x <= 240 && y >= 35 && y <= 65) {
			gDragging = true;
		}

		dragging = true;
	}
}

void GameDisplay::getDemoBall(mixr::base::Identifier* id) {
	auto* demoBall = dynamic_cast<mixr::graphics::Circle*>(subpage()->findByName("demoBall")->object());
	demoBall->setColor(id);
}
