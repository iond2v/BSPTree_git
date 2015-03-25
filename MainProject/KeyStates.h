#pragma once


#include <algorithm>  //std::fill
#include <iostream>	  //std::cout


#define not !

//attempt at singleton.
//holds info about pressed keys.
//can set key to be toggleable.
class keyStates {

public: 

	~keyStates(void){
		instance = nullptr;

		//std::cout << "keystates destructed" << std::endl;
	
	}


	bool isPressed(unsigned int value);
	bool isToggled(unsigned int value); 
	bool isToggleable(unsigned int value);
	void setToggleable(unsigned int key, bool value);
	void setToggle(unsigned int key, bool value); //handled by press
	void swapToggle(unsigned int key);

	void press(unsigned int value);
	void release(unsigned int value);




	//Returns intance. If it doesnt exist, creates one.
	//for thread safety add mutex. unnecessary here.
	static keyStates* getInstance(){ 

		if(instance == nullptr){ 
			instance = new keyStates();
		}

		return instance;
	}

private:

	static keyStates *instance;
	bool pressed[256];
	bool toggled[256];
	bool toggleable[256];



	keyStates(){
		std::cout << "keystates constructed" << std::endl;

		std::fill(pressed, pressed + 256 * sizeof(bool), false);
		std::fill(toggled, toggled + 256 * sizeof(bool), false);
		std::fill(toggleable, toggleable + 256 * sizeof(bool), false);

	}

};
