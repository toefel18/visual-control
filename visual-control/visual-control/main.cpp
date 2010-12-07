#include "gui/visualcontrol.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	gui::VisualControl w;
	w.show();
	return a.exec();
}
