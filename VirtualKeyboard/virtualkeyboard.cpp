#include "stdafx.h"
#include "virtualkeyboard.h"

#include "widgetKeyBoard.h"
#include "EditDistance.h"

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
{}

//////////////////////////////////////////////////////////////////////////
#include <vector>
#include <algorithm>
#include <cassert>
//#include <string>

using namespace std;

class HtmlTags
{
public:
	typedef std::vector<QString> QStrings;

	static QString toString(const QStrings& strs){
		QString ret;
		for_each(strs.begin(), strs.end(),[&ret](QString str){
			ret += str;
		});

		return ret;
	}

	QStrings prefix;
	QStrings suffix;
};

static const QString insPrefix = "<font color=\"green\">";
static const QString insSuffix = "</font>";
static const QString delPrefix = "<s>";
static const QString delSuffix = "</s>";
static const QString repPrefix = "<font color=\"red\">";
static const QString repSuffix = "</font>";
//////////////////////////////////////////////////////////////////////////

void VirtualKeyboard::slotOnUserSelection( QString text )
{
	QString echoTxt = ui.echoLE->text();
	QString inputTxt = ui.inputLE->text() + text;
	ui.inputLE->setText(inputTxt);

	EditDistance::DistanceResult res = EditDistance::calc(inputTxt.toUpper().toStdString(), echoTxt.toUpper().toStdString());
	
	if (res.distance == 0){
		ui.resultTB->setHtml("<font size=5 color=\"green\">Perfect Match!</font>");
	}else{
		std::vector<HtmlTags> tags(inputTxt.size());

		for_each(res.route.begin(), res.route.end(), [&tags](const EditDistance::OperationObject& oo){
			switch(oo.opTy){
			case EditDistance::Insertion:
				tags[oo.srcOffset].suffix.push_back(insPrefix + oo.tgtChar + insSuffix);
				break;
			case EditDistance::Deletion:
				tags[oo.srcOffset].prefix.push_back(delPrefix);
				tags[oo.srcOffset].suffix.push_back(delSuffix);
				break;
			case EditDistance::Replacment:
				tags[oo.srcOffset].prefix.push_back(delPrefix);
				tags[oo.srcOffset].suffix.push_back(delSuffix);
				tags[oo.srcOffset].suffix.push_back(insPrefix + oo.tgtChar + insSuffix);
				break;
			default:
				assert(0);
			}
		});

		QString finalOutput = "<font size= 5>";
		QString stepsStr = QString::number(res.route.size());
		if(res.distance == 1)
			finalOutput += "Need "+stepsStr + " step(insert, replace or delete) to make them same!<br/>";
		else
			finalOutput += "Need "+stepsStr + " steps(insert, replace or delete) to make them same!<br/>";

		for (auto i = 0; i < inputTxt.size(); ++i){
			QString prefix = HtmlTags::toString(tags[i].prefix);
			QString tc = inputTxt.at(i);
			QString suffix = HtmlTags::toString(tags[i].suffix);

			finalOutput += prefix + tc + suffix;
		}

		finalOutput += "</font>";

		ui.resultTB->setHtml(finalOutput);
	}

}
