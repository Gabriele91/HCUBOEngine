#include <hcube/utilities/timer.h>
#include <hcube/core/query_performance.h>

namespace hcube
{
	///////////////////////////////////////////////////////////
	timer::timer()
	{
		reset();
	}

	void timer::reset()
	{
		m_start_time = query_performance::get_time();
	}

	double timer::get_time() const
	{
		return query_performance::get_time() - m_start_time;
	}

	double timer::get_time_ms() const
	{
		return query_performance::get_time_ms() - m_start_time * 1000.0;
	}
	///////////////////////////////////////////////////////////
	void fps_counter::reset()
	{
		m_timer.reset();
		m_count = 0;
		m_last = 0;
	}

	double fps_counter::count_frame()
	{
		//sum
		m_count += 1;

		//time
		double time = m_timer.get_time();

		//return fps
		if (time >= 1.0)
		{
			//compute number of frames
			m_last = m_count / time;
			//reset timer
			m_timer.reset();
			//reset count
			m_count = 0;
		}

		//return last
		return m_last;
	}
}