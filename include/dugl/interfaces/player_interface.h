#pragma once

#include "perspective_interface.h"
#include "window_interface.h"


// Abstract interface for a mouse-controllable Camera. Mouse inputs are 
// accessed through the inherited WindowInterface methods, while the Camera 
// object is accessed through the PerspectiveInterface.
class PlayerInterface : public WindowInterface, public PerspectiveInterface {};