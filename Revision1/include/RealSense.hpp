#ifndef REALSENSE_HPP
#define REALSENSE_HPP

#include <librealsense/rs.hpp>
#include <opencv2/opencv.hpp>
#include <signal.h>
#include <stringhack.hpp>

using namespace std;
using namespace cv;

class Realsense
{
	private:
	public:
		// Opencv matrices
		Mat *bgrmatCV;
		Mat *rgbmatCV; 
		Mat *depthmatCV; 
		Mat *registeredCV;
		Mat *largeDepthCV;
	
		//@60FPS, depth at 320x240, color can be 640x480
		//@60FPS, depth at 480x360, color can be 320x240 or 640x480
		//@30FPS, depth at 320x240, color can be 640x480, 1280x720 or 1920x1080
		//@30FPS, depth at 480x360, color can be 320x240, 640x480, 1280x720, or 1920x1080

		// Realsense stuffs
		rs::context ctx;
		rs::device* dev;

		void GrabFrames () {
			if (ctx.get_device_count() > 0)	{
				//try {	
					dev->wait_for_frames();
				//} catch (int e) {
				//	cout << "[ ERROR ] Realsense wait for frams timed out. Error code: " + hack::to_string(e) << endl;
				//}

				if (!dev->is_streaming()) {
					cout << "[ ERROR ] Streaming stopped" << endl;
				}

				depthmatCV->data = (unsigned char*)dev->get_frame_data(rs::stream::depth);
				rgbmatCV->data = (unsigned char*)dev->get_frame_data(rs::stream::rectified_color);
				cvtColor(*rgbmatCV, *bgrmatCV, CV_RGB2BGR);
				largeDepthCV->data = (unsigned char*)dev->get_frame_data(rs::stream::depth_aligned_to_color);
			} else {
				cout << "[ ERROR ] No devices connected! " << endl;
			}
		}

		Realsense() {
			bgrmatCV = new Mat (1080, 1920, CV_8UC3);
			rgbmatCV = new Mat (1080, 1920, CV_8UC3);
			depthmatCV = new Mat (360, 480, CV_16UC1);
			registeredCV = new Mat (1080, 1920, CV_8UC3); 
			largeDepthCV = new Mat (1080, 1920, CV_16UC1); 

			/*bgrmatCV = new Mat (720, 1280, CV_8UC3);
			  rgbmatCV = new Mat (720, 1280, CV_8UC3);
			  depthmatCV = new Mat (480, 640, CV_16UC1);
			  registeredCV = new Mat (720, 1280, CV_8UC3);*/ 

			printf("There are %d connected RealSense devices.\n", ctx.get_device_count());

			if(ctx.get_device_count() == 0)
				exit(-1);

			dev = ctx.get_device(0);
			printf("\nUsing device 0, an %s\n", dev->get_name());
			printf("    Serial number: %s\n", dev->get_serial());
			printf("    Firmware version: %s\n", dev->get_firmware_version());

			dev->enable_stream(rs::stream::depth, 480, 360, rs::format::z16, 30);
			dev->enable_stream(rs::stream::color, 1920, 1080, rs::format::rgb8, 30);
			//dev->enable_stream(rs::stream::depth, 640, 480, rs::format::z16, 60);
			//dev->enable_stream(rs::stream::color, 1280, 720, rs::format::rgb8, 60);

			dev->start();

		}

		// Free up memory/stop processes
		~Realsense() {
		}
};

#endif // KINECT_HPP
