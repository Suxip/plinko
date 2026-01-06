#include "Station.hpp"
#include "mixr/base/Pair.hpp"

IMPLEMENT_SUBCLASS(Station, "Station")
EMPTY_COPYDATA(Station)
EMPTY_DELETEDATA(Station)
EMPTY_SLOTTABLE(Station)

BEGIN_EVENT_HANDLER(Station)
	//ON_EVENT(LEFT_ARROW_KEY, ballLeft)
	//ON_EVENT(RIGHT_ARROW_KEY, ballRight)
END_EVENT_HANDLER()

Station::Station() 
{
	STANDARD_CONSTRUCTOR()
}

GameDisplay* Station::getDisplay()
{
    return dynamic_cast<GameDisplay*>(findByName("mydisp")->object());
}
