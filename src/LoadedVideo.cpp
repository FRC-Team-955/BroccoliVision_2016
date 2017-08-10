#include "LoadedVideo.hpp"

void LoadedVideo::GrabFrames () {
	if (file_index < FRAMES) {
		file_in.read((char*)rgbmat.data, DATA_LENGTH_COLOR);	
		file_in.read((char*)largeDepthCV.data, DATA_LENGTH_DEPTH);	
		file_index++;
		//waitKey (1000/FRAMERATE);
	} else {
		exit(-1);
	}
	cvtColor(*rgbmatCV, *bgrmatCV, CV_RGB2BGR);
}

LoadedVideo::LoadedVideo(char* video_dir) {
	bgrmatCV      =  new  Mat  (FRAME_HEIGHT,  FRAME_WIDTH,  CV_8UC3);
	rgbmatCV      =  new  Mat  (FRAME_HEIGHT,  FRAME_WIDTH,  CV_8UC3);
	largeDepthCV  =  new  Mat  (FRAME_HEIGHT,  FRAME_WIDTH,  CV_16UC1);
	//depthmatCV  =  new  Mat  (FRAME_HEIGHT,  FRAME_WIDTH,  CV_16UC1);

	file_in = ifstream (argv[1], ios::binary);
}

LoadedVideo::~LoadedVideo() {
	delete[] bgrmatCV;	 
	delete[]	rgbmatCV;	 
	delete[]	largeDepthCV;	 
	delete[]	depthmatCV;
}
