/***************************************************************************
*   Copyright (C) 2008 by Giovanni Romano                                 *
*   Giovanni.Romano.76@gmail.com                                          *
*                                                                         *
*   This program may be distributed under the terms of the Q Public       *
*   License as defined by Trolltech AS of Norway and appearing in the     *
*   file LICENSE.QPL included in the packaging of this file.              *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
***************************************************************************/

#include "stdafx.h"
#include "RecogWord.h"
#include "QKeyPushButton.h"
#include "widgetKeyBoard.h"


QKeyPushButton::QKeyPushButton(QWidget *parent) : QPushButton(parent), m_parent(parent), m_weight(-1)
{
	this->setStyleSheet(QString(DEFAULT_STYLE_BUTTON) + QString(DEFAULT_BACKGROUND_BUTTON));
	connect(this, SIGNAL(pressedKey(bool)), SLOT(getKeyPress(bool)));
	if ((static_cast<widgetKeyBoard *> (parent))->isEmbeddedKeyboard() == true)
		this->style_embedded = QString(EMBEDDED_KEYBOARD);
}

void QKeyPushButton::getKeyPress(bool capsStatus)
{
	int 		keyCode = 0;
	QKeyEvent	*key = NULL;
	QString		text = this->text();
	//
	// per tutti i car speciali tranne il CAPS (RETURN, ALT, SHIFT, ecc...) inoltra al componente widgetKeyBoard:
	if (NO_SPECIAL_KEY(text) == false && (IS_BACK(text) == true || IS_BACK_EMB(text) == true || IS_TAB(text) == true || IS_RETURN(text) == true || IS_CTRL_LEFT(text) == true ||
		IS_ALT(text) == true || IS_CANC(text) == true || IS_CUT_LEFT(text) == true || IS_PASSWORD(text) || IS_PASSWORD_EMB(text) || IS_PASTE(text) || IS_COPY(text)))
		key = new QKeyEvent(QEvent::KeyPress, keyCode, Qt::NoModifier, text);
	else { // trattasi di caratteri stampabili
		keyCode = text.toAscii()[0]; // prende il valore numerico (sempre maiuscolo)
		if (capsStatus == false) { // se deve prendere minuscolo, controlla se il carattere ?alfabetico
			if (keyCode >= tr("A")[0] && keyCode <= tr("Z")[0]) {
				keyCode += 32; // carattere piccolo
				text = (char ) keyCode; // carattere piccolo
			}
			key = new QKeyEvent(QEvent::KeyPress, keyCode, Qt::NoModifier, text);
		}
		else
			key = new QKeyEvent(QEvent::KeyPress, keyCode, Qt::ShiftModifier, text);
	}
	((widgetKeyBoard *) this->m_parent)->receiptChildKey(key, NULL);
	QCoreApplication::processEvents();
}


void QKeyPushButton::mousePressEvent(QMouseEvent *event)
{
	widgetKeyBoard  *tmpKeyBoard = (widgetKeyBoard *) this->m_parent;

	//put the first character when press the mouse button
	tmpKeyBoard->putTrace(TraceObject(text()));

	//set style to pressed
	setPressedStyle();
	
	QWidget::mousePressEvent(event);
}

void QKeyPushButton::mouseReleaseEvent(QMouseEvent *event)
{
	widgetKeyBoard  *tmpKeyBoard = (widgetKeyBoard *) this->m_parent;

	if (tmpKeyBoard->getTrace().size() == 1){//restore this key's style sheet, if we only input one letter
		setDefaultStyle();
		emit pressedKey(false);//output the single key
	}
	else {		
		//restore last key's style to default, if we input a serious of letter
		TraceObject& traceObj = tmpKeyBoard->peekTrace();

		QKeyPushButton* lastKey = tmpKeyBoard->findKey(traceObj.getCharcter());
		//set the weight for the last key
		traceObj.setWeight(lastKey->getWeight());
		lastKey->setDefaultStyle();

		emit traceFinished();
	}

	//clear the trace and ready for the next input
	tmpKeyBoard->clearTrace();

	QWidget::mousePressEvent(event);
}

void QKeyPushButton::mouseMoveEvent( QMouseEvent * event )
{
	widgetKeyBoard  *tmpKeyBoard = (widgetKeyBoard *) this->m_parent;
	QWidget* currentWig = QApplication::widgetAt (event->globalX(), event->globalY());

	QKeyPushButton* currentKey = dynamic_cast<QKeyPushButton*>(currentWig);

	if (currentKey){
		currentKey->increaseWeight();
		if (currentKey->text().compare(tmpKeyBoard->peekTrace().getCharcter(), Qt::CaseInsensitive) != 0){
			//restore the last key to default style
			QKeyPushButton* lastKey = tmpKeyBoard->findKey(tmpKeyBoard->peekTrace().getCharcter());
			//set the weight
			tmpKeyBoard->peekTrace().setWeight(lastKey->getWeight());
			lastKey->setDefaultStyle();

			//set current key to pressed style
			tmpKeyBoard->putTrace(TraceObject(currentKey->text()));
			currentKey->setPressedStyle();
		}
	}

	QWidget::mousePressEvent(event);
}

void QKeyPushButton::setPressedStyle()
{
	m_weight = 0;
	setStyleSheet(QString(DEFAULT_STYLE_BUTTON) + QString(CHANGED_BACKGROUND_BUTTON));
}

void QKeyPushButton::setDefaultStyle()
{
	m_weight = -1;
	setStyleSheet(QString(DEFAULT_STYLE_BUTTON) + QString(DEFAULT_BACKGROUND_BUTTON));
}

int QKeyPushButton::getWeight()
{
	return m_weight;
}

void QKeyPushButton::increaseWeight()
{
	++m_weight;
}