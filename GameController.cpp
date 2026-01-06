#include "GameController.hpp"
#include "Station.hpp"
#include "GameDisplay.hpp"
#include "mixr/base/Pair.hpp"
#include "mixr/graphics/Shapes.hpp"
#include "Ball.hpp"
#include "mixr/base/colors/Color.hpp"
#include "mixr/graphics/Polygon.hpp"
#include <vector>
#include <cmath>

IMPLEMENT_SUBCLASS(GameController, "GameController")
EMPTY_DELETEDATA(GameController)
EMPTY_SLOTTABLE(GameController)

BEGIN_EVENT_HANDLER(GameController)
    ON_EVENT(4000, resetGame)
    //ON_EVENT(4001, makeBall) // how would I do this???
    ON_EVENT(4002, isBallDrop)
    ON_EVENT(4004, ballLeft) //left
    ON_EVENT(4005, ballRight) //right
END_EVENT_HANDLER()

GameController::GameController() {
    STANDARD_CONSTRUCTOR()
    pegCoords();
    //printPegCoords();
}

void GameController::copyData(const GameController& org, const bool) {
    BaseClass::copyData(org);
}

void GameController::updateTC(const double dt)
{
    BaseClass::updateTC(dt);

    pegCollision(dt);

    wallCollision(dt);

    if (isDropped) {
        ballDrop(dt);
    }
    if (yf < -9) { // fix this later 
        isDropped = false;
        isBallReset = true;
    }
    
    if (isBallReset) {
        resetBall();
    }

    moveBasket(dt);
    
    if (hitDetection()) {
        ++score;
        SendData sd;
        Station* station = dynamic_cast<Station*>(container()); 
        GameDisplay* display = station->getDisplay();
        auto* page = display->subpage();
        page->send("scoreval", UPDATE_VALUE, score, sd);
    }



}

void GameController::resetBall() {
    Station* station = dynamic_cast<Station*>(container()); //what is a component again? difference with pair
    GameDisplay* display = station->getDisplay();
    auto* page = display->subpage();

    if (page->findByName("ball") != nullptr) {
        mixr::graphics::Circle* ball = dynamic_cast<mixr::graphics::Circle*>(page->findByName("ball")->object());

        if (ball_x < 0) { 
            ball->lcTranslate(ball_x, 8 - yf, 0); 
            std::cout << "landed to the left";
            std::cout << "ball_x: " << ball_x;

        }
        else if (ball_x > 0) { 
            ball->lcTranslate(-ball_x, 8 - yf, 0); 
            std::cout << "landed to the right";
            std::cout << "ball_x: " << ball_x;

        }
        else { 
            ball->lcTranslate(0, 8 - yf, 0); 
            std::cout << "in the else";
        }
        
        //why does it work when I include these?
        ball_x = 0;
        dy = 0;
        yi = 8;
        yf = 8;
        vy = 0;
        vx = 0;
        dx = 0;
    }

    isBallReset = false;
}

bool GameController::resetGame() {
    score = 0;
    isBallReset = true;
    SendData sd;
    Station* station = dynamic_cast<Station*>(container());
    GameDisplay* display = station->getDisplay();
    auto* page = display->subpage();
    page->send("scoreval", UPDATE_VALUE, score, sd);
    return true;
}

bool GameController::isBallDrop() {
    isDropped = true;
    return true;
}

bool GameController::hitDetection() {
    //std::cout << "yf + ball_r: " << yf + ball_r << " <= " << "basket_y + 0.5: " << basket_y + 0.5 << std::endl; 
    //std::cout << "yf - ball_r: " << yf - ball_r << " <= " << "basket_y - 0.5: " << basket_y - 0.5 << std::endl;
    //std::cout << "ball_x + ball_r:  " << ball_x + ball_r << " <= " << "basket_x + basket_r:  " << basket_x + basket_r << std::endl;
    //std::cout << "ball_x - ball_r:  " << ball_x - ball_r << " >= " << "basket_x - basket_r:  " << basket_x - basket_r << std::endl;
    //std::cout << "-------------" << std::endl;

    //replace with distance function
    if (yf + ball_r <= basket_y + 0.5 && 
        yf - ball_r >= basket_y - 0.5 && 
        ball_x + ball_r <= basket_x + basket_r && 
        ball_x - ball_r >= basket_x - basket_r) {
        return true;
    }
    else { return false; }
}

/* Loops through the pegs and if the ball has collided with a peg, 
isCollided becomes true and angle of collision is calculated (theta) */
bool GameController::pegCollision(const double dt) {
    for (const auto& peg : pegs) {
        float x_dist = ball_x - peg.front();
        float y_dist = yf - peg.back();
        float x_dist_sq = x_dist * x_dist;
        float y_dist_sq = y_dist * y_dist;
        float radiusSum = ball_r + peg_r;

        // checks whether the ball and peg collided
        if (x_dist_sq + y_dist_sq <= radiusSum * radiusSum) {
            std::cout << "yf: " << yf << std::endl
                << "peg.back(): " << peg.back() << std::endl
                << "ball_x: " << ball_x << std::endl
                << "peg.front(): " << peg.front() << std::endl
                << "x_dist: " << x_dist << std::endl
                << "y-dist: " << y_dist << std::endl <<std::endl;
            
            theta = atan2(y_dist, x_dist);
            vy = speed * sin(theta);
            vx = speed * cos(theta);
           
            // to deal with straight vertical
            if (theta == verticalAngle) {
                dx = 0.01 + vx * dt;
                //std::cout << "vertical angle" << std::endl;
                //std::cout << "vx: " << vx << std::endl;
                //std::cout << "dx: " << dx << std::endl;
            }
            else {
                dx = vx * dt;
            }
            
            //vy = -vy * 0.8;
            
            //dx = 0.01 + speed * cos(theta) * dt;

            //if (y_dist <= 0) {
            //    dx = speed * cos(theta) * dt;
            //}
            //else {
            //    dx = 0.01 + speed * cos(theta) * dt;
            //
            //}
          

            //// to deal with straight vertical
            //if (theta == verticalAngle) {
            //    dx = 0.01 + speed * cos(theta) * dt;
            //    std::cout << "vertical angle" << std::endl;

            //}
            //// to deal with sticking problem
            //else if (abs(dx) < 0.001) {
            //    std::cout << "dealing with the sticking" << std::endl;
            //    dx = 2 * speed * cos(theta) * dt;

            ////    std::cout << "speed * cos(theta) * dt: " << speed * cos(theta) * dt << std::endl;
            ////    std::cout << "dx: " << dx << std::endl;

            //}
            //else {
            //    dx = speed * cos(theta) * dt;
            //}

            std::cout << "this is theta: " << theta << std::endl;
            std::cout << "vy: " << vy << std::endl;
            std::cout << "cos(theta): " << cos(theta) << std::endl;
            std::cout << "vx: " << vx << std::endl;
            std::cout << "dx: " << dx << std::endl;
            std::cout << std::endl << std::endl;

            return true;
        }
    }
    return false;
}


bool GameController::wallCollision(const double dt) {
    float x_dist = ball_x - 10;
    float y_dist = yf - 10;

    if (ball_x <= -10 + ball_r || ball_x >= 10 - ball_r) {
        //speed = -speed * 0.8;
        dx = -dx;
    }

    return true;

    //if (ball_x <= -10 + ball_r) {
    //    speed = -speed * 0.8;
    //    dx = .05;
    //    return true;
    //}
    //else if (ball_x >= 10 - ball_r) {
    //    speed = -speed * 0.8;
    //    dx = -.05;
    //    return true;
    //}

    //if (ball_x <= -10 + ball_r || ball_x >= 10 - ball_r) {
    //    speed = -speed * 0.8;
    //    dx = 1.2;
    //    if (y_dist <= 0) {
    //        dx = 1.2 * speed * cos(theta) * dt;
    //    }
    //    else {
    //        dx = 0.01 + speed * cos(theta) * dt;
    //    }
    //}

}

// when ball is dropped, translate ball down by increasing
// amounts according to acceleration of gravity
bool GameController::ballDrop(const double dt) {
    Station* station = dynamic_cast<Station*>(container()); //what is a component again? difference with pair
    GameDisplay* display = station->getDisplay();
    auto* page = display->subpage();
    
    if (page->findByName("ball") != nullptr) {
        mixr::graphics::Circle* ball = dynamic_cast<mixr::graphics::Circle*>(page->findByName("ball")->object());

        //yf = yi + vy * dt + 0.5 * -g * pow(dt, 2);
        //dy = yf - yi;
        //// dy = vy * t
        //speed = sqrt(vy * vy + vx * vx);
        //
        //ball->lcTranslate(dx, dy, 0);
        //
        //ball_x += dx;
        //yi = yf;
        //vy -= (g * dt);
         
        dy = vy * dt + 0.5 * -g * pow(dt, 2);
        speed = sqrt(vy * vy + vx * vx);
        
        ball->lcTranslate(dx, dy, 0);
        
        ball_x += dx;
        yf += dy;
        vy -= (g * dt);

    } 
    return true;
}

void GameController::moveBasket(const double dt) {
    Station* station = dynamic_cast<Station*>(container());
    GameDisplay* display = station->getDisplay();
    auto* page = display->subpage();
    if (page->findByName("basket") != nullptr) {
        mixr::graphics::Polygon* basket = dynamic_cast<mixr::graphics::Polygon*>(page->findByName("basket")->object());      
        if (basket_d >= 0 && basket_d < 9) { 
            basket->lcTranslate(basketSpeed * dt, 0, 0);
            basket_d = basketSpeed * dt + basket_d;
            basket_x = basketSpeed * dt + basket_x;
        }
        else if (basket_d >= 9 && basket_d < 27) {
            basket->lcTranslate(-basketSpeed * dt, 0, 0);
            basket_d = basketSpeed * dt + basket_d;
            basket_x =  basket_x - basketSpeed * dt;
        }
        else if (basket_d >= 27 && basket_d < 36) {
            basket->lcTranslate(basketSpeed * dt, 0, 0);
            basket_d = basketSpeed * dt + basket_d;
            basket_x = basketSpeed * dt + basket_x;

        }
        else {
            basket_d = 0;
            basket_x = 0;

        }
        //std::cout << "this should be the position of the basket: " << basket_x << std::endl;
    }
}

bool GameController::ballLeft() {
    ball_x -= 1;
    return true;
}

bool GameController::ballRight() {
    ball_x += 1;
    return true;
}

//bool GameController::addBall() {
//    std::cout << "adding a ball from controller";
//    return true;
//}

void GameController::addPeg(float x, float y, int count) {
    for (int i = 0; i < count; ++i) {
        std::vector<float> peg = { x, y };
        pegs.push_back(peg);
        x += 3;
    }
}

void GameController::pegCoords() {
    addPeg(-7.5, 4.5, 6);
    addPeg(-7.5, 0.5, 6);
    addPeg(-7.5, -3.5, 6);
    addPeg(-6, 2.5, 5);
    addPeg(-6, -1.5, 5);
}


//void GameController::pegCoords() {
//    float x = -7.5;
//    float y = 4.5;
//    
//    for (int i = 0; i < 6; ++i) {
//        std::vector<float> peg = { x, y };
//        pegs.push_back(peg);
//        x = x + 3;
//    }
//
//    x = -7.5; 
//    y = 0.5;
//    for (int i = 0; i < 6; ++i) {
//        std::vector<float> peg = { x, y };
//        pegs.push_back(peg);
//        x = x + 3;
//    }
//
//    x = -7.5;
//    y = -3.5;
//    for (int i = 0; i < 6; ++i) {
//        std::vector<float> peg = { x, y };
//        pegs.push_back(peg);
//        x = x + 3;
//    }
//
//    x = -6;
//    y = 2.5;
//    for (int j = 0; j < 5; ++j) {
//        std::vector<float> peg = { x, y };
//        pegs.push_back(peg);
//        x = x + 3;
//    }
//
//    x = -6;
//    y = -1.5;
//    for (int j = 0; j < 5; ++j) {
//        std::vector<float> peg = { x, y };
//        pegs.push_back(peg);
//        x = x + 3;
//    }
//}


////1st
//[7.5 4.5]
//[4.5 4.5]
//[1.5 4.5]
//[-1.5 4.5]
//[-4.5 4.5]
//[-7.5 4.5]
//
////2nd
//[6 2.5]
//[3 2.5]
//[0 2.5]
//[-3 2.5]
//[-6 2.5]
//
////3rd
//[7.5 0.5]
//[4.5 0.5]
//[1.5 0.5]
//[-1.5 0.5]
//[-4.5 0.5]
//[-7.5 0.5]
//
////4th
//[6 - 1.5]
//[3 -1.5]
//[0 - 1.5]
//[-3 - 1.5]
//[-6 -1.5]
//
////5th
//[7.5 -3.5]
//[4.5 -3.5]
//[1.5 -3.5]
//[-1.5 -3.5]
//[-4.5 -3.5]
//[-7.5 -3.5]
//// difference in y is 2, difference in x is 3

