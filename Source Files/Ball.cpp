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

void Ball::updateData(const double dt) {
    BaseClass::updateData(dt);
}
