#ifndef OVERLAY_H
#define OVERLAY_H

#include <QtGui/QtGui>

class Overlay : public QWidget
{
	Q_OBJECT

public:
	Overlay(QWidget *parent = 0);
	~Overlay();

	QImage* GetOverlay() const { return overlayImg; }
	void SetOverlay(QImage* val) { overlayImg = val; }

private:
	void paintEvent(QPaintEvent *event);

private:
	QImage* overlayImg;
};

#endif // OVERLAY_H
