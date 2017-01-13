#pragma once
#include <hcube/config.h>

namespace hcube
{
	class HCUBE_API timer
	{

	public:

		timer();

		void reset();

		double get_time() const;

		double get_time_ms() const;

	protected:

		double m_start_time{ 0 };

	};

	class HCUBE_API fps_counter
	{
	public:

		void reset();
		
		double count_frame();

	private:

		timer  m_timer;
		double m_count{ 0 };
		double m_last { 0 };

	};

}