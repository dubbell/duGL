#ifndef PLAYER_INTERFACE
#define PLAYER_INTERFACE

#include "perspective_interface.h"
#include "window_interface.h"


class PlayerInterface : public WindowInterface, public PerspectiveInterface {};

#endif