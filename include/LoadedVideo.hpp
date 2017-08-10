#ifndef LOADEDVIDEO_HPP
#define LOADEDVIDEO_HPP

#include <opencv2/opencv.hpp>
#include <StringHack.hpp>
#include <VideoInterface.hpp>

#include <iostream>
#include <fstream>

#define FRAME_WIDTH 1920
#define FRAME_HEIGHT 1080

#define DATA_LENGTH_COLOR FRAME_WIDTH * FRAME_HEIGHT * 3
#define DATA_LENGTH_DEPTH FRAME_WIDTH * FRAME_HEIGHT * 2
#define LENGTH_S 60
#define FRAMERATE 30
#define FRAMES LENGTH_S * FRAMERATE

using namespace cv;

class LoadedVideo : public VideoInterface
{
	private:
		std::ifstream file_in; 
		int file_index = 0;
	public:
		void GrabFrames (bool skip) ;

		LoadedVideo(char* video_dir);

		float GetTimeStamp();

		// Free up memory/stop processes
		~LoadedVideo();
};

#endif
