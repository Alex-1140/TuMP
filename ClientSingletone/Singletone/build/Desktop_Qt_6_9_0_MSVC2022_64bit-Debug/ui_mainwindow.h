/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLineEdit *commandLineEdit;
    QPushButton *sendButton;
    QPlainTextEdit *logTextEdit;
    QLabel *label;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(208, 212);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        commandLineEdit = new QLineEdit(centralwidget);
        commandLineEdit->setObjectName("commandLineEdit");
        commandLineEdit->setGeometry(QRect(10, 110, 171, 24));
        sendButton = new QPushButton(centralwidget);
        sendButton->setObjectName("sendButton");
        sendButton->setGeometry(QRect(60, 140, 80, 24));
        logTextEdit = new QPlainTextEdit(centralwidget);
        logTextEdit->setObjectName("logTextEdit");
        logTextEdit->setGeometry(QRect(10, 30, 171, 71));
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        label->setGeometry(QRect(8, 10, 51, 20));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 208, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        sendButton->setText(QCoreApplication::translate("MainWindow", "Send", nullptr));
        logTextEdit->setPlainText(QString());
        label->setText(QCoreApplication::translate("MainWindow", "Console", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
