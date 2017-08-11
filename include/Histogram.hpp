#ifndef HISTOGRAM_HPP
#define HISTOGRAM_HPP
#include <cmath>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <opencv2/opencv.hpp>

template<
typename T, //real type
			typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type
			>
			class Histogram
{
	private:
		T* histogram;
		size_t range;
		unsigned long data_sum = 0;

		void clear_histogram() {
			memset (histogram, 0, sizeof (T) * range);
			data_sum = 0;
		}

	public:
		size_t min, max;

		Histogram (int min, int max) {
			this->min = min;
			this->max = max;
			range = max - min;
			histogram = new T [range];
			clear_histogram();
		}

		/*
		void insert_histogram_data (T* data, size_t data_length) {
			clear_histogram();
			for (size_t i = 0; i < data_length; i++) {
				if (data[i] >= min && data[i] <= max) {
					histogram[data[i] - min]++;
					data_sum ++;
				}
			}
		}
		*/

		void insert_histogram_data (cv::Mat* image) {
			clear_histogram();
			T pixel = 0;
			for (size_t x = 0; x < image->size().width; x++) { //TODO: Find a faster method
				for (size_t y = 0; y < image->size().height; y++) { //TODO: Find a faster method
					pixel = image->at<T> (y, x); 
					if (pixel >= min && pixel <= max) {
						histogram[pixel - min]++;
						data_sum++;
					}
				}
			}
		}

		T take_percentile (int percentile) {
			T* histogram_start_copy = histogram;
			unsigned long long accumulator = 0;
			int percentile_max = ceil ( ((float) percentile / 100.0f) * (float) data_sum);
			size_t bin_number = 0;

			while (accumulator < percentile_max && bin_number < range) {
				accumulator += *histogram_start_copy;
				histogram_start_copy++;
				bin_number++;
			}

			if (bin_number > 0) {
				return bin_number - 1 + min;
			} else {
				return 0;
			}
		}

		~Histogram() {
			delete[] histogram;
		}
};
#endif
