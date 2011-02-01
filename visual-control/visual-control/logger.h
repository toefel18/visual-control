#ifndef VC_LOGGER_H
#define VC_LOGGER_H
#include <string>

class QListWidget;

class Logger
{
public:

	void log(const std::string &msg);

	static Logger& getInstance();

	void setLogWidget(QListWidget *logWidget);

protected:

	Logger();

private:
	QListWidget *logWidget;
	static Logger instance;
};


#endif //VC_LOGGER_H