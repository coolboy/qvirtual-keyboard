#include "stdafx.h"
#include "virtualkeyboard.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	VirtualKeyboard w;
	w.show();
	return a.exec();
}
