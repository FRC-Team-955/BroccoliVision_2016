//OH GOD THIS CODE NEEDS REFACTORING FOR THE
#define HEADLESS true
#define DETAILS false

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

#include <librealsense/rs.hpp>

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
	//VideoWriter rgbcap;
	//VideoWriter depthcap;

	if (argc < 3) {
		std::cerr << "Usage: " << argv[0] << " <Slider save dir> <Video file dir>" << std::endl;
	}

	Sliders* interface = new Sliders (!HEADLESS, argv[1]);

	cv::waitKey (30);

	LocadedVideo *sensor = new LoadedVideo(argv[2]);

	//Realsense *sensor = new Realsense(
	//		640,				//depth_width,
	//		480,				//depth_height,
	//		30,				//depth_framerate,
	//		1920,				//bgr_width,
	//		1080,				//bgr_height,
	//		30,				//bgr_framerate,
	//		"2391016026"	//serial
	//		);

	// Set up contour info
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	// Set up the image vars
	Mat hsv_threshold, kernel_out, kernel_filtered_final, countour_out;
	vector <Mat> split_colors (3);

	Mat Grey, Kinect_RGB_Copy;

	// Kernel to detect Broccoli pebbling
	Mat kernel = Mat::ones (3, 3, CV_32F);
	kernel.at<float> (1, 1) = -8.0f;

	double default_value = 1300.00;

	//Histogram settings
	Histogram *hist = new Histogram (200, 2000);

	//Median Filter
	Median *median_filter = new Median (10, default_value);

	sensor->GrabFrames();

	while (true)  /*if (false)*/ {
		//if (false) { //Disable everything
		// Tell the *sensor to supply frames
		sensor->GrabFrames();

		// HSV thresholding on the RGB image, to detect only broccoli
		cvtColor (*sensor->bgrmatCV, hsv_threshold, COLOR_BGR2HSV); // Convert to HSV colorspace

		inRange (hsv_threshold,
				Scalar (interface->hue_slider_lower, interface->sat_slider_lower, interface->val_slider_lower),
				Scalar (interface->hue_slider_upper, interface->sat_slider_upper, interface->val_slider_upper),
				hsv_threshold);

#if(DETAILS)
		imshow ("HSV", hsv_threshold);
#endif
		// Split the bgr into it's component channels
		split (*sensor->bgrmatCV, split_colors);

		// Kernel convolute on just the green channel
		filter2D (split_colors[1], kernel_out, -1, kernel, Point (-1, -1), 0, BORDER_DEFAULT);

		// Filter out negative results on the kernel output
		threshold (kernel_out, kernel_out, interface->thresh_slider, 255, THRESH_BINARY); // Threshhold the Kernel image

#if(DETAILS)
		imshow ("Kernel", kernel_out);
#endif

		// Clear the kernel image
		kernel_filtered_final.setTo (Scalar (0, 0, 0, 0));

		// Filter the kenel image to remove things that are not within the HSV threshold
		kernel_out.copyTo (kernel_filtered_final, hsv_threshold); //, hsv_threshold);

		// Invert the image to ready it for morphologicals
		bitwise_not (kernel_filtered_final, kernel_filtered_final);

		// Filter out small eddies and areas of detection
		Mat open_element =
			getStructuringElement (0, Size (2 * interface->open_slider + 1, 2 * interface->open_slider + 1),
					Point (interface->open_slider, interface->open_slider));
		morphologyEx (kernel_filtered_final, kernel_filtered_final, MORPH_OPEN, open_element);
		Mat close_element =
			getStructuringElement (0, Size (2 * interface->close_slider + 1, 2 * interface->close_slider + 1),
					Point (interface->close_slider, interface->close_slider));
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


#if(DETAILS)
		imshow ("Pre-Canny", kernel_filtered_final);
#endif

		// Canny edge detector on the image
		Canny (kernel_filtered_final, kernel_filtered_final, interface->canny_slider, interface->canny_slider * 2, 3);

		// Find the edges of the broccoli
		findContours (kernel_filtered_final, contours, hierarchy, RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE,
				Point (0, 0)); // RETR_CCOMP = Fill holes, KEEP THIS!!


		(*sensor->bgrmatCV).copyTo (countour_out);

		unsigned int contsize = contours.size();

		Point imageCent ((*sensor->largeDepthCV).size().width / 2, (*sensor->largeDepthCV).size().height / 2);
		int brocfound = 0;
		for (unsigned int i = 0; i < contsize; i++) {
			Moments moment_set = moments (contours[i], false);
			Point2f center = Point2f (moment_set.m10 / moment_set.m00, moment_set.m01 / moment_set.m00);

			if (contourArea (contours[i]) > interface->area_slider) {
				Rect bound = cv::boundingRect (contours[i]);
				// bound -= Size(30, 30);
				rectangle (countour_out, bound.tl(), bound.br(), Scalar (255, 255, 0), 2);
				Mat boundImg = (*sensor->largeDepthCV) (bound);
				Mat boundImg16;
				boundImg.convertTo (boundImg16, CV_16UC1, 1.0);

				//Flatten the pixels into a single dimension array to be put into a histogram
				int width = bound.size().width;
				int height = bound.size().height;

				size_t data_length = (width * height) ;

				unsigned short* pixelList = new unsigned short[data_length];

				unsigned short *moving_pixelList = pixelList ;

				for (int x = 0; x < width; x++) {
					for (int y = 0; y < height; y++) {
						//pixelList[x + y * bound.size().width] = boundImg16.at<unsigned short>(y, x);
						*moving_pixelList = boundImg16.at<unsigned short> (y, x);
						moving_pixelList++ ;

					}
				}

				//Execute histogram
				hist->insert_histogram_data (pixelList, data_length);
				int value = hist->take_percentile (10);

				delete[] pixelList;

#if(!HEADLESS)
				putText (countour_out,(hack::to_string (value)+"mm").c_str(), addPoints (center, Point (-50, -150)),
						FONT_HERSHEY_COMPLEX_SMALL, 5.0, Scalar (0, 255, 255),10);
#endif

				if (interface->broc_roi.contains (center) && value > hist->min && value < hist->max) {
					drawContours (countour_out, contours, i, Scalar (0, 0, 255), 2, 8, hierarchy, 0, Point());
					median_filter->insert_median_data (value);
					brocfound++;
				} else {
					median_filter->insert_median_data ((int) default_value);
				}

			}
		}

		if (contsize == 0 || brocfound == 0) {
			median_filter->insert_median_data ((int) default_value);
			std::cerr << "No countours" << std::endl;
		}

		rectangle (countour_out, interface->broc_roi, Scalar (0,128,255), 3, 8, 0);
		int final_value = median_filter->compute_median();

#if(!HEADLESS)
		putText (countour_out, (hack::to_string (final_value)+"mm").c_str(), Point (0,70),
				FONT_HERSHEY_COMPLEX_SMALL, 5.0, Scalar (0, 255, 128), 10);
		imshow ("Contours", countour_out);

		Mat tri_channel_depth;
		Mat tri_channel_depth_8u;
		cvtColor(*sensor->largeDepthCV / 6, tri_channel_depth, CV_GRAY2RGB);
		tri_channel_depth.convertTo(tri_channel_depth_8u, CV_8U);

		//rgbcap.write(countour_out);
		//depthcap.write(tri_channel_depth_8u);

		if (cv::waitKey (30) == 27) {
			return 0;
		}
#endif
		cout << final_value << endl;



#if(DETAILS)
		imshow ("Depth", *sensor->largeDepthCV * 20);
#endif

		// cv::waitKey(1);
	}

	return 0;
}
