#include "TestGameController.hpp"
#include "Station.hpp"
#include "GameDisplay.hpp"
#include "mixr/base/Pair.hpp"
#include "mixr/base/PairStream.hpp"

#include "mixr/graphics/Shapes.hpp"
#include "Ball.hpp"
#include "mixr/base/colors/Color.hpp"
#include "mixr/graphics/Polygon.hpp"
#include "mixr/base/numeric/Number.hpp"
#include <vector>
#include <cmath>

using namespace std;

IMPLEMENT_SUBCLASS(TestGameController, "TestGameController")
EMPTY_DELETEDATA(TestGameController)
EMPTY_SLOTTABLE(TestGameController)
EMPTY_COPYDATA(TestGameController)

BEGIN_EVENT_HANDLER(TestGameController)
    ON_EVENT(4000, resetGame)
    ON_EVENT(4001, isBallAdded) 
    ON_EVENT(4002, isBallDrop)
    ON_EVENT(4004, ballLeft) 
    ON_EVENT(4005, ballRight) 
    ON_EVENT_OBJ(4007, changeGravity, mixr::base::Number)
    ON_EVENT_OBJ(4008, changeSpeed, mixr::base::Number)
END_EVENT_HANDLER()


TestGameController::TestGameController() {
    STANDARD_CONSTRUCTOR()
        pegCoords();
}

void TestGameController::updateTC(const double dt)
{
    BaseClass::updateTC(dt);

    moveBasket(dt);

    for (Ball* ball : balls) {
        
        currentBall = ball;

        pegCollision(ball, dt);
        wallCollision(ball, dt);
        
        if (ball->isDropped) { 
            ballDrop(ball, dt);
        }

        if (ball->getYf() < -9) { 
            ball->isDropped = false;
            ball->isBallReset = true;
        }

        if (ball->isBallReset) { 
            resetBall(ball);
        }

        if (!ball->hitBasket) {
            if (hitDetection(ball)) {
                score++;
                auto* page = getPage();
                page->send("scoreval", UPDATE_VALUE, score, sd);

                ball->isDropped = false;
                ball->isBallReset = true;
            }
        }

        cout << "vy: " << ball->getVy() << endl;
    }
}

// this takes the fallen ball and resets it back to the top
//void TestGameController::resetBall(Ball* ball) {
//    if (ball->getX() < 0) {
//        ball->lcTranslate(ball->getX(), 8 - ball->getYf());
//        std::cout << "landed to the left";
//        std::cout << "ball_x: " << ball->getX();
//    }
//    else if (ball->getX() > 0) {
//        ball->lcTranslate(-ball->getX(), 8 - ball->getYf());
//        std::cout << "landed to the right";
//        std::cout << "ball_x: " << ball->getX();
//    }
//    else {
//        ball->lcTranslate(0, 8 - ball->getYf());
//        std::cout << "landed in the center";
//    }
//
//    ball->setX(0);
//    ball->setDy(0);
//    ball->setYi(8);
//    ball->setYf(8);
//    ball->setVy(0);
//    ball->setVx(0);
//    ball->setDx(0);
//
//    ball->isBallReset = false;
//}


void TestGameController::resetBall(Ball* ball) {

    auto it = std::find(balls.begin(), balls.end(), ball);
    if (it != balls.end()) {
        balls.erase(it);
    }


    auto* page = getPage();

    mixr::base::PairStream* components = page->getComponents();

    std::string slotname = "ball" + std::to_string(ball->getCount());
    mixr::base::String slot(slotname.c_str());
    mixr::base::Pair* pair = page->findByName(slot);
    components->remove(pair);
}

bool TestGameController::resetGame() {

    g = 9.8;
    score = 0;
    SendData sd;
    auto* page = getPage();
    page->send("scoreval", UPDATE_VALUE, score, sd);


    mixr::base::PairStream* components = page->getComponents();

    for (Ball* ball : balls) {
        std::string slotname = "ball" + std::to_string(ball->getCount());
        mixr::base::String slot(slotname.c_str());
        mixr::base::Pair* pair = page->findByName(slot);
        components->remove(pair);

    }

    balls.clear();

    if (page->findByName("gthumb") != nullptr) {
        mixr::graphics::Polygon* gthumb = dynamic_cast<mixr::graphics::Polygon*>(page->findByName("gthumb")->object());
        gthumb->lcTranslate(-gNumber, 0);
        Station* station = dynamic_cast<Station*>(container());
        GameDisplay* display = station->getDisplay();
        display->setGRange(0);
    }

    if (page->findByName("sthumb") != nullptr) {
        mixr::graphics::Polygon* sthumb = dynamic_cast<mixr::graphics::Polygon*>(page->findByName("sthumb")->object());
        sthumb->lcTranslate(-sNumber, 0);
        Station* station = dynamic_cast<Station*>(container());
        GameDisplay* display = station->getDisplay();
        display->setSRange(0);
    }

    return true;
}

bool TestGameController::isBallDrop() {
    for (Ball* ball : balls) {
        if (!ball->isDropped) ball->isDropped = true;
    }
    return true;
}

/* Returns true if ball collided with basket, false otherwise */
bool TestGameController::hitDetection(Ball* ball) {

    if (ball->getYf() + ball->getRad() <= basketY + 0.7 &&
        ball->getYf() - ball->getRad() >= basketY - 0.7 &&
        ball->getX() + ball->getRad() <= basketX + basketRad &&
        ball->getX() - ball->getRad() >= basketX - basketRad) {

        ball->hitBasket = true;
        return true;
    }
    else {
        return false;
    }

}

/* Loops through the pegs and if the ball has collided with a peg, isCollided becomes true and angle of collision is calculated (theta) */
void TestGameController::pegCollision(Ball* ball, const double dt) {
    for (const auto& peg : pegs) {
        float x_dist = ball->getX() - peg.front();
        float y_dist = ball->getYf() - peg.back();
        float x_dist_sq = x_dist * x_dist;
        float y_dist_sq = y_dist * y_dist;
        float radiusSum = ball->getRad() + pegRad;

        // checks whether the ball and peg collided
        if (x_dist_sq + y_dist_sq <= radiusSum * radiusSum) {
            ball->setTheta(atan2(y_dist, x_dist));
            ball->setVy(ball->getSpeed() * sin(ball->getTheta()) * dampen);
            ball->setVx(ball->getSpeed() * cos(ball->getTheta()) * dampen);

            // to deal with straight vertical
            if (ball->getTheta() - verticalAngle < 1e-5 && ball->getTheta() - verticalAngle > -1e-5) {
                ball->setDx(ball->getVx() * dt + 0.01);
                //std::cout << "ball->getTheta()" << ball->getTheta() << std::endl;
                //std::cout << "verticalAngle" << verticalAngle << endl;
                //cout << "ball->getTheta() - verticalAngle < 1e-5? " << ball->getTheta() - verticalAngle << std::endl;
                //cout << "ball->getTheta() - verticalAngle > -1e-5" << ball->getTheta() - verticalAngle << std::endl << std::endl;

            }
            else {
                ball->setDx(ball->getVx() * dt);
            }

            //std::cout << "this is theta: " << ball->getTheta() << std::endl;
            //std::cout << "vy: " << ball->getVy() << std::endl;
            //std::cout << "cos(theta): " << cos(ball->getTheta()) << std::endl;
            //std::cout << "vx: " << ball->getVx() << std::endl;
            //std::cout << "dx: " << ball->getDx() << std::endl;
            //std::cout << std::endl << std::endl;
        }
    }
}


void TestGameController::wallCollision(Ball* ball, const double dt) {
    float x_dist = ball->getX() - 10;
    float y_dist = ball->getYf() - 10;

    if (ball->getX() <= -10 + ball->getRad() || ball->getX() >= 10 - ball->getRad()) {
        ball->setDx(-1 * ball->getDx() * dampen);

        //ball->setVx(-1 * ball->getVx() * dampen);
        //ball->setDx(ball->getVx() * dt);
    }
}

/* when ball is dropped, translate ball down by increasing amounts according to acceleration of gravity */
void TestGameController::ballDrop(Ball* ball, const double dt) {
    float tempDy = ball->getVy() * dt + 0.5 * -g * pow(dt, 2);
    ball->setDy(tempDy);
    float tempSpeed = sqrt(ball->getVy() * ball->getVy() + ball->getVx() * ball->getVx());
    ball->setSpeed(tempSpeed);

    ball->lcTranslate(ball->getDx(), ball->getDy());

    ball->setX(ball->getX() + ball->getDx());
    ball->setYf(ball->getYf() + ball->getDy());
    ball->setVy(ball->getVy() - (g * dt));
}

void TestGameController::moveBasket(const double dt) {
    auto* page = getPage();
    if (page->findByName("basket") != nullptr) {
        mixr::graphics::Polygon* basket = dynamic_cast<mixr::graphics::Polygon*>(page->findByName("basket")->object());
        if (basketD >= 0 && basketD < 9) {
            basket->lcTranslate(basketSpeed * dt, 0, 0);
            basketD = basketSpeed * dt + basketD;
            basketX = basketSpeed * dt + basketX;
        }
        else if (basketD >= 9 && basketD < 27) {
            basket->lcTranslate(-basketSpeed * dt, 0, 0);
            basketD = basketSpeed * dt + basketD;
            basketX = basketX - basketSpeed * dt;
        }
        else if (basketD >= 27 && basketD < 36) {
            basket->lcTranslate(basketSpeed * dt, 0, 0);
            basketD = basketSpeed * dt + basketD;
            basketX = basketSpeed * dt + basketX;

        }
        else {
            basketD = 0;
            basketX = 0;

        }
    }
}

bool TestGameController::ballLeft() {
    Ball* finalBall = balls.back();
    if (finalBall != nullptr && !finalBall->isDropped) {
        finalBall->lcTranslate(-1,0);
        finalBall->setX(finalBall->getX() - 1);
    }

    return true;
}

bool TestGameController::ballRight() {
    Ball* finalBall = balls.back();
    if (finalBall != nullptr && !finalBall->isDropped) {
        finalBall->lcTranslate(1, 0);
        finalBall->setX(finalBall->getX() + 1);
    }
    return true;
}

void TestGameController::addPeg(float x, float y, int count) {
    for (int i = 0; i < count; ++i) {
        std::vector<float> peg = { x, y };
        pegs.push_back(peg);
        x += 3;
    }
}

void TestGameController::pegCoords() {
    addPeg(-7.5, 4.5, 6);
    addPeg(-7.5, 0.5, 6);
    addPeg(-7.5, -3.5, 6);
    addPeg(-6, 2.5, 5);
    addPeg(-6, -1.5, 5);
}

Ball* TestGameController::getBall() {
    auto* page = getPage();

    if (page->findByName("ball") != nullptr) {
        Ball* ball = dynamic_cast<Ball*>(page->findByName("ball")->object());
        return ball;
    }   
    return nullptr;
}

/* Adds ball with corresponding slot name to balls vector. Ex: When "ball1" is added in GameDisplay, that object is found and added to the balls vector. */
bool TestGameController::isBallAdded() {
    auto* page = getPage();

    std::string slotname = "ball" + std::to_string(ballCount); 
    mixr::base::String slot(slotname.c_str());
    
    if (page->findByName(slot) != nullptr) {

        Ball* ball = dynamic_cast<Ball*>(page->findByName(slot)->object());

        ball->setCount(ballCount);
        
        balls.push_back(ball);
    }

    ++ballCount;

    return true;
}

/* Slider pixels will be 0-4.8, gravity will be 5-30 */
bool TestGameController::changeGravity(const mixr::base::Number* const num) {
    g = num->getFloat() * 5 + 5;

    gNumber = num->getFloat();

    return true;
}

/* Slider pixels will be 0-4.8 */
/* hstarting from 0 to 4.8 and then 0 is the current speed up until 12 speed 
and then moving until then it will increase linearly by that amoutn....
and then what would happen when it goes back down???? okay yeah i think 
that would work */
bool TestGameController::changeSpeed(const mixr::base::Number* const num) {

    //sNumber = num->getFloat();
    //float ratio = (1.3 - 0.5) / 5 * num->getFloat() + 0.5;
    //currentBall->setVy(currentBall->getVy()*ratio);
    //std::cout << "ratio: " << ratio << std::endl << endl;
    //std::cout << "new vy: " << currentBall->getVy() << std::endl <<endl;

    sNumber = num->getFloat();

    float ratio = (1.3 - 0.5) / 5 * num->getFloat() + 0.5;

    currentBall->setVy(currentBall->getVy() * ratio);

    std::cout << "ratio: " << ratio << std::endl << endl;
    std::cout << "new vy: " << currentBall->getVy() << std::endl << endl;

    return true; 
}


// probably won't do this
bool TestGameController::changeDampen(const mixr::base::Number* const num) {
    float ratio = (1.2 - 0.5) / 5 * num->getFloat();

    dampen = ratio + 0.5;

    cout << "dampen: " << dampen << endl;
    return true;
}


mixr::graphics::Page* TestGameController::getPage() {
    Station* station = dynamic_cast<Station*>(container());
    GameDisplay* display = station->getDisplay();
    mixr::graphics::Page* page = dynamic_cast<mixr::graphics::Page*>(display->subpage());
    return page;
}