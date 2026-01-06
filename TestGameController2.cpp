#include "TestGameController2.hpp"
#include "Station.hpp"
#include "GameDisplay.hpp"

IMPLEMENT_SUBCLASS(TestGameController2, "TestGameController2")
EMPTY_DELETEDATA(TestGameController2)
EMPTY_SLOTTABLE(TestGameController2)

BEGIN_EVENT_HANDLER(TestGameController2)
    //ON_EVENT(4000, resetGame)
    ON_EVENT(4001, isBallAdded)
    ON_EVENT(4002, isBallDrop)
    ON_EVENT(4004, ballLeft)
    ON_EVENT(4005, ballRight)
END_EVENT_HANDLER()

TestGameController2::TestGameController2() {
    STANDARD_CONSTRUCTOR()
        pegCoords();
}

void TestGameController2::copyData(const TestGameController2& org, const bool) {
    BaseClass::copyData(org);
}

void TestGameController2::updateTC(const double dt) {
    BaseClass::updateTC(dt);
}


void TestGameController2::pegCoords() {
    addPeg(-7.5, 4.5, 6);
    addPeg(-7.5, 0.5, 6);
    addPeg(-7.5, -3.5, 6);
    addPeg(-6, 2.5, 5);
    addPeg(-6, -1.5, 5);
}

void TestGameController2::addPeg(float x, float y, int count) {
    for (int i = 0; i < count; ++i) {
        std::vector<float> peg = { x, y };
        pegs.push_back(peg);
        x += 3;
    }
}

bool TestGameController2::ballLeft() {
    float x = currentBall->getX();
    currentBall->setX(x - 1);
    return true;
}

bool TestGameController2::ballRight() {
    float x = currentBall->getX();
    currentBall->setX(x + 1);
    return true;
}

bool TestGameController2::isBallDrop() {
    isDropped = true;
    return true;
}

/* Adds ball with corresponding slot name to balls vector. Ex: When "ball1" is added in GameDisplay, that object is found and added to the balls vector. */
bool TestGameController2::isBallAdded() {
    //Station* station = dynamic_cast<Station*>(container());
    //GameDisplay* display = station->getDisplay();
    //mixr::graphics::Page* page = display->subpage();

    //std::string slotname = "ball" + std::to_string(ballCount);
    //mixr::base::String slot(slotname.c_str());

    //if (page->findByName(slot) != nullptr) {
    //    Ball* ball = dynamic_cast<Ball*>(page->findByName(slot)->object());
    //    balls.push_back(ball);
    //}

    //++ballCount;

    return true;
}
