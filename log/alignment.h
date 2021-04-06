#ifndef AVS_API_ALIGNMENT_H
#define AVS_API_ALIGNMENT_H

#include <string>
#include <map>

class AlignmentModule
{
	friend class Profile;
	friend class Module;

	AlignmentModule(const char* name) : m_name(name)
	{
	}

public:
	const char* getName() const {return m_name.c_str();}
	void updateMisOfWithin(unsigned index)
	{
		m_misOfWithin[index]++;
	}
	void updateMisOutRange(unsigned index)
	{
		m_misOutRange[index]++;
	}
	void updateMisOfRead1(unsigned index)
	{
		m_misOfRead1[index]++;
	}
	void updateMisOfRead2(unsigned index)
	{
		m_misOfRead2[index]++;
	}

	void setBlockMapCount(int map, int unmap)
	{
		m_mapCount += map;
		m_unmapCount += unmap;
	}

	void setAlignBlockCount()
	{
		m_alignBlockCount++;
	}

	void merge(const AlignmentModule& other)
	{
		merge(m_misOfWithin, other.m_misOfWithin);
		merge(m_misOutRange, other.m_misOutRange);
		merge(m_misOfRead1, other.m_misOfRead1);
		merge(m_misOfRead2, other.m_misOfRead2);

		m_mapCount += other.m_mapCount;
		m_unmapCount += other.m_unmapCount;

		m_alignBlockCount += other.m_alignBlockCount;
	}

private:
	static void merge(std::map<int, int>& out, const std::map<int, int>& in)
	{
		for (auto& it : in)
		{
			auto itr = out.find(it.first);
			if (itr == out.end())
			{
				out[it.first] = it.second;
			}
			else
			{
				out[it.first] += it.second;
			}
		}
	}

private:
    const std::string m_name;

	std::map<int, int> m_misOfWithin;
	std::map<int, int> m_misOutRange;
	std::map<int, int> m_misOfRead1;
	std::map<int, int> m_misOfRead2;

	int m_alignBlockCount = 0;

	int m_unmapCount = 0;
	int m_mapCount = 0;
};

#endif