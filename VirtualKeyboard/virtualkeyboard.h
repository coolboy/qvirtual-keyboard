#pragma once

#include <QtGui/QDialog>
#include "ui_virtualkeyboard.h"

class widgetKeyBoard;

class VirtualKeyboard : public QDialog
{
	Q_OBJECT

public:
	VirtualKeyboard(QWidget *parent = 0, Qt::WFlags flags = 0);
	~VirtualKeyboard();

protected slots:
	void slotOnUserSelection(QString text);

private:
	Ui::VirtualKeyboardClass ui;

	widgetKeyBoard  *myKeyboard;
};
