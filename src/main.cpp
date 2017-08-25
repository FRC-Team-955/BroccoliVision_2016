//TODO: Add masking
#define HEADLESS false
#define DETAILS true

#include <vector>
#include <RealSense.hpp>
#include <Sliders.hpp>
#include <cmath>
#include <stdio.h>
#include <string>
#include <Histogram.hpp>
#include <opencv2/opencv.hpp>
#include <Median.hpp>
#include <iostream>
#include <LoadedVideo.hpp>
#include <stdlib.h>

#include <librealsense/rs.hpp>

//TODO: Add sliders/config entries for these!!
#define HISTOGRAM_PERCENTILE 40

#define DEPTH_LOWER_BOUND 10
#define DEPTH_UPPER_BOUND 8000

#define DEFAULT_OUTPUT 1300.00

#define REALSENSE_CONV_LINE_M 0.123558
#define REALSENSE_CONV_LINE_B -7.16639

using namespace cv;

Point addPoints (Point a, Point b)
{
	Point out = a;
	out.x += b.x;
	out.y += b.y;
	return out;
}

int main (int argc, char** argv)
{
	//TODO: Decide on the interface based on the number of args, or take the camera serial number as an arg
	if (argc < 4) {
		std::cerr << "Usage: " << argv[0] << " <Slider save dir> <video/realsense> <Video file dir/Serial ID>" << std::endl;
		return -1;
	}

	Sliders* interface = new Sliders (!HEADLESS, argv[1]);

#if !HEADLESS
	cv::waitKey (30);
#endif

	VideoInterface *sensor;
	if (!strcmp(argv[2], "video")) {
		sensor = new LoadedVideo(argv[3]);
	} else {
		sensor = new Realsense(
			640,				//depth_width,
			480,				//depth_height,
			30,				//depth_framerate,
			1920,				//bgr_width,
			1080,				//bgr_height,
			30,				//bgr_framerate,
			argv[3]
			//"2391016026"	//serial
			);

	}

	system("stty -F /dev/ttyACM0 cs8 115200 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts");
   FILE* fptr = fopen("/dev/ttyACM0", "w");

	// Set up contour info
	std::vector<std::vector<Point> > contours;
	std::vector<Vec4i> hierarchy;

	// Set up the image vars
	Mat hsv_threshold, kernel_out, kernel_filtered_final, contour_out, canny_out;
	std::vector <Mat> split_colors (3);

	Mat Grey, Kinect_RGB_Copy;

	// Kernel to detect Broccoli pebbling
	Mat kernel = Mat::ones (3, 3, CV_32F);
	kernel.at<float> (1, 1) = -8.0f;


	//Histogram settings
	Histogram <unsigned short> *hist = new Histogram <unsigned short> (DEPTH_LOWER_BOUND, DEPTH_UPPER_BOUND);

	//Median Filter
	Median <unsigned short> *median_filter = new Median <unsigned short> (10, DEFAULT_OUTPUT);

	sensor->GrabFrames(false);

	bool skip = false;

#if HEADLESS
	Mat open_element =
		getStructuringElement (0, Size (2 * interface->open_slider + 1, 2 * interface->open_slider + 1),
				Point (interface->open_slider, interface->open_slider));
	Mat close_element =
		getStructuringElement (0, Size (2 * interface->close_slider + 1, 2 * interface->close_slider + 1),
				Point (interface->close_slider, interface->close_slider));
#endif

	while (true) {

		// Tell the *sensor to supply frames
		sensor->GrabFrames(skip);

		// HSV thresholding on the RGB image, to detect only broccoli
		cvtColor (*sensor->bgrmatCV, hsv_threshold, COLOR_BGR2HSV); // Convert to HSV colorspace

		inRange (hsv_threshold,
				Scalar (interface->hue_slider_lower, interface->sat_slider_lower, interface->val_slider_lower),
				Scalar (interface->hue_slider_upper, interface->sat_slider_upper, interface->val_slider_upper),
				hsv_threshold);

#if DETAILS && !HEADLESS
		imshow ("HSV", hsv_threshold);
#endif
		// Split the bgr into it's component channels
		split (*sensor->bgrmatCV, split_colors);

		// Kernel convolute on just the green channel
		filter2D (split_colors[1], kernel_out, -1, kernel, Point (-1, -1), 0, BORDER_DEFAULT);

		// Filter out negative results on the kernel output
		threshold (kernel_out, kernel_out, interface->thresh_slider, 255, THRESH_BINARY); // Threshhold the Kernel image

#if DETAILS && !HEADLESS
		imshow ("Kernel", kernel_out);
#endif

		// Clear the kernel afterimage of the last rerun
		kernel_filtered_final.setTo (Scalar (0, 0, 0, 0));

		// Filter the kenel image to remove things that are not within the HSV threshold
		kernel_out.copyTo (kernel_filtered_final, hsv_threshold); //, hsv_threshold);

		// Invert the image to ready it for morphologicals
		bitwise_not (kernel_filtered_final, kernel_filtered_final);

		//TODO: Use a function pointer to determine if this should update by slider or not
#if !HEADLESS //Live update
		Mat open_element =
			getStructuringElement (0, Size (2 * interface->open_slider + 1, 2 * interface->open_slider + 1),
					Point (interface->open_slider, interface->open_slider));
		Mat close_element =
			getStructuringElement (0, Size (2 * interface->close_slider + 1, 2 * interface->close_slider + 1),
					Point (interface->close_slider, interface->close_slider));
#endif

		// Filter out small eddies and areas of detection
		morphologyEx (kernel_filtered_final, kernel_filtered_final, MORPH_OPEN, open_element);
		morphologyEx (kernel_filtered_final, kernel_filtered_final, MORPH_CLOSE, close_element);

		// Make some borders for the image as to make the contour detector not fail to get objects touching it
		line (
				kernel_filtered_final,
				Point (0, 0),
				Point (kernel_filtered_final.size().width, 0),
				Scalar (255, 255, 255),
				5
			  );
		line (
				kernel_filtered_final,
				Point (0, 0),
				Point (0, kernel_filtered_final.size().height),
				Scalar (255, 255, 255),
				5
			  );
		line (
				kernel_filtered_final,
				Point (kernel_filtered_final.size().width, kernel_filtered_final.size().height),
				Point (kernel_filtered_final.size().width, 0),
				Scalar (255, 255, 255),
				5
			  );
		line (
				kernel_filtered_final,
				Point (kernel_filtered_final.size().width, kernel_filtered_final.size().height), Point (0, kernel_filtered_final.size().height),
				Scalar (255, 255, 255),
				5
			  );


#if DETAILS && !HEADLESS
		imshow ("Pre-Canny", kernel_filtered_final);
#endif

		// Canny edge detector on the image
		Canny (kernel_filtered_final, canny_out, interface->canny_slider, interface->canny_slider * 2, 3);

		// Find the edges of the broccoli
		findContours (canny_out, contours, hierarchy, RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE,
				Point (0, 0)); // RETR_CCOMP = Fill holes, KEEP THIS!!

		//Fix after morph TODO: Fix all of the steps leading up to this so you don't have to at all
		bitwise_not (kernel_filtered_final, kernel_filtered_final);

		//Copy the image to a display buffer
#if !HEADLESS
		(*sensor->bgrmatCV).copyTo (contour_out);
#endif

		int broccoli_count = 0;

		for (unsigned int i = 0; i < contours.size(); i++) { //For each blob...

			//Get the center of the blob
			Moments moment_set = moments (contours[i], false);
			Point2f center = Point2f (moment_set.m10 / moment_set.m00, moment_set.m01 / moment_set.m00);

			//Check if the blob's center is within the range, and that it is large enough
			if (contourArea (contours[i]) > interface->area_slider && interface->broc_roi.contains (center)) {

				//Get the bounding box for this blob
				Rect bound = cv::boundingRect (contours[i]);

#if !HEADLESS
				//Draw the blob's bounding box
				rectangle (contour_out, bound.tl(), bound.br(), Scalar (255, 255, 0), 2);
#endif
				//Cut out the blob from the corresponding area of the depth image
				Mat depth_cutout = (*sensor->largeDepthCV) (bound);

				//Mask the depth image by the blob's area (Only look at the actual broccoli!
				Mat depth_masked;
				depth_cutout.copyTo(depth_masked, kernel_filtered_final(bound)); //Mask!
				//imshow("mask", depth_masked * 4);

				//Run the histogram and only capture the percentile
				hist->insert_histogram_data (&depth_masked);
				int value = hist->take_percentile (HISTOGRAM_PERCENTILE);


				if (value > DEPTH_LOWER_BOUND && value < DEPTH_UPPER_BOUND) {
#if !HEADLESS
					//Draw detections
					putText (contour_out,(hack::to_string (REALSENSE_CONV_LINE_B + ((float)value * REALSENSE_CONV_LINE_M))+"mm").c_str(), addPoints (center, Point (-50, -150)),
							FONT_HERSHEY_COMPLEX_SMALL, 5.0, Scalar (0, 255, 255),10);
					drawContours (contour_out, contours, i, Scalar (0, 0, 255), 2, 8, hierarchy, 0, Point());
#endif

					//Median filter the data
					median_filter->insert_median_data (value);
					broccoli_count++;
				} else {
					//median_filter->insert_median_data ((int) DEFAULT_OUTPUT);
				}

			}
		}

		//We didn't find anything. Just fall back on the default, but do it slowly in case we're just skipping a frame.
		if (contours.size() == 0 || broccoli_count == 0) {
			//median_filter->insert_median_data ((int) DEFAULT_OUTPUT);
			std::cerr << "No contours" << std::endl;
		}

#define REALSENSE_CONV_LINE_M 0.123558
#define REALSENSE_CONV_LINE_B -7.16639
		int final_value = REALSENSE_CONV_LINE_B + ((float)median_filter->compute_median() * REALSENSE_CONV_LINE_M);
		fprintf (fptr, "%d\n", final_value);
		std::cout << final_value << std::endl;

#if !HEADLESS
		//Nice display stuff
		rectangle (contour_out, interface->broc_roi, Scalar (0,128,255), 3, 8, 0);
		putText (contour_out, (hack::to_string (final_value)+"mm").c_str(), Point (0,70),
				FONT_HERSHEY_COMPLEX_SMALL, 5.0, Scalar (0, 255, 128), 10);
		imshow ("Contours", contour_out);

#if DETAILS
		imshow ("Depth", *sensor->largeDepthCV * 4);
#endif
		int key = cv::waitKey (1);
		if (key == 27) {
			return 0;
		} else if (key == ' ') {
			skip = !skip;
		}
#endif

	}

	return 0;
}
