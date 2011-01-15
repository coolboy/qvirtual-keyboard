/********************************************************************************
** Form generated from reading UI file 'virtualkeyboard.ui'
**
** Created: Sat Jan 15 01:04:43 2011
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIRTUALKEYBOARD_H
#define UI_VIRTUALKEYBOARD_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLineEdit>
#include <QtGui/QTextBrowser>

QT_BEGIN_NAMESPACE

class Ui_VirtualKeyboardClass
{
public:
    QGridLayout *gridLayout;
    QLineEdit *echoLE;
    QLineEdit *inputLE;
    QTextBrowser *dbgOutput;

    void setupUi(QDialog *VirtualKeyboardClass)
    {
        if (VirtualKeyboardClass->objectName().isEmpty())
            VirtualKeyboardClass->setObjectName(QString::fromUtf8("VirtualKeyboardClass"));
        VirtualKeyboardClass->resize(600, 400);
        gridLayout = new QGridLayout(VirtualKeyboardClass);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        echoLE = new QLineEdit(VirtualKeyboardClass);
        echoLE->setObjectName(QString::fromUtf8("echoLE"));

        gridLayout->addWidget(echoLE, 0, 0, 1, 1);

        inputLE = new QLineEdit(VirtualKeyboardClass);
        inputLE->setObjectName(QString::fromUtf8("inputLE"));

        gridLayout->addWidget(inputLE, 1, 0, 1, 1);

        dbgOutput = new QTextBrowser(VirtualKeyboardClass);
        dbgOutput->setObjectName(QString::fromUtf8("dbgOutput"));

        gridLayout->addWidget(dbgOutput, 2, 0, 1, 1);


        retranslateUi(VirtualKeyboardClass);

        QMetaObject::connectSlotsByName(VirtualKeyboardClass);
    } // setupUi

    void retranslateUi(QDialog *VirtualKeyboardClass)
    {
        VirtualKeyboardClass->setWindowTitle(QApplication::translate("VirtualKeyboardClass", "VirtualKeyboard", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class VirtualKeyboardClass: public Ui_VirtualKeyboardClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIRTUALKEYBOARD_H
