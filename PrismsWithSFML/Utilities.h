#pragma once
#include "stdafx.h"

#include <chrono>

namespace utilities
{
	//custom chrono class to measure durations in code
	class Chrono
	{
	private:
		std::chrono::system_clock::time_point t;
		bool isChronoStarted = false;
	public:

		Chrono() = default;

		// initialize the chronometer
		void startChrono()
		{
			t = std::chrono::system_clock::now();	// Set start value for timer.
			isChronoStarted = true;
		}

		// get the elpased time since last call
		double getChronoElapsedTime()
		{
			if (isChronoStarted)
			{
				// Compute frame time:
				std::chrono::system_clock::time_point t2 = std::chrono::system_clock::now();
				std::chrono::duration<double> dt{ t2 - t };
				t = t2;
				return dt.count() * 1000;
			}
			else
			{
				throw std::logic_error("Chrono not started");
			}
		}
	};
}


