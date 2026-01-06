#include "mixr/base/Pair.hpp"
#include "mixr/base/Timers.hpp"
#include "mixr/base/edl_parser.hpp"
#include "mixr/base/Component.hpp"

#include "mixr/ui/glut/GlutDisplay.hpp"
#include <GL/glut.h>
#include "mixr/base/util/system_utils.hpp"

// factories
#include "mixr/graphics/factory.hpp"
#include "mixr/base/factory.hpp"
#include "mixr/ui/glut/factory.hpp"
#include "mixr/instruments/factory.hpp"

#include "GameDisplay.hpp"
#include "Station.hpp"
#include "GameController.hpp"
#include "Ball.hpp"
#include "TestGameController.hpp"
#include "TestGameController2.hpp"

#include "mixr/instruments/buttons/Button.hpp"
#include "mixr/simulation/factory.hpp"
#include "mixr/models/factory.hpp"
#include "mixr/terrain/factory.hpp"
#include "mixr/interop/dis/factory.hpp"
#include "mixr/ighost/cigi/factory.hpp"
#include "mixr/ighost/pov/factory.hpp"

//initialize a global frameRate variable that will be used throughout other functions in main 
const int frameRate{ 30 };
Station* station{};

// timer function, in this case, the background (updateData) function
void timerFunc(int)
{
    const double dt0{ 1.0 / static_cast<double>(frameRate) };
    const int millis{ static_cast<int>(dt0 * 1000) };
    glutTimerFunc(millis, timerFunc, 1);

    // Current time
    const double time{ mixr::base::getComputerTime() };

    // N-1 Time
    static double time0{ time };

    // Compute delta time
    const double dt{ static_cast<double>(time - time0) };

    time0 = time;

    int oldCount = station->getDisplay()->getCount();
    if (oldCount < 4) { 
        int newCount = station->getDisplay()->getCount() + 1;
        station->getDisplay()->setCount(newCount);
    }
    else {
        station->getDisplay()->setCount(1); // not sure if this if else is the best logic
        station->getDisplay()->setLoaded(true);
    }

    mixr::base::Timer::updateTimers(dt);
    mixr::graphics::Graphic::flashTimer(dt);
    station->updateData(dt);
}

mixr::base::Object* factory(const std::string& name)
{
    mixr::base::Object* obj{};

    // we need to add our object to the factory so mixr can identify it in the epp
    if (name == Station::getFactoryName()) {
        obj = new Station;
    }
    else if (name == GameDisplay::getFactoryName()) {
        obj = new GameDisplay;
    }
    else if (name == mixr::instruments::Button::getFactoryName()) {
        obj = new mixr::instruments::Button;
    }
    else if (name == GameController::getFactoryName()) {
        obj = new GameController;
    }
    else if (name == Ball::getFactoryName()) {
        obj = new Ball;
    }
    else if (name == TestGameController::getFactoryName()) { // changed
        obj = new TestGameController;
    } 
    else if (name == TestGameController2::getFactoryName()) { // changed
        obj = new TestGameController2;
    }

    // platform libraries
    if (obj == nullptr) { obj = mixr::cigi::factory(name); }
    if (obj == nullptr) { obj = mixr::pov::factory(name); }
    if (obj == nullptr) { obj = mixr::instruments::factory(name); }
    if (obj == nullptr) { obj = mixr::simulation::factory(name); }
    if (obj == nullptr) { obj = mixr::models::factory(name); }
    if (obj == nullptr) { obj = mixr::terrain::factory(name); }
    if (obj == nullptr) { obj = mixr::dis::factory(name); }
    if (obj == nullptr) { obj = mixr::graphics::factory(name); }
    if (obj == nullptr) { obj = mixr::glut::factory(name); }
    if (obj == nullptr) { obj = mixr::base::factory(name); }

    return obj;
}

// station builder
Station* builder(const std::string& filename)
{
    // read configuration file
    int num_errors{};
    mixr::base::Object* obj{ mixr::base::edl_parser(filename, factory, &num_errors) };
    if (num_errors > 0) {
        std::cerr << "File: " << filename << ", number of errors: " << num_errors << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // test to see if an object was created
    if (obj == nullptr) {
        std::cerr << "Invalid configuration file, no objects defined!" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // do we have a base::Pair, if so, point to object in Pair, not Pair itself
    const auto pair = dynamic_cast<mixr::base::Pair*>(obj);
    if (pair != nullptr) {
        obj = pair->object();
        obj->ref();
        pair->unref();
    }

    // try to cast to proper object, and check
    const auto station = dynamic_cast<Station*>(obj);
    if (station == nullptr) {
        std::cerr << "Invalid configuration file!" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    return station;
}

//
int main(int argc, char* argv[])
{
    glutInit(&argc, argv);

    std::system("make-edl.cmd");

    // default configuration filename
    std::string configFilename{ "test0.edl" };

    // build a station
    station = builder(configFilename);

    // reset the Simulation
    station->event(mixr::base::Component::RESET_EVENT);

    // set timer for the background tasks
    const double dt{ 1.0 / static_cast<double>(frameRate) };
    const int millis{ static_cast<int>(dt * 1000) };

    // ensure everything is reset
    station->updateData(dt);
    station->updateTC(dt);
    station->event(mixr::base::Component::RESET_EVENT);

    station->getDisplay()->createWindow();

    glutTimerFunc(millis, timerFunc, 1);

    // create the Time Critical Thread (updateTC())
    station->createTimeCriticalProcess();

    glutMainLoop();
    return 0;
}
