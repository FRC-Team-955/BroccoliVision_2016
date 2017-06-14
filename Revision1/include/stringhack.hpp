//TODO: Add this to it's own class! It reoccures in main.cpp
#include <sstream>
#include <string>

class get_frame_data;
namespace hack
{
	template <typename T> std::string to_string (const T& n)
	{
		std::ostringstream stm;
		stm << n;
		return stm.str();
	}
}

