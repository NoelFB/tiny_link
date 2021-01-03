#include "timer.h"

using namespace TL;

Timer::Timer(float duration, const std::function<void(Timer* self)>& on_end)
	: m_duration(duration), on_end(on_end)
{

}

void Timer::start(float duration)
{
	m_duration = duration;
}

void Timer::update()
{
	if (m_duration > 0)
	{
		m_duration -= Time::delta;
		if (m_duration <= 0 && on_end)
		{
			on_end(this);
		}
	}
}
