#include "stdafx.h"
#include "virtualkeyboard.h"

#include "widgetKeyBoard.h"

VirtualKeyboard::VirtualKeyboard(QWidget *parent, Qt::WFlags flags)
	: QDialog(parent, flags)
{
	ui.setupUi(this);

	myKeyboard = new widgetKeyBoard(false, this);
	myKeyboard->setZoomFacility(true);
	myKeyboard->enableSwitchingEcho(false); // enable possibility to change echo through keyboard
	myKeyboard->createKeyboard(); // only create keyboard

	//set the focus
	//myKeyboard->show (this);//set the focus to the widget
	myKeyboard->focusThis(ui.inputLE);//set the focus to the line edit

	ui.gridLayout->addWidget(myKeyboard);
}

VirtualKeyboard::~VirtualKeyboard()
{

}
