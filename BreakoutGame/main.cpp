#include <QtWidgets/QApplication>
#include "breakout.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	Breakout window;

	window.setWindowTitle("Window");
	window.setWindowIcon(QIcon("Finch.jpg"));
	window.show();

	return a.exec();
}
