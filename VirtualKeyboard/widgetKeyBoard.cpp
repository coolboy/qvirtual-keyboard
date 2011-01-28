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

#include <vector>
#include <string>
#include <cctype>
#include <algorithm>

#include "widgetKeyBoard.h"
#include "RecogWord.h"

using namespace std;

bool widgetKeyBoard::m_created = false;


widgetKeyBoard::widgetKeyBoard(bool embeddedKeyboard, QWidget *activeForm, QWidget *parent) : QWidget(parent), m_activeWindow(activeForm),
	m_currentTextBox(NULL), m_embeddedKeyboard(embeddedKeyboard), m_echoMode(false), m_zoomFacilityEmbedded(false), m_enablePasswordEcho(false),
	m_player(QDir::currentPath() + CLICK_SOUND), m_clipboard(QApplication::clipboard())
{        
	this->m_clipboard->clear();
	this->setWindowIcon(QPixmap(":/VirtualKeyboard/logo"));

	//
	m_reg = new RecogWord();

	m_context = new QMenu(this);


	//for overlay
	modified = false;
	scribbling = false;
	myPenWidth = 4;
	myPenColor = Qt::red;

}

widgetKeyBoard::~widgetKeyBoard()
{
	delete m_reg;
}


void widgetKeyBoard::enableSwitchingEcho(bool status)
{
	this->m_enablePasswordEcho = status;
}

bool widgetKeyBoard::isEnabledSwitchingEcho(void)
{
	return (this->m_enablePasswordEcho);
}

inline bool widgetKeyBoard::isEmbeddedKeyboard(void)
{
	return (this->m_embeddedKeyboard);
}

void widgetKeyBoard::setZoomFacility(bool active)
{
	this->m_zoomFacilityEmbedded = active;
}

bool widgetKeyBoard::isZoomFacilityEnable(void)
{
	return (this->m_zoomFacilityEmbedded);
}

QKeyPushButton * widgetKeyBoard::createNewKey(QString keyValue)
{
	QKeyPushButton *tmp = new QKeyPushButton(this);
	QString        style = QString(DEFAULT_STYLE_BUTTON) + QString(DEFAULT_BACKGROUND_BUTTON);
	QSize          imageDim;
	int            width = 0, height = 0;

	if (this->isEmbeddedKeyboard() == true) {
		imageDim.setWidth(16);
		imageDim.setHeight(16);
	}
	else {
		imageDim.setWidth(32);
		imageDim.setHeight(32);
	}
	if (IS_PASSWORD(keyValue) == true || IS_PASSWORD_EMB(keyValue) == true) {
		tmp->setIconSize(imageDim);
		tmp->setIcon(QPixmap(":/VirtualKeyboard/normalEcho"));            
	}        
	else if (IS_PASTE(keyValue) == true) {
		tmp->setIconSize(imageDim);
		tmp->setEnabled(false);
		tmp->setIcon(QPixmap(":/VirtualKeyboard/paste"));
	}
	else if (IS_COPY(keyValue) == true) {
		tmp->setIconSize(imageDim);
		tmp->setIcon(QPixmap(":/VirtualKeyboard/copy"));
	}
	else if (IS_CUT_LEFT(keyValue) == true) {
		tmp->setIconSize(imageDim);
		tmp->setIcon(QPixmap(":/VirtualKeyboard/cut"));
	}
	tmp->setText(keyValue);
	if (this->isEmbeddedKeyboard() == true) {
		width = KEY_WIDTH_EMBEDDED;
		height = KEY_HEIGHT_EMBEDDED;
		style += QString(EMBEDDED_KEYBOARD);
	}
	else {
		width = 54;
		height = 40;
	}
	tmp->setObjectName(keyValue);
	tmp->setMinimumSize(width, height);
	tmp->setMaximumSize(width, height);
	tmp->setStyleSheet(style);
	tmp->setVisible(true);

	connect (tmp, SIGNAL(traceFinished()), this, SLOT(onTraceFinish()));

	return (tmp);
}

void widgetKeyBoard::controlKeyEcho(QLineEdit *control)
{
	QString         textToSearch;
	QKeyPushButton  *echoControlKey;
	QString         tmpStyle;
	QPushButton     *pasteButton = this->findChild<QPushButton *>(KEY_PASTE);
	QPushButton     *cutButton = this->findChild<QPushButton *>(KEY_CUT_LEFT);
	QPushButton     *copyButton = this->findChild<QPushButton *>(KEY_COPY);

	if (this->isEmbeddedKeyboard()) {
		textToSearch = KEY_HIDECHAR_EMBEDDED;
		tmpStyle = QString(EMBEDDED_KEYBOARD);
	}
	else
		textToSearch = KEY_HIDECHAR;
	if (pasteButton != NULL && copyButton != NULL) { // support for copy/paste functionality
		if (control->echoMode() == QLineEdit::Normal) {
			cutButton->setEnabled(true);
			copyButton->setEnabled(true);
			if (this->m_clipboard->text().length() > 0)
				pasteButton->setEnabled(true);
			else
				pasteButton->setEnabled(false);
		}
		else if (control->echoMode() == QLineEdit::Password) {
			copyButton->setEnabled(false);
			cutButton->setEnabled(false);
			pasteButton->setEnabled(false);
		}
	}
	echoControlKey = this->findChild<QKeyPushButton *>(textToSearch);
	if (echoControlKey != NULL) { // support for password echo functionality            
		if (control->echoMode() == QLineEdit::Normal) {            
			echoControlKey->setStyleSheet(QString(DEFAULT_STYLE_BUTTON) + QString(DEFAULT_BACKGROUND_BUTTON) +  tmpStyle); // segnalato come attivo
			echoControlKey->setIcon(QPixmap(":/VirtualKeyboard/normalEcho"));
		}
		else if (control->echoMode() == QLineEdit::Password) {            
			echoControlKey->setStyleSheet(QString(DEFAULT_STYLE_BUTTON) + QString(CHANGED_BACKGROUND_BUTTON) + tmpStyle); // segnalato come attivo
			echoControlKey->setIcon(QPixmap(":/VirtualKeyboard/passwdEcho"));
		}
		echoControlKey->repaint();
		QApplication::processEvents();
	}
}

//
// riceve i caratteri che sono stati digitati:
void widgetKeyBoard::receiptChildKey(QKeyEvent *event, QLineEdit *focusThisControl, bool reset)
{
	static QLineEdit    *nextInput = NULL;

	if (reset == true) { // reinizializza il controllo
		nextInput = this->getNextTextbox(focusThisControl, true);
		return;
	}        
	if (nextInput == NULL)
		return;        
	//
	// inizia l'analisi del carattere ricevuto:
	QString	newKey = event->text();
	QString tmpReceiptString = nextInput->text();
	int     tmpPos = nextInput->cursorPosition();

	if (NO_SPECIAL_KEY(newKey) == false) {
		if (IS_RETURN(newKey) == true || IS_TAB(newKey) == true) { // trattasi di TAB, si sposta alla text successiva
			nextInput = this->setDefaultTextStyle(nextInput);
			nextInput->deselect();
			nextInput = this->getNextTextbox();
			this->controlKeyEcho(nextInput); // status of key echo here
			if (nextInput != NULL) {
				nextInput->setCursorPosition(nextInput->text().length()); // comment this line if you want caret position at current char inserted
				nextInput->setFocus(Qt::TabFocusReason);
			}
		}
		else if (IS_BACK(newKey) == true || IS_BACK_EMB(newKey) == true) { // trattasi di CANCELLARE carattere, elimina il carattere a sinistra
			if (tmpPos-1 >= 0) {
				tmpReceiptString = tmpReceiptString.remove(tmpPos-1, 1);
				nextInput->setText(tmpReceiptString);
				nextInput->setCursorPosition(tmpPos-1);
				nextInput->setSelection(tmpPos-2, 1);
			}
		}
		else if (IS_CANC(newKey) == true) { // trattasi di CANC carattere, elimina il carattere a destra
			tmpReceiptString = tmpReceiptString.remove(tmpPos, 1);
			nextInput->setText(tmpReceiptString);
			nextInput->setCursorPosition(tmpPos);
			nextInput->setSelection(tmpPos-1, 1);
		}
		else if (IS_COPY(newKey) == true || IS_CUT_LEFT(newKey) == true) {
			QPushButton *button = this->findChild<QPushButton *>(KEY_PASTE);

			if (button != NULL) {
				if (nextInput->text().length() != 0) {
					this->m_clipboard->setText(nextInput->text());
					if (IS_CUT_LEFT(newKey) == true)
						nextInput->setText("");
					button->setEnabled(true);
				}
				else
					button->setEnabled(false);
			}
		}
		else if (IS_PASTE(newKey) == true)
			nextInput->setText(this->m_clipboard->text());
		else if (IS_ALT(newKey) == true || IS_CTRL_LEFT(newKey) == true)
			; // non esegue nessuna operazione
	}
	else { // si tratta di un carattere da visualizzare nella casella di testo corrente
		tmpReceiptString = tmpReceiptString.insert(tmpPos, newKey);
		nextInput->setText(tmpReceiptString);
		//nextInput->setCursorPosition(tmpPos+1);
		//nextInput->setSelection(tmpPos, 1);
	}
}


void widgetKeyBoard::switchKeyEchoMode(QLineEdit *control)
{
	this->setStatusEchoMode(!this->statusEchoMode());
	if (this->statusEchoMode() == true)
		control->setEchoMode(QLineEdit::Password);
	else
		control->setEchoMode(QLineEdit::Normal);
	this->controlKeyEcho(control);
}


QLineEdit * widgetKeyBoard::getNextTextbox(QLineEdit *thisControl, bool reset)
{
	static QWidget	*nextInput = NULL;
	QLineEdit	*tmpInputFound = NULL;

	this->m_currentTextBox = NULL;
	if (this->m_activeWindow == NULL) { // nessuna finestra principale su cui operare
		nextInput = NULL;
		return (NULL);
	}
	if (thisControl != NULL) {
		if (this->m_activeWindow->findChild<QLineEdit *>(thisControl->objectName()) == 0) // verifies that thisControl is a child of m_activeWindow
			return (NULL);
		else // it's true!
			return (this->setCurrentTextStyle(thisControl));
	}
	//
	// cicla nella catena dei controlli in ordine di focus per ottenere la QEditLine
	if (nextInput == NULL || reset == true)
		nextInput = this->m_activeWindow->nextInFocusChain();
	else
		nextInput = nextInput->nextInFocusChain();
	do {
		if (nextInput->inherits("QLineEdit") == true) { // trovata una casella di testo da utilizzare
			tmpInputFound = dynamic_cast<QLineEdit *>(nextInput);                        
			return (this->setCurrentTextStyle(tmpInputFound));
		}
		nextInput = nextInput->nextInFocusChain();
	} while (nextInput != NULL);
	return (NULL);
}


QLineEdit * widgetKeyBoard::setCurrentTextStyle(QLineEdit *control)
{
	this->m_currentTextBox = control;
	control->setStyleSheet(STYLE_FOCUS_TEXT);
	return (control);
}

QLineEdit * widgetKeyBoard::setDefaultTextStyle(QLineEdit *control)
{
	control->setStyleSheet(this->m_noFocusPalette.styleSheet());
	return (control);
}

void widgetKeyBoard::init_keyboard(QLineEdit *focusThisControl)
{
	this->m_currentTextBox = NULL;
	this->receiptChildKey(NULL, focusThisControl, true);
}

void widgetKeyBoard::soundClick(void)
{
	this->m_player.play();
}

void widgetKeyBoard::show(QWidget *activeForm)
{
	this->m_activeWindow = activeForm;
	this->init_keyboard(NULL);
	if (this->windowState() == Qt::WindowMinimized)
		this->setWindowState(Qt::WindowNoState);
	this->setStatusEchoMode(false);
	this->m_clipboard->clear();
	this->move(20, QApplication::desktop()->screenGeometry().height() - this->height() - 200);
	this->setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint | Qt::MSWindowsFixedSizeDialogHint);
	QWidget::show();
}


void widgetKeyBoard::focusThis(QLineEdit *control)
{
	if (this->m_activeWindow != NULL) {
		if (this->m_currentTextBox != NULL)
			this->m_currentTextBox = this->setDefaultTextStyle(this->m_currentTextBox);
		this->init_keyboard(control);
		this->controlKeyEcho(control);
	}
	else
		;
}

void widgetKeyBoard::closeEvent(QCloseEvent * event)
{
	event->ignore();
}


void widgetKeyBoard::hide(bool changeColor)
{
	try {
		if (changeColor == true)
			if (this->m_activeWindow != NULL && this->m_currentTextBox != NULL)
				this->m_currentTextBox = this->setDefaultTextStyle(this->m_currentTextBox);
		this->m_currentTextBox = NULL;
		this->m_activeWindow = NULL;
		setVisible(false);
	}
	catch (...) {
	}
}


void widgetKeyBoard::createKeyboard(void)
{
	QVBoxLayout* baseLayout = new QVBoxLayout();
	baseLayout->setSpacing(0);
	baseLayout->setMargin(0);

	QHBoxLayout* h1Layout = new QHBoxLayout();
	h1Layout->setSpacing(0);
	h1Layout->setMargin(0);
	h1Layout->addWidget(createNewKey(tr("Q")));
	h1Layout->addWidget(createNewKey(tr("W")));
	h1Layout->addWidget(createNewKey(tr("E")));
	h1Layout->addWidget(createNewKey(tr("R")));
	h1Layout->addWidget(createNewKey(tr("T")));
	h1Layout->addWidget(createNewKey(tr("Y")));
	h1Layout->addWidget(createNewKey(tr("U")));
	h1Layout->addWidget(createNewKey(tr("I")));
	h1Layout->addWidget(createNewKey(tr("O")));
	h1Layout->addWidget(createNewKey(tr("P")));
	h1Layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ));

	baseLayout->addLayout(h1Layout);

	QHBoxLayout* h2Layout = new QHBoxLayout();
	h2Layout->setSpacing(0);
	h2Layout->setMargin(0);
	h2Layout->addWidget(createNewKey(tr("A")));
	h2Layout->addWidget(createNewKey(tr("S")));
	h2Layout->addWidget(createNewKey(tr("D")));
	h2Layout->addWidget(createNewKey(tr("F")));
	h2Layout->addWidget(createNewKey(tr("G")));
	h2Layout->addWidget(createNewKey(tr("H")));
	h2Layout->addWidget(createNewKey(tr("J")));
	h2Layout->addWidget(createNewKey(tr("K")));
	h2Layout->addWidget(createNewKey(tr("L")));
	h2Layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ));

	baseLayout->addLayout(h2Layout);

	QHBoxLayout* h3Layout = new QHBoxLayout();

	h3Layout->setSpacing(0);
	h3Layout->setMargin(0);

	h3Layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ));
	h3Layout->addWidget(createNewKey(tr("Z")));
	h3Layout->addWidget(createNewKey(tr("X")));
	h3Layout->addWidget(createNewKey(tr("C")));
	h3Layout->addWidget(createNewKey(tr("V")));
	h3Layout->addWidget(createNewKey(tr("B")));
	h3Layout->addWidget(createNewKey(tr("N")));
	h3Layout->addWidget(createNewKey(tr("M")));
	h3Layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ));
	
	QString tmpStyle;
	if (this->isEmbeddedKeyboard() == true)
		tmpStyle = QString(KEY_BACKSPACE_EMBEDDED);
	else
		tmpStyle = QString(KEY_BACKSPACE);
	QKeyPushButton* bsKey = createNewKey(tmpStyle);
	bsKey->setMaximumWidth(bsKey->maximumWidth() * 2);
	bsKey->setMinimumWidth(bsKey->minimumWidth() * 2);
	h3Layout->addWidget(bsKey);

	baseLayout->addLayout(h3Layout);


	QHBoxLayout* h4Layout = new QHBoxLayout();
	QKeyPushButton* spKey = createNewKey(KEY_SPACE);
	spKey->setMaximumWidth(spKey->maximumWidth() * 11);
	spKey->setMinimumWidth(spKey->minimumWidth() * 11);
	h4Layout->addWidget(spKey);
	baseLayout->addLayout(h4Layout);

	baseLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ));

	this->setLayout(baseLayout);
	this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	//create overlay, must create at last to make sure it at bottom
	overlay = new Overlay(this);
	overlay->SetOverlay(&overlayImg);
}

QKeyPushButton* widgetKeyBoard::findKey( QString keyStr )
{
	QKeyPushButton* ret = NULL;
	QObjectList lst = children();

	std::for_each(lst.begin(), lst.end(), [&ret, keyStr](QObject* obj){
		QKeyPushButton* key = dynamic_cast<QKeyPushButton*>(obj);
		if (key == NULL)
			return;

		if (key->text().compare(keyStr, Qt::CaseInsensitive) == 0)
			ret = key;
	});

	return ret;
}

void widgetKeyBoard::mousePressEvent( QMouseEvent *event )
{
	if (event->button() == Qt::LeftButton) {
		lastPoint = event->pos();
		scribbling = true;
	}

	QWidget::mousePressEvent(event);
}

void widgetKeyBoard::mouseReleaseEvent(QMouseEvent *event )
{
	if (event->button() == Qt::LeftButton && scribbling) {
		clearOverlay();

		//user input size is less 2, no feedback
		if(this->m_selection.size() < 2)
			return;

		//1. get last user selection
		QString str = this->m_selection;;
		if(this->m_selection.at(0) == ' ')
			str = str.right(str.size()-1);

		vector<QPoint> inputPoint;

		for(int i = 0; i < str.size(); i++){
			QString character;
			character.push_back(str.at(i));
			int x = this->findKey(character)->geometry().x() + 30;
			int y = this->findKey(character)->geometry().y() + 22;
			inputPoint.push_back(QPoint(x,y));
		}

		//drawLineTo()
		QPainter painter(&overlayImg);
		painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));


		for(vector<int>::size_type i = 1; i < inputPoint.size(); i++){
				painter.drawLine(inputPoint[i-1], inputPoint[i]);
		}

			modified = true;
			overlay->update();
			QTimer::singleShot(300, this, SLOT(clearOverlay()));
			inputPoint.clear();

		//int rad = (myPenWidth / 2) + 2;
		//lastPoint = endPoint;

		scribbling = false;
	}

	QWidget::mouseReleaseEvent(event);
}

void widgetKeyBoard::mouseMoveEvent( QMouseEvent * event )
{
	if ((event->buttons() & Qt::LeftButton) && scribbling){
		drawLineTo(event->pos());
	}

	QWidget::mouseMoveEvent(event);
}

void widgetKeyBoard::resizeEvent( QResizeEvent *event )
{
	if (width() > overlayImg.width() || height() > overlayImg.height()) {
		int newWidth = qMax(width() + 128, overlayImg.width());
		int newHeight = qMax(height() + 128, overlayImg.height());
		resizeImage(&overlayImg, QSize(newWidth, newHeight));
	}

	overlay->resize(event->size());

	QWidget::resizeEvent(event);
}

void widgetKeyBoard::drawLineTo( const QPoint &endPoint )
{
	QPainter painter(&overlayImg);
	painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap,
		Qt::RoundJoin));
	painter.drawLine(lastPoint, endPoint);
	modified = true;

	int rad = (myPenWidth / 2) + 2;
	lastPoint = endPoint;

	overlay->update();
}

void widgetKeyBoard::resizeImage( QImage *image, const QSize &newSize )
{
	if (image->size() == newSize)
		return;

	QImage newImage(newSize, QImage::Format_ARGB32);
	newImage.fill(Qt::transparent);
	QPainter painter(&newImage);
	painter.drawImage(QPoint(0, 0), *image);
	*image = newImage;
}

void widgetKeyBoard::clearOverlay()
{
	overlayImg.fill(Qt::transparent);
	modified = true;
	update();
}

void widgetKeyBoard::onTraceFinish()
{
	m_context->clear();

	vector<string> hints = m_reg->RecogWordVector(m_trace);
	//make it upper case
	for_each(hints.begin(), hints.end(), [](string& hint){
		std::transform(hint.begin(), hint.end(), hint.begin(), toupper);

	});

	//just display it when we only have one hint
	if (hints.size() == 1){
		QAction* act = new QAction(QString::fromStdString(hints[0]), 0);
		connect(act, SIGNAL(triggered ( bool)), this, SLOT(onUserHintSelection()));
		act->trigger();
		act->deleteLater();
		this->m_selection = QString::fromStdString(hints[0]);
	}else{//display the menu to make user choice
		for_each(hints.begin(), hints.end(), [&] (string hint){
			auto act = m_context->addAction(QString::fromStdString(hint));
			connect(act, SIGNAL(triggered ( bool)), this, SLOT(onUserHintSelection()));
		});

		m_context->exec(QCursor::pos());
	}
}

void widgetKeyBoard::onUserHintSelection()
{
	QString text = ((QAction*)sender())->text();

	/*emit*/ sigOnUserSelection(text);

	this->m_selection = text;
}
