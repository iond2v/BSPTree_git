#pragma once

#include <glm/glm.hpp>
#include <fstream>
#include "CatmullRom.h"
#include "time_item.h"
#include "Control.h"		//Log class
#include <GL/freeglut.h>
#include <memory>

class Log;


using namespace glm;

class Camera {
	
	public:

	Camera(float speed = 25.0f, float mouse_speed = 0.2f, float frustum_near = 0.1f, float frustum_far = 1000.0f);

    ~Camera();

	vec3 position, target;  
	float angleSIDE, angleUP;
	float speed, mouse_speed;
	float fzNear, fzFar;
	float height;					///why do I have here height?  --position.y should be the same..


	struct {
		CRSpline positions;
		CRSpline targets;
		std::vector<float> times;

		time_item start_time; //should be set when starting movement
		//float path_duration; //times.last()
	} movement;

	int current_index;		// index into movement


	static mat4 CalcLookAtMatrix(const vec3 &cameraPt, const vec3 &lookPt, const vec3 &upPt);
    
	void moveForward(float step = 1.0f);  //moves camera and updates target
	void moveBack(float step = 1.0f);
	void moveLeft(float step = 1.0f);
	void moveRight(float step = 1.0f);
	void setPosition(vec3 position);
	void printPosition();

	void updateTarget();  //only updates target of look based on angles
	std::unique_ptr<Log> cameraWaypoints; //used by recordWaypoint, initialized by loadWaypoints

	//is loaded...? to prevent trying to access not initialized pointer cameraWaypoints
	void loadWaypoints(std::string name);		//loads camera waypoints from file.     maybe rename to initRecording or something..
	void convertToFrames(unsigned int frame_count_at_end);   //converts loaded times in movement.times to frame numbers - makes every camera position deterministic at set frame..
	void recordWaypoint();
	void loadNextPosition(float valueToInterpolate);	//computes and sets camera position from movement
};



