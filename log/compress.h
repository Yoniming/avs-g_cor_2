#ifndef AVS_API_COMPRESS_H
#define AVS_API_COMPRESS_H

#include <string>
#include <map>

// 记录压缩结果
class CompressUnit
{
public:
	CompressUnit() = default;

	CompressUnit(uint64_t originSize, uint64_t compressedSize)
		:m_originSize(originSize)
		, m_compressedSize(compressedSize)
	{
		calculateRatio();
	}

	void append(uint64_t originSize, uint64_t compressedSize)
	{
		m_originSize += originSize;
		m_compressedSize += compressedSize;
		calculateRatio();
	}

	uint64_t getOriginSize() const { return m_originSize; }
	uint64_t getCompressedSize() const { return m_compressedSize; }
	double getRatio() const { return m_ratio; }
	double getRate() const { return m_rate; }

private:
	void calculateRatio()
	{
		m_ratio = (m_originSize * 1.0) / (m_compressedSize * 1.0);
	}
	void calculateRate()
	{
		m_ratio = (m_compressedSize * 1.0) / (m_originSize * 1.0);
	}
		
private:
	uint64_t m_originSize = 0;
	uint64_t m_compressedSize = 0;
	double m_ratio = 0.0;
	double m_rate = 0.0;
};

class CompressModule
{
	friend class Profile;
	friend class Module;

	CompressModule(const char* name) : m_name(name)
	{
	}

public:
	const char* getName() const {return m_name.c_str();}
	
	void recordCompress(const char* name, uint64_t originSize, uint64_t compressedSize)
	{
        auto itr = m_compressUnits.find(name);
        if (itr == m_compressUnits.end())
        {
            m_compressUnits[name] = std::make_shared<CompressUnit>(originSize, compressedSize);
        }
        else
        {
			itr->second->append(originSize, compressedSize);
        }
	}

private:
	const std::string m_name;
	std::map<std::string, std::shared_ptr<CompressUnit>> m_compressUnits;
};

#endif