#include "stdafx.h"
#include "virtualkeyboard.h"

#include "widgetKeyBoard.h"

VirtualKeyboard::VirtualKeyboard(QWidget *parent, Qt::WFlags flags)
	: QDialog(parent, flags)
{
	ui.setupUi(this);

	myKeyboard = new widgetKeyBoard(false, this /*set the focus to the widget*/);
	myKeyboard->setZoomFacility(true);
	myKeyboard->enableSwitchingEcho(false); // enable possibility to change echo through keyboard
	myKeyboard->createKeyboard(); // only create keyboard

	//set the focus
	myKeyboard->focusThis(ui.inputLE);//set the focus to the line edit

	//add the keyboard to the layout
	ui.gridLayout->addWidget(myKeyboard);

	connect(myKeyboard, SIGNAL(sigOnUserSelection(QString)), this, SLOT(slotOnUserSelection(QString)));

}

VirtualKeyboard::~VirtualKeyboard()
{

}

void VirtualKeyboard::slotOnUserSelection( QString text )
{
	ui.inputLE->setText(ui.inputLE->text() + text);
}
