#ifndef _LOGGER_
#define _LOGGER_

#include <vector>
#include <string>

class Log
{
public:
	static Log *	instance;
	static Log *getInstance()
	{
		if (instance == NULL)
			instance = new Log();
		return instance;
	};

	void log(std::string log);
	void clientlog(std::string log);
	std::vector<std::string>	loglist;
	std::vector<std::string>	clientloglist;

private:
	Log();
	~Log();
};


#endif