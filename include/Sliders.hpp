#ifndef SLIDERS_HPP
#define SLIDERS_HPP

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <pugixml.hpp>
#include <string>

using namespace cv;
using namespace pugi;

class Sliders {
	private:
		static void on_trackbar(int newVal, void * object) { //Stuipid HACK that I have to do because opencv wants a static callback function
			Sliders* slidervals = (Sliders*) object;
			slidervals->saveSliders();
		};
		String directory;
	public:
		// HSV
		int hue_slider_lower = 0;
		int hue_slider_upper = 179;
		int sat_slider_lower = 0;
		int sat_slider_upper = 256;
		int val_slider_lower = 0;
		int val_slider_upper = 256;
		// Morphology
		int open_slider = 0;
		int close_slider = 0;
		// Blur size
		int thresh_slider = 0;
		// Canny
		int canny_slider = 0;
		// Minimum area for detection
		int area_slider = 7069;
		// ROI
		Rect broc_roi;


		Sliders(bool showUI, String dir) {
			directory = dir;
			xml_document settings;
			//try {
			xml_parse_result result = settings.load_file(directory.c_str());
			xml_node settingRoot = settings.child("sliders");
			hue_slider_lower = settingRoot.attribute("HueLw").as_int();
			hue_slider_upper = settingRoot.attribute("HueUp").as_int();
			sat_slider_lower = settingRoot.attribute("SatLw").as_int();
			sat_slider_upper = settingRoot.attribute("SatUp").as_int();
			val_slider_lower = settingRoot.attribute("ValLw").as_int();
			val_slider_upper = settingRoot.attribute("ValUp").as_int();
			open_slider = settingRoot.attribute("MorphOpen").as_int();
			close_slider = settingRoot.attribute("MorphClose").as_int();
			thresh_slider = settingRoot.attribute("Thresh").as_int();
			area_slider = settingRoot.attribute("Area").as_int();
			canny_slider = settingRoot.attribute("Canny").as_int();
			broc_roi.x = settingRoot.attribute("xPos").as_int();
			broc_roi.y = settingRoot.attribute("yPos").as_int();
			broc_roi.width = settingRoot.attribute("Width").as_int();
			broc_roi.height = settingRoot.attribute("Height").as_int();

			//} catch (int e) {
			//	std::cout << "Slider XML parse failed" << std::endl;
			//}

			if (showUI) {
				Mat sliderWidth(1, 1920 / 1, 0); //HACK to make a separate window for the sliders with a custom width
				namedWindow("Sliders", CV_WINDOW_AUTOSIZE);
				imshow("Sliders", sliderWidth);
				// Hue
				cvCreateTrackbar2("Hue Lw", "Sliders", &hue_slider_lower, 179, on_trackbar, this);
				cvCreateTrackbar2("Hue Up", "Sliders", &hue_slider_upper, 179, on_trackbar, this);

				// Sat
				cvCreateTrackbar2("Sat Lw", "Sliders", &sat_slider_lower, 256, on_trackbar, this);
				cvCreateTrackbar2("Sat Up", "Sliders", &sat_slider_upper, 256, on_trackbar, this);
				// Val
				cvCreateTrackbar2("Val Lw", "Sliders", &val_slider_lower, 256, on_trackbar, this);
				cvCreateTrackbar2("Val Up", "Sliders", &val_slider_upper, 256, on_trackbar, this);
				// Thresh
				cvCreateTrackbar2("Thresh", "Sliders", &thresh_slider, 256, on_trackbar, this);
				// Morph
				cvCreateTrackbar2("Open", "Sliders", &open_slider, 20, on_trackbar, this);
				cvCreateTrackbar2("Close", "Sliders", &close_slider, 20, on_trackbar, this);
				// Canny
				cvCreateTrackbar2("Canny", "Sliders", &canny_slider, 20, on_trackbar, this);
				// Area
				cvCreateTrackbar2("Area", "Sliders", &area_slider, 9999, on_trackbar, this);
				//ROI
				cvCreateTrackbar2("xPos", "Sliders", &broc_roi.x, 1920, on_trackbar, this);
				cvCreateTrackbar2("yPos", "Sliders", &broc_roi.y, 1080, on_trackbar, this);
				cvCreateTrackbar2("Width", "Sliders", &broc_roi.width, 1920, on_trackbar, this);
				cvCreateTrackbar2("Height", "Sliders", &broc_roi.height, 1080, on_trackbar, this);

			}
		}

		void saveSliders () {
			xml_document settings;
			xml_node sliders = settings.append_child("sliders");
			sliders.append_attribute("HueLw").set_value(hue_slider_lower);
			sliders.append_attribute("HueUp").set_value(hue_slider_upper);
			sliders.append_attribute("SatLw").set_value(sat_slider_lower);
			sliders.append_attribute("SatUp").set_value(sat_slider_upper);
			sliders.append_attribute("ValLw").set_value(val_slider_lower);
			sliders.append_attribute("ValUp").set_value(val_slider_upper);
			sliders.append_attribute("MorphOpen").set_value(open_slider);
			sliders.append_attribute("MorphClose").set_value(close_slider);
			sliders.append_attribute("Thresh").set_value(thresh_slider);
			sliders.append_attribute("Canny").set_value(canny_slider);
			sliders.append_attribute("Area").set_value(area_slider);
			sliders.append_attribute("xPos").set_value(broc_roi.x);
			sliders.append_attribute("yPos").set_value(broc_roi.y);
			sliders.append_attribute("Width").set_value(broc_roi.width);
			sliders.append_attribute("Height").set_value(broc_roi.height);
			settings.save_file(directory.c_str());
		}
};

#endif // SLIDERS_HPP
