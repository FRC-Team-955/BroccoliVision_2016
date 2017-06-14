#include "RealSense.hpp"

void Realsense::GrabFrames () {
	if (ctx.get_device_count() > 0)	{
		dev->wait_for_frames();

		if (!dev->is_streaming()) {
			std::cerr << "[ ERROR ] Streaming stopped" << std::endl;
		}

		depthmatCV->data = (unsigned char*)dev->get_frame_data(rs::stream::depth);
		rgbmatCV->data = (unsigned char*)dev->get_frame_data(rs::stream::rectified_color);
		cvtColor(*rgbmatCV, *bgrmatCV, CV_RGB2BGR);
		largeDepthCV->data = (unsigned char*)dev->get_frame_data(rs::stream::depth_aligned_to_color);
		//leftIRCV->data = (unsigned char*)dev->get_frame_data(rs::stream::infrared);
		//rightIRCV->data = (unsigned char*)dev->get_frame_data(rs::stream::infrared2);
	} else {
		std::cerr << "[ ERROR ] No devices connected! " << std::endl;
	}
}

Realsense::Realsense(int depth_width, int depth_height, int depth_framerate, int bgr_width, int bgr_height, int bgr_framerate, char* serial) {
	bgrmatCV = new Mat (bgr_height, bgr_width, CV_8UC3);
	rgbmatCV = new Mat (bgr_height, bgr_width, CV_8UC3);
	registeredCV = new Mat (bgr_height, bgr_width, CV_8UC3); 
	largeDepthCV = new Mat (bgr_height, bgr_width, CV_16UC1); 
	depthmatCV = new Mat (depth_height, depth_width, CV_16UC1);

	//leftIRCV = new Mat (depth_height, depth_width, CV_8UC1);
	//rightIRCV = new Mat (depth_height, depth_width, CV_8UC1);

	if(!GetDeviceBySerial(serial)) {
		std::cerr << "Camera with serial number " << serial << " not found. Double check?" << std::endl;
		exit(-1);
	}
	printf("\nUsing device 0, an %s\n", dev->get_name());
	printf("    Serial number: %s\n", dev->get_serial());
	printf("    Firmware version: %s\n", dev->get_firmware_version());

	//@60FPS, depth at 320x240, color can be 640x480
	//@60FPS, depth at 480x360, color can be 320x240 or 640x480
	//@30FPS, depth at 320x240, color can be 640x480, 1280x720 or 1920x1080
	//@30FPS, depth at 480x360, color can be 320x240, 640x480, 1280x720, or 1920x1080
	dev->enable_stream(rs::stream::depth, depth_width, depth_height, rs::format::z16, depth_framerate);
	dev->enable_stream(rs::stream::color, bgr_width, bgr_height, rs::format::rgb8, bgr_framerate);

	//dev->enable_stream(rs::stream::infrared, depth_width, depth_height, rs::format::y8, depth_framerate);
	//dev->enable_stream(rs::stream::infrared2, depth_width, depth_height, rs::format::y8, depth_framerate);

	dev->start();

}

void Realsense::SetDepthExposure(int exposure) {
	dev->set_option(rs::option::r200_lr_exposure, exposure);
}


void Realsense::SetColorExposure(int exposure) {
	dev->set_option(rs::option::color_enable_auto_exposure, 0);
	dev->set_option(rs::option::color_exposure, exposure);
}

float Realsense::GetTimeStamp() {
	return dev->get_frame_timestamp(rs::stream::color);
}

Realsense::~Realsense() {
	delete[] bgrmatCV;	 
	delete[]	rgbmatCV;	 
	delete[]	registeredCV;	 
	delete[]	largeDepthCV;	 
	delete[]	depthmatCV;
}

bool Realsense::GetDeviceBySerial(char* serial) {
	for (int dev_idx = 0; dev_idx < ctx.get_device_count(); dev_idx++) {
		rs::device* current_dev = ctx.get_device(dev_idx);
		std::cout << "Found device serial: " << current_dev->get_serial() << std::endl;
		if (!strcmp(current_dev->get_serial(), serial)) {
			dev = current_dev; 
			return true;
		}
	}
	return false;
}
