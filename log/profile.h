
#ifndef AVS_API_PROFILE_H
#define AVS_API_PROFILE_H

#include <mutex>
#include "timer.h"
#include "compress.h"
#include "alignment.h"
#include "logger.h"

class Module
{
	friend class Profile;

	explicit Module(const char* name) :m_name(name)
	{
		if(m_name == "main")
		{
			m_logger = std::shared_ptr<Logger>(new Logger());
		}
		m_compress = std::shared_ptr<CompressModule>(new CompressModule(name));
		m_timer = std::shared_ptr<TimerModule>(new TimerModule(name));
		m_align = std::shared_ptr<AlignmentModule>(new AlignmentModule(name));
	}

public:
	const char* getName() const {return m_name.c_str();}
	std::shared_ptr<Logger> getLogger() { return m_logger; }
	std::shared_ptr<CompressModule> getCompress() { return m_compress; }
	std::shared_ptr<TimerModule>  getTimerModule() { return m_timer; }
	std::shared_ptr<AlignmentModule> getAlignment() {return m_align;}

	std::shared_ptr<Timer>  getTimer() { return std::shared_ptr<Timer>(new Timer(m_timer)); }

private:
	const std::string m_name;
	std::shared_ptr<Logger> m_logger;
	std::shared_ptr<CompressModule>  m_compress;
	std::shared_ptr<TimerModule>  m_timer;
	std::shared_ptr<AlignmentModule>  m_align;
};

class Profile
{
    Profile() = default;

public:
	static Profile& getInstance()
	{
		static Profile _inst;
		return _inst;
	}

    void outputElapsed()
    {
		auto logger = getModule("main")->getLogger();

		logger->info("******************************************************************************************");
        logger->info("time profile list:\n");
        logger->info("%-40s%s", "process", "elapsed(s)");

		std::map<std::string, uint64_t> elapsed;

		for (const auto& itr : m_profileModules)
		{
			for (const auto& it : itr.second->getTimerModule()->m_elapseds)
			{
				elapsed[it.first] += it.second;
			}
		}

		for (const auto& it : elapsed)
		{
			logger->info("%-40s%8.2f", it.first.c_str(), it.second*1.0 / 1000);
		}
		
		logger->info("******************************************************************************************");
    }

	void outputCompressResult()
	{
		auto logger = getModule("main")->getLogger();

		logger->info("******************************************************************************************");
		logger->info("compress profile list:");
		logger->info("name                 originSize                 compress                   ratio");
		logger->info("------------------------------------------------------------------------------------------");

		std::map<std::string, CompressUnit> comps;
		CompressUnit nameComp;
		CompressUnit seqComp;
		CompressUnit qualComp;

		for (const auto& itr : m_profileModules)
		{
			const auto& name = itr.first;
			const auto& units = itr.second->getCompress()->m_compressUnits;
			for (const auto& it : units)
			{
				if (it.first == "name")
				{
					nameComp.append(it.second->getOriginSize(), it.second->getCompressedSize());
				}
				else if (it.first == "seq")
				{
					seqComp.append(it.second->getOriginSize(), it.second->getCompressedSize());
				}
				else if (it.first == "qual")
				{
					qualComp.append(it.second->getOriginSize(), it.second->getCompressedSize());
				}
				else
				{
					comps[it.first].append(it.second->getOriginSize(), it.second->getCompressedSize());
				}
			}
		}
		
		outputCompressUnit("name", nameComp);
		outputCompressUnit("seq", seqComp);
		outputCompressUnit("qual", qualComp);
		for (const auto& it : comps)
		{
			outputCompressUnit(it.first.c_str(), it.second);
		}

		logger->info("******************************************************************************************");
	}

	void outputSEAlignmentMisResult(int totalBlockCount)
	{
		auto logger = getModule("main")->getLogger();

		logger->info("******************************************************************************************");
		AlignmentModule total("for total");
		for (const auto& it : m_profileModules)
		{
			total.merge(*(it.second->getAlignment()));
		}

		outputAlignmentTotal(totalBlockCount, total);

		logger->info("SE aligment profile list:");
		outputAlignmentMisResult("mis", total.m_misOfWithin);
		logger->info("******************************************************************************************");
	}

	void outputPEAlignmentMisResult(int totalBlockCount)
	{
		auto logger = getModule("main")->getLogger();

		logger->info("******************************************************************************************");
		AlignmentModule total("for total");
		for (const auto& it : m_profileModules)
		{
			total.merge(*(it.second->getAlignment()));
		}

		outputAlignmentTotal(totalBlockCount, total);

		logger->info("PE aligment profile list:");
		outputAlignmentMisResult("mis of within", total.m_misOfWithin);
		logger->info("------------------------------------------------------------------------------------------");
		outputAlignmentMisResult("mis out range", total.m_misOutRange);
		logger->info("------------------------------------------------------------------------------------------");
		outputAlignmentMisResult("mis of read 1", total.m_misOfRead1);
		logger->info("------------------------------------------------------------------------------------------");
		outputAlignmentMisResult("mis of read 2", total.m_misOfRead2);

		logger->info("******************************************************************************************");
	}

	std::shared_ptr<Module> getModule(const char* name)
	{
        std::lock_guard<std::mutex> lock(m_moduleMtx);
        auto itr = m_profileModules.find(name);
        if (itr != m_profileModules.end())
        {
            return itr->second;
        }

		auto module = std::shared_ptr<Module>(new Module(name));
        m_profileModules.emplace(name, module);
        return module;
	}

private:
	void outputAlignmentMisResult(const char* type, const std::map<int, int>& mis)
	{
		auto logger = getModule("main")->getLogger();

		logger->info("%s:", type);
		for (const auto& itr : mis)
		{
			logger->info("mis%d: %d", itr.first, itr.second);
		}
	}
	void outputAlignmentTotal(int totalBlockCount, const AlignmentModule& total)
	{
		auto logger = getModule("main")->getLogger();
		logger->info("%-20s%-20s", "mapcount", "unmapcount");
		logger->info("%-20d%-20d", total.m_mapCount, total.m_unmapCount);
		logger->info("------------------------------------------------------------------------------------------");
		logger->info("%-20s%-20s%-20s%20s", "Total Block Count", "Align Block Clunt", "Fqz Block Count", "Ratio");
		logger->info("%-20d%-20d%-20d%20.2f", totalBlockCount, total.m_alignBlockCount, totalBlockCount - total.m_alignBlockCount, (total.m_alignBlockCount * 1.0) / (totalBlockCount * 1.0));
		logger->info("------------------------------------------------------------------------------------------");
	}

	void outputCompressUnit(const char* name, const CompressUnit& unit)
	{
		auto logger = getModule("main")->getLogger();
		logger->info("%-20s %-26llu %-26llu %-8.2f", 
			name,
			unit.getOriginSize(), unit.getCompressedSize(), unit.getRatio());
	}


public:
    std::mutex m_moduleMtx;
	std::map<std::string, std::shared_ptr<Module>> m_profileModules;
};

#endif