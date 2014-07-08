#include "Camera.h"
#include <stdio.h>





Camera::Camera(float speed, float mouse_speed, float frustum_near, float frustum_far){
		

		this->speed = abs(speed);
		this->mouse_speed = abs(mouse_speed);
		fzNear = frustum_near;
		fzFar = frustum_far;

		angleSIDE = 0.0f;  //start with zero angles
		angleUP = 0.0f;

		height = 0.0f;    //and on the ground level

		position = vec3(0.0f, height, 0.5f);

		

	updateTarget();

	std::cout << "Camera constructed\n";
}

Camera::~Camera(){

	std::cout << "Camera destructed\n";

}

/*
Hooks file to camera for recording coordinates (+ creates it if it not exists) 
and if it is not empty loads waypoints.
Enables having saved multiple paths.
*/
void Camera::loadWaypoints(std::string name){


	cameraWaypoints = std::unique_ptr<Log>(new Log(name));


		std::ifstream waypoints;
		waypoints.open(name, std::ios::in);

		if(waypoints.is_open()){



			//////////load first line with time


			std::string line;
			std::getline(waypoints, line);
	
			while(not line.empty()){
				

				std::istringstream iss(line);
				std::vector<float> vector;

				std::copy(std::istream_iterator<float>(iss),
					std::istream_iterator<float>(),
						std::back_inserter(vector));

				//populate splines
				movement.positions.addSplinePoint(vec3(vector[0], vector[1], vector[2]));
				
				movement.targets.addSplinePoint(vec3(vector[3], vector[4], vector[5]));

				movement.times.push_back(vector[6]);

				//load next line
				std::getline(waypoints, line);

			}

		}



}


/*
Defined in Camera.cpp
converts degrees to radians

const float fDegToRad = 3.14159f * 2.0f / 360.0f; =~ 0,0174532777777778
*/
float DegToRad(float fAngDeg) {
	return fAngDeg * 0.017453278f; ///probably slightly too big..
}

/*
Responsible for moving target around depending on position and angles of look.
*/
void Camera::updateTarget(){
	
	target.x = position.x + sinf(DegToRad(angleSIDE)) * 0.1f;
	target.z = position.z - cosf(DegToRad(angleSIDE)) * 0.1f;

	//allows look higher. range limited by clamping the angle up to +-89 in mouse callback
	target.y = position.y - (sinf(DegToRad(angleUP)) + tanf(DegToRad(angleUP))) * 0.1f; 
	
}

/*
Records camera movement into a file.
*/
void Camera::recordWaypoint(){

	std::string msg = std::to_string(position.x) + " " + std::to_string(position.y) + " " + std::to_string(position.z) + " ";
	msg.append(std::to_string(target.x) + " " + std::to_string(target.y) + " " + std::to_string(target.z) + " " +
		std::to_string(time_item().seconds(movement.start_time)) + "\n"); 

	cameraWaypoints->append(msg, false);

}


/*
Computes normalized distance from travel time to allow for nonuniform distances and times with constant travel speed.
Expects that all vectors in movement are of the same size..
Upon finishing it triggers end of main loop.
valueToInterpolate should be of the same type as values in movement.times (as in time or frame number)
*/
void Camera::loadNextPosition(float valueToInterpolate){

	

	//idea - adjustable speed for interpolation...

	//if next index is not out of bounds, adjust current index
	while(current_index + 1 != movement.times.size() && valueToInterpolate > movement.times[current_index + 1])	//could have skipped one or more nodes..
		current_index++;

	

	if(current_index + 1 != movement.times.size()){
	
		float segment_duration =  movement.times[current_index + 1] - movement.times[current_index];
		float percentage_of_current_segment_travelled = (valueToInterpolate - movement.times[current_index]) / segment_duration; ///0.5 = 45 / 90     - means im at half of current segment - use it to compute normalized distance for interpolation


		float delta = movement.positions.delta();   //is number between 0.0 and 1.0

		float normalized_distance = delta * current_index + delta * percentage_of_current_segment_travelled;
	

		position = movement.positions.getInterpolatedSplinePoint(normalized_distance);
		target = movement.targets.getInterpolatedSplinePoint(normalized_distance);
	} else {
		glutLeaveMainLoop();
	}

}

/*
Converts loaded times in movement.times to frame numbers
To enable deterministic camera position at set frame..
*/
void Camera::convertToFrames(unsigned int frame_count_at_end){

	if(movement.times.empty())
		return;

	float end_time = movement.times.back();

	for(unsigned int i = 0; i < movement.times.size(); i++){
	
		movement.times[i] = movement.times[i] / end_time * frame_count_at_end;
	
	}
	
}  

/*
Sets position and updates target.
*/
void Camera::setPosition(vec3 position){

	this->position = position;
	updateTarget();
}


/*
Prints camera position to console.
*/
void Camera::printPosition(){

	printf("X %4.2f Y %4.2f Z %4.2f\n", position.x, position.y, position.z );
}

/*
Sets camera position depending on step and calls updateTarget to set target for new position.
*/
void Camera::moveForward(float step){

step = abs(step / speed);	
vec3 direction(0.0f, 0.0f, 0.0f);

		direction.x = sinf(DegToRad(angleSIDE));
		direction.z = -cosf(DegToRad(angleSIDE));


	 //in direction of camera.target
	this->position.x += step * direction.x;
	this->position.z += step * direction.z;
	this->position.y = height;

updateTarget();
}

/*
Sets camera position depending on step and calls updateTarget to set target for new position.
*/
void Camera::moveBack(float step){

step = abs(step / speed);
vec3 direction(0.0f, 0.0f, 0.0f);

		direction.x = -sinf(DegToRad(angleSIDE));
		direction.z = +cosf(DegToRad(angleSIDE));	
	
	this->position.x += step * direction.x;
	this->position.z += step * direction.z; //in direction of camera.target
	this->position.y = height;

updateTarget();
}

/*
Sets camera position depending on step and calls updateTarget to set target for new position.
*/
void Camera::moveLeft(float step){

step = abs(step / speed);	
vec3 direction(0.0f, 0.0f, 0.0f);

		direction.x = -cosf(DegToRad(angleSIDE));
		direction.z = -sinf(DegToRad(angleSIDE));


	 //in direction of camera.target
	this->position.x += step * direction.x;
	this->position.z += step * direction.z;
	this->position.y = height;

updateTarget();
}

/*
Sets camera position depending on step and calls updateTarget to set target for new position.
*/
void Camera::moveRight(float step){

step = abs(step / speed);	
vec3 direction(0.0f, 0.0f, 0.0f);

		direction.x = cosf(DegToRad(angleSIDE));
		direction.z = sinf(DegToRad(angleSIDE));


	 //in direction of camera.target
	this->position.x += step * direction.x;
	this->position.z += step * direction.z;
	this->position.y = height;

updateTarget();
}

/*
Calculates LookAt matrix from position of camera, target and up vector
*/
mat4 Camera::CalcLookAtMatrix(const vec3 &cameraPt, const vec3 &lookPt, const vec3 &upPt){

	glm::vec3 lookDirection = glm::normalize(lookPt - cameraPt);
	glm::vec3 upDirection = glm::normalize(upPt);

	glm::vec3 rightDirection = glm::normalize(glm::cross(lookDirection, upDirection));
	glm::vec3 perpUpDirection = glm::cross(rightDirection, lookDirection);

	glm::mat4 rotMat(1.0f);
	rotMat[0] = glm::vec4(rightDirection, 0.0f);
	rotMat[1] = glm::vec4(perpUpDirection, 0.0f);
	rotMat[2] = glm::vec4(-lookDirection, 0.0f);

	rotMat = glm::transpose(rotMat);

	glm::mat4 transMat(1.0f);
	transMat[3] = glm::vec4(-cameraPt, 1.0f);

	return rotMat * transMat;
}

