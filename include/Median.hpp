#ifndef MEDIAN_HPP
#define MEDIAN_HPP
#include <cmath>
#include <stdio.h>
#include <deque>
#include <algorithm>
#include <type_traits>

template<
    typename T, //real type
    typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type
>
class Median
{
	private:
		std::deque<T> median_stack;
		size_t max_stack_length;
		T default_value;
		
	public:
		~Median() {
		}

		Median (size_t max_stack_length, T default_value) {
			this->max_stack_length = max_stack_length;
			this->default_value = default_value;
		}

		//TODO: Make this a pointer
		void insert_median_data (T data) {
			median_stack.push_front (data);

			if (median_stack.size() > max_stack_length) {
				median_stack.pop_back();
			}
		}

		T compute_median () {							
			if (median_stack.size() != 0)
			{
				std::deque<T> median_stack_copy = median_stack;
				std::sort (median_stack_copy.begin(), median_stack_copy.end());
				return median_stack_copy[median_stack_copy.size() / 2];
			} 
			else 
			{
				return default_value;
			}
		}
};
#endif
