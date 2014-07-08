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
		current_time(0), last_time(0), frame(0), fps(0), render_time_avg(0), update_interval(update_interval),
		avg_fps(0), avg_render_time(0), update_count(0), frames_count(0) {
	
			control = Control::getInstance();
			stats = new dynamicText(control->font.get());
	}


	GLuint64 current_time;	//to be updated by query	


	void update();
	float getAvgRenderTime();
	float getAvgFps();
	float getFrameCount();
	void setUpdateInterval(unsigned int ns);

	Control *control;
	dynamicText *stats;

private:
	GLuint64 last_time;		
	GLuint update_interval;


	unsigned int frame;   //frame since last update
	float fps;				//current fps value
	float render_time_avg;	//current render time average per frame

	//aplication averages
	unsigned int frames_count;	//added frame count
	unsigned int update_count;	//update count 
	float avg_fps;				//added fps values
	float avg_render_time;		//added render times

};

