#include <cmath>
#include <stdio.h>

class Histogram
{
	private:
		unsigned short* histogram;
		size_t range;
		unsigned long data_sum = 0;
		
		void clear_histogram() {
			memset (histogram, 0, sizeof (unsigned short) * range);
			data_sum = 0;
		}
		
	public:
		size_t min, max;
		Histogram (int min, int max) {
			this->min = min;
			this->max = max;
			range = max - min;
			histogram = new unsigned short [range];
			clear_histogram();
			
			//DumpHistogram() ;
		}
		
		void insert_histogram_data (unsigned short* data, size_t data_length) {
			clear_histogram();
			
			for (size_t i = 0; i < data_length; i++) {
				if (data[i] >= min && data[i] <= max) {
					histogram[data[i] - min]++;
					//data_sum += data[i];
					data_sum ++ ;
				}
			}
			
			//DumpHistogram() ;
		}
		
		unsigned short take_percentile (int percentile) {
			unsigned short* histogram_start_copy = histogram;
			unsigned long accumulator = 0;
			int percentile_max = ceil ( (float) percentile / 100.0f * (float) data_sum);
			size_t bin_number = 0;
			
			while (accumulator < percentile_max) {
				accumulator += *histogram_start_copy;
				histogram_start_copy++;
				bin_number++;
			}
			
			if (bin_number - 1 + min >= 0) {
				return bin_number - 1 + min;
			} else {
				return 0;
			}
		}
		
		void DumpHistogram(void)
		{
			for (int i = 0; i<range; i++)
				 cout << histogram[i] << endl;
				//printf("%d, %d\n",i,histogram[i]) ;
		}
		
		
		~Histogram() {
			
			delete[] histogram ;
		}
};
