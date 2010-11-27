#include "visualcontrol.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	VisualControl w;
	w.show();
	return a.exec();
}
