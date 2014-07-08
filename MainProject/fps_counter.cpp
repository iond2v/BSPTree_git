#include "fps_counter.h"

/*
reevaluates current_time to last_time difference and computes avg_render_time and fps accordingly.
INCREMENTS FRAME COUNT - call it only once per frame
*/
void fps_counter::update(){

	frame++;
	frames_count++;

	if (current_time - last_time > update_interval) { 

		fps = frame / ((float)(current_time - last_time) / 1000000000.0f);   //extrapolate to whole second  //frame count / elapsed time converted from nanoseconds to seconds 
		render_time_avg = 1.0f / fps * 1000; 
		last_time = current_time;
		frame = 0;



		if(render_time_avg < 1000){  //prevent distortion of values at start.. 33ms =~ 30fps; 66 cca 15fps; 100ms - 10fps
			update_count++;

				avg_fps = avg_fps + fps;
				avg_render_time = avg_render_time + render_time_avg;
			
			if(control->stats){
				std::string msg = "FPS:        "+std::to_string(fps)+
				"\nAvg FPS: "+std::to_string(avg_fps/update_count)+
				"\nRender time:       "+std::to_string(render_time_avg)+"ms per frame"+
				"\nAvg render time: "+std::to_string(avg_render_time/update_count)+"ms\n"+
				"Frames rendered: "+std::to_string(frames_count);
				
				stats->addTextData(msg, control->windowHeight, 10, 20, 10);
				stats->uploadData();
			}
		}
	}

}
/*
again in nanoseconds
*/
void fps_counter::setUpdateInterval(unsigned int ns){

	update_interval = ns;
}

float fps_counter::getFrameCount(){
	return frames_count;
}

/*
Returns added render time divided by update count.
*/
float fps_counter::getAvgRenderTime(){

return avg_render_time / update_count;
}

/*
Returns added fps divided by update count.
*/
float fps_counter::getAvgFps(){

return avg_fps / update_count;
}