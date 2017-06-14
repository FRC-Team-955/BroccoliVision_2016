#ifndef VIDEOINTERFACE_HPP
#define VIDEOINTERFACE_HPP
#include <librealsense/rs.hpp>
#include <opencv2/opencv.hpp>
#include <signal.h>

using namespace cv;

class VideoInterface
{
	protected:
	public:
		Mat *bgrmatCV;
		Mat *rgbmatCV; 
		Mat *depthmatCV; 
		Mat *registeredCV;
		Mat *largeDepthCV;

		virtual void GrabFrames () = 0;

		virtual float GetTimeStamp () = 0;
};
#endif
