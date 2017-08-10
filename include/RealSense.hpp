#ifndef REALSENSE_HPP
#define REALSENSE_HPP

#include <librealsense/rs.hpp>
#include <opencv2/opencv.hpp>
#include <signal.h>
#include <StringHack.hpp>
#include <cstring>
#include <VideoInterface.hpp>

using namespace cv;

class Realsense : public VideoInterface
{
	private:
		// Realsense stuffs
		rs::context ctx;
		rs::device* dev;
	public:
		void GrabFrames (bool skip);

		Realsense(int depth_width, int depth_height, int depth_framerate, int bgr_width, int bgr_height, int bgr_framerate, char* serial);

		void SetColorExposure (int exposure);
		
		void SetDepthExposure (int exposure);

		bool GetDeviceBySerial (char* serial);

		float GetTimeStamp();

		//cv::Mat* leftIRCV;
		//cv::Mat* rightIRCV;

		// Free up memory/stop processes
		~Realsense();
};

#endif
