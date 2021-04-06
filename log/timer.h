/** timer module
 * 
 * unit: ms
 */

#ifndef AVS_API_TIMER_H
#define AVS_API_TIMER_H

#include <map>
#include <memory>
#include <sys/time.h>

class TimerModule
{
	friend class Profile;
	friend class Module;
	explicit TimerModule(const char* name) : m_name(name)
	{
	}

public:
    const char* getName() const {return m_name.c_str();}

    void recordElapsed(const std::string& process, double elapsed)
    {
        auto itr = m_elapseds.find(process);
        if (itr == m_elapseds.end())
        {
            m_elapseds.emplace(process, elapsed);
        }
        else
        {
            itr->second += elapsed;
        }
    }

private:
    std::string m_name;
    std::map<std::string, double> m_elapseds;
};

class Timer final
{
public:
	explicit Timer(std::shared_ptr <TimerModule>& parent) : m_module(parent)
	{
	}

    void start(const char* name)
    {
		m_name = name;
        gettimeofday(&m_startPoint, NULL);
    }

    void stop()
    {
        gettimeofday(&m_endPoint, NULL);
        double t = (m_endPoint.tv_sec - m_startPoint.tv_sec) * 1000 + (m_endPoint.tv_usec - m_startPoint.tv_usec) * 1e-3;
		m_module->recordElapsed(getName(), t);
    }

    const char* getName() const {return m_name.c_str();}

private:
    std::string m_name;
	std::shared_ptr<TimerModule> m_module;
    timeval m_startPoint, m_endPoint;
};

#endif