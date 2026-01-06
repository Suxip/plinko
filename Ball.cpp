#include "Ball.hpp"
#include <cmath>
#include "mixr/base/numeric/Number.hpp"

IMPLEMENT_SUBCLASS(Ball, "Ball")
EMPTY_DELETEDATA(Ball)
EMPTY_SLOTTABLE(Ball)

BEGIN_EVENT_HANDLER(Ball)
//ON_EVENT(3008, resetGame)
END_EVENT_HANDLER()

Ball::Ball() {
    STANDARD_CONSTRUCTOR()
}

void Ball::copyData(const Ball& org, const bool) {
    BaseClass::copyData(org);
}

// should this be updateTC?
void Ball::updateData(const double dt) {
    BaseClass::updateData(dt);

    ////treating t = 0.2s
    //xf = 0.5 * g * pow(t, 2); // initial velocity is 0

    //dx = xf - xi;
    //lcTranslate(1, dx, 0);
    //t = t + 0.2;
    //xi = xf;
}