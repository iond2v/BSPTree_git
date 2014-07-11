#pragma once

#include "Font.h"


/*
Added values are updated only after render time stays below 33ms. -> avoid initial spike.
*/
struct  fps_counter {
public:
	//default value equals 300ms
	//all times and interval are in nanoseconds
	fps_counter(GLuint update_interval = 300000000) :
		current_time(0), last_update_time(0), last_frame_time(0), frame(0), fps(0), render_time_avg(0), update_interval(update_interval),
		avg_fps(0), avg_render_time(0), update_count(0), frames_count(0) {
	
			control = Control::getInstance();
			stats = std::unique_ptr<dynamicText> (new dynamicText(control->font.get()));
			frame_time_slots.resize(8);
	}


	GLuint64 current_time;	//to be updated by query	


	void update();
	float getAvgRenderTime();
	float getAvgFps();
	float getFrameCount();
	void setUpdateInterval(unsigned int ns);

	Control *control;
	std::unique_ptr<dynamicText> stats;

	/* time slots - can be interpreted as fps targets..
	lower  - greater than 100ms
	10fps  - smaller than 100ms
	15fps  - smaller than 66.6ms
	20fps  - smaller than 50ms
	30fps  - smaller than 33.3ms
	60fps  - smaller than 16.7ms
	120fps - smaller than 8.3ms
	240fps - smaller than 4.16ms
	*/
	std::vector<GLuint> frame_time_slots;     //idea is to measure how many frames fit into each time slot.
private:
	GLuint64 last_update_time;
	GLuint64 last_frame_time;
	GLuint update_interval;


	unsigned int frame;   //frame since last update
	float fps;				//current fps value
	float render_time_avg;	//current render time average per frame

	//aplication averages
	unsigned int frames_count;	//summed frame count
	unsigned int update_count;	//update count 
	float avg_fps;				//summed fps values
	float avg_render_time;		//summed render times
	void put_frame_into_time_slot(GLuint time);
};

