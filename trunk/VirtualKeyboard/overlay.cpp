#include "StdAfx.h"
#include "overlay.h"

Overlay::Overlay(QWidget *parent)
	: QWidget(parent), overlayImg(0)
{
	setPalette(Qt::transparent);
	setAttribute(Qt::WA_TransparentForMouseEvents);
}

Overlay::~Overlay()
{

}

void Overlay::paintEvent( QPaintEvent *event )
{
	QPainter painter(this);

	painter.setRenderHint(QPainter::Antialiasing);
	QRect dirtyRect = event->rect();
	painter.drawImage(dirtyRect, *overlayImg, dirtyRect);

	QWidget::paintEvent(event);
}
