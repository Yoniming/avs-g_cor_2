#ifndef AVS_API_LOGGER_H
#define AVS_API_LOGGER_H

#include <cstdio>
#include <cstdarg>
#include <string>
#include <fstream>
#include <map>
#include <memory>

// #ifdef _WIN32 | _WINDOWS
// #include <io.h>                      //C (Windows)    access
// #include <direct.h>
// #else
#include <unistd.h>                  //C (Linux)      access   
//#endif

enum class level : int
{
    DEBUG = 0,
    INFO,
    WARN,
    ERROR,
};

#define __to_buffer__ \
    char buffer[512]; \
    va_list args; \
    va_start (args, fmt); \
    vsnprintf (buffer, sizeof(buffer), fmt, args); \
    va_end (args);

#define __output__(stream, lv, buffer) \
    switch (lv) \
    { \
    case level::DEBUG: \
        fprintf(stdout, "%s\n", buffer); \
		break;\
    case level::ERROR: \
        fprintf(stdout, "\033[31m%s\033[0m\n", buffer); \
        break; \
    case level::WARN: \
        fprintf(stdout, "\033[33m%s\033[0m\n", buffer); \
        break; \
    case level::INFO: \
        fprintf(stdout, "\033[34m%s\033[0m\n", buffer); \
        break;     \
    default: \
        fprintf(stdout, "%s\n", buffer); \
        break; \
    }

static void arc_stdout(level lv, const char* fmt, ...)
{
    __to_buffer__
    __output__(stdout, lv, buffer);
}

static void arc_stderr(level lv, const char* fmt, ...)
{
    __to_buffer__
    __output__(stderr, lv, buffer);
}

class Logger
{
public:
    Logger()
    {
        m_filePath = "./";
		m_filePath.append("arclog").append("/");

		if (access(m_filePath.c_str(), 0) != 0)
		{
			mkdir(m_filePath.c_str(), 0775);
		}
    }
    ~Logger()
    {
        if (m_fstream)
        {
            m_fstream.flush();
            m_fstream.clear();
            m_fstream.close();
        }
    }
    void setLogName(string &name)
    {
        m_filePath.append(name).append(".log");
        m_fstream.open(m_filePath, std::ios::out);
        if (!m_fstream)
        {
            arc_stderr(level::ERROR, "fail to open log file %s", m_filePath.c_str());
        }
    }

    void writeHeader(level lv)
    {
        switch (lv)
        {
        case level::ERROR:
            m_fstream << "[ERROR] ";
            break;
        case level::WARN:
            m_fstream << "[WARN]  ";
            break;
        case level::INFO:
            m_fstream << "[INFO]  ";
            break;    
        default:
            m_fstream << "[*****] ";
            break;
        }
    }

    void output(level lv, const char* buffer)
    {
        arc_stdout(lv, "%s", buffer);
        writeHeader(lv);
        m_fstream << buffer << std::endl;
    }

    void info(const char* fmt, ...)
    {
		__to_buffer__
			output(level::INFO, buffer);
    }
private:
    std::string m_filePath;
    std::ofstream m_fstream;
};

class Manager;

/** 文件日志类
 * 
 * NOTO:
 * 一个实例对应一个日志文件。
 * 非线程安全
 */
class FileLogger final
{
    friend class Manager;

    std::ofstream m_fstream;
    bool m_toStdout = true;
    FileLogger(const char* dir, const char* logger_name, bool to_stdout = true)
    {
        m_filePath = dir,
        m_filePath.append("/").append(logger_name).append(".log");

        m_toStdout = to_stdout;
        openFile();
    }

    void openFile()
    {
        m_fstream.open(m_filePath.c_str(), std::ios::trunc);
        if (!m_fstream)
        {
            arc_stderr(level::ERROR, "fail to open log file %s", m_filePath.c_str());
        }
    }

    void closeFile()
    {
        if (m_fstream)
        {
            m_fstream.flush();
            m_fstream.clear();
            m_fstream.close();
        }
    }

    void writeHeader(level lv)
    {
        switch (lv)
        {
        case level::ERROR:
            m_fstream << "[ERROR] ";
            break;
        case level::WARN:
            m_fstream << "[WARN]  ";
            break;
        case level::INFO:
            m_fstream << "[INFO]  ";
            break;    
        default:
            m_fstream << "[*****] ";
            break;
        }
    }

    void output(level lv, const char* buffer)
    {
        if (m_toStdout)
        {
            arc_stdout(lv, "%s", buffer);
        }

        writeHeader(lv);

        // log body
        m_fstream << buffer << std::endl;
    }
public:
    ~FileLogger()
    {
        closeFile();
    }

    const std::string& getFilePath() const {return m_filePath;}

    void debug(const char* fmt, ...)
    {
		__to_buffer__
			output(level::DEBUG, buffer);
    }

    void info(const char* fmt, ...)
    {
		__to_buffer__
			output(level::INFO, buffer);
    }
    void warn(const char* fmt, ...)
    {
		__to_buffer__
			output(level::WARN, buffer);
    }
    void error(const char* fmt, ...)
    {
		__to_buffer__
			output(level::ERROR, buffer);
    }

private:
    std::string m_filePath;
};

class Manager
{
    Manager() = default;

public:
    static Manager& getInstance()
    {
        static Manager _inst;
        return _inst;
    }

    ~Manager()
    {
        // if (!m_filePath.empty())
        // {
        //     mergeAllLogFiles();
        // }
    }

	/**
	 * 设置最终日志文件名称，同时也作为所有日志文件保存的路径.
	 * 一经设置，不可再次修改
	 */
    void setLogFileName(const char* name)
    {
		if (!m_filePath.empty())
		{
			arc_stderr(level::ERROR, "%s: log file name had been set, cannot to set again!", __FUNCTION__);
			return;
		}

		initFileDirector(name);

		m_filePath = getFileDirector();
        m_filePath.append(name).append(".log");
    }

	std::shared_ptr<FileLogger> getMainLogger()
	{
		if (!m_mainLogger)
			m_mainLogger = createLogger("main");

		return m_mainLogger;
	}

    std::shared_ptr<FileLogger> getLogger(const char* name)
    {
		if (strcmp(name, "main") == 0)
		{
			return getMainLogger();
		}

        auto itr = m_loggers.find(name);
        if (itr != m_loggers.end())
        {
            return itr->second;
        }

		auto logger = createLogger(name);
        m_loggers.emplace(name, logger);
        return logger;
    }

private:
	void initFileDirector(const char* name)
	{
		m_fileDirector = "./";
		m_fileDirector.append("arclog").append("/");

		if (access(m_fileDirector.c_str(), 0) != 0)
		{
			mkdir(m_fileDirector.c_str(), 0775);
		}
	}

	const char* getFileDirector() const
	{
		return m_fileDirector.c_str();
	}

	std::shared_ptr<FileLogger> createLogger(const char* name)
	{
		return std::shared_ptr<FileLogger>(new FileLogger(getFileDirector(),name));
	}

    void mergeAllLogFiles()
    {
        std::ofstream target(m_filePath, std::ios::trunc);
        if (!target)
        {
            arc_stderr(level::ERROR, "fail to open file %s", m_filePath.c_str());
            return;
        }

        for(auto itr : m_loggers)
        {
            auto& log = itr.second;

            // should close before copy
            log->closeFile();

            if (!copyLogFile(log->getFilePath(), target))
                break;

        }

		copyLogFile(m_mainLogger->getFilePath(), target);

        target.flush();
        target.clear();
        target.close();
    }

    bool copyLogFile(const std::string file, std::ofstream& target)
    {
        std::ifstream src(file);
        if (!src)
        {
            arc_stderr(level::ERROR, "fail to open file %s", file.c_str());
            return false;
        }

        src.seekg(0, std::ios_base::end);
        size_t len = src.tellg();
        src.seekg(0, std::ios_base::beg);

        char* buffer = new char[len + 1];
        memset(buffer, 0, len + 1);
        src.read(buffer, len);
        src.clear();
        src.close();
        target << buffer;
        delete [] buffer;

        return true;
    }


private:
    std::map<std::string, std::shared_ptr<FileLogger>> m_loggers;

	// 主线程使用的logger,无需人为构造，只需调用getMainLogger()即可
	std::shared_ptr<FileLogger> m_mainLogger;

	/**
	 * 本进程所有日志文件保存的目录
	 *
	 * 默认为"./"，即当前路径
	 * 一经设置，不可再次修改
	 */
	std::string m_fileDirector;

    /**
     * 该文件由所有的logger file最终合并得到。
     * 
     * 如果未设置该文件名，那么不执行合并；
     * 设置文件后，会在该实例析构时自动执行合并，该过程往往是在程序退出时。
     */
    std::string m_filePath;
};

#endif