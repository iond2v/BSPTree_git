#include "keyStates.h"

keyStates* keyStates::instance = nullptr;


//return true when toggled on or pressed.
bool keyStates::isPressed(unsigned int value){

return toggled[value] || pressed[value];
}



/*
Registers key press, if key is toggleable, sets key as toggled on/off.
*/
void keyStates::press(unsigned int value){

pressed[value] = true;

//if toggleable invert toggle value
if(isToggleable(value))
	setToggle(value, not isToggled(value));

}

void keyStates::release(unsigned int value){

pressed[value] = false;
}


void keyStates::setToggleable(unsigned int key, bool value){

toggleable[key] = value;

}

bool keyStates::isToggleable(unsigned int value){

return toggleable[value];
}

void keyStates::setToggle(unsigned int key, bool value){

toggled[key] = value;
}

void keyStates::swapToggle(unsigned int key){
	setToggle(key, not isToggled(key));
}

bool keyStates::isToggled(unsigned int value){

return toggled[value];
}