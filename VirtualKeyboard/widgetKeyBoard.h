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


#pragma once

#include <string>

#include <QtGui>

#include "QKeyPushButton.h"

class widgetKeyBoard : public QWidget {
	Q_OBJECT

public:
	widgetKeyBoard(bool embeddedKeyboard = false, QWidget *activeForm = NULL, QWidget *parent = NULL);
	~widgetKeyBoard();

	void createKeyboard(void);
	bool isEmbeddedKeyboard(void);
	void                    soundClick(void);
	void                    setZoomFacility(bool active); // only valid for embedded keyboard
	bool                    isZoomFacilityEnable(void);
	bool                    statusEchoMode(void){return (this->m_echoMode);}
	void                    setStatusEchoMode(bool echo) {this->m_echoMode = echo; }
	void 			receiptChildKey(QKeyEvent *event, QLineEdit *focusThisControl, bool reset = false); // accoglie i tasti premuti
	QLineEdit *             currentTextBox(void) { return (this->m_currentTextBox);}
	void                    switchKeyEchoMode(QLineEdit *control);
	void                    enableSwitchingEcho(bool status); // if you don't want control echo from keyboard
	bool                    isEnabledSwitchingEcho(void); // current status

	QKeyPushButton* findKey(QString keyStr);

public slots:
	void 			show(QWidget *activeForm);
	void 			hide(bool noChangeColor);
	void            focusThis(QLineEdit *control);

	//Yang's methods for trace support
public:
	std::string getTrace(){
		return m_trace;
	}

	void clearTrace(){
		m_trace.clear();
	}

	void putTrace(char chr){
		m_trace.push_back(chr);
	}

	char peekTrace(){
		if (m_trace.empty())
			return -1; // means empty
			//throw std::exception("Trace is empty!");

		return *m_trace.rbegin();
	}

protected:
	void			closeEvent (QCloseEvent * event);

private:
	widgetKeyBoard(const widgetKeyBoard&);
	widgetKeyBoard& operator=(const widgetKeyBoard&);

	void			init_keyboard(QLineEdit *focusThisControl); // reinizializza le funzioni base della tastiera
	QLineEdit *             setCurrentTextStyle(QLineEdit *control);
	QLineEdit *             setDefaultTextStyle(QLineEdit *control);
	QKeyPushButton * 	createNewKey(QString keyValue);
	QLineEdit *		getNextTextbox(QLineEdit *thisControl = NULL, bool reset = false);
	void                    controlKeyEcho(QLineEdit *control);

private:
	QLineEdit		m_noFocusPalette; // usata per ripristinare la linetext senza focus
	QWidget			*m_activeWindow;
	QLineEdit		*m_currentTextBox; // mantiene il riferimento alla casella correntemente in uso                      
	static bool             m_created;
	bool                    m_embeddedKeyboard;
	bool                    m_echoMode; // status of current text object for echo
	bool                    m_zoomFacilityEmbedded;
	bool                    m_enablePasswordEcho; // controls the possibility to change among normal/password echo
	QSound                  m_player;
	QClipboard              *m_clipboard;

	//Yang's method for trace support
private:
	std::string m_trace;
};
