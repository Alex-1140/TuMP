/********************************************************************************
** Form generated from reading UI file 'loginwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINWINDOW_H
#define UI_LOGINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_loginWindow
{
public:
    QWidget *centralwidget;
    QLineEdit *ipLineEdit;
    QLineEdit *portLineEdit;
    QLineEdit *usernameLineEdit;
    QLineEdit *passwordLineEdit;
    QPushButton *connectButton;
    QPushButton *loginButton;
    QPushButton *registerButton;
    QLabel *ipaddresstext;
    QLabel *porttext;
    QLabel *label;
    QLabel *passwordtext;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *loginWindow)
    {
        if (loginWindow->objectName().isEmpty())
            loginWindow->setObjectName("loginWindow");
        loginWindow->resize(329, 164);
        centralwidget = new QWidget(loginWindow);
        centralwidget->setObjectName("centralwidget");
        ipLineEdit = new QLineEdit(centralwidget);
        ipLineEdit->setObjectName("ipLineEdit");
        ipLineEdit->setGeometry(QRect(10, 20, 121, 16));
        portLineEdit = new QLineEdit(centralwidget);
        portLineEdit->setObjectName("portLineEdit");
        portLineEdit->setGeometry(QRect(10, 60, 51, 16));
        usernameLineEdit = new QLineEdit(centralwidget);
        usernameLineEdit->setObjectName("usernameLineEdit");
        usernameLineEdit->setGeometry(QRect(190, 20, 113, 16));
        passwordLineEdit = new QLineEdit(centralwidget);
        passwordLineEdit->setObjectName("passwordLineEdit");
        passwordLineEdit->setGeometry(QRect(190, 50, 113, 16));
        passwordLineEdit->setEchoMode(QLineEdit::EchoMode::Password);
        connectButton = new QPushButton(centralwidget);
        connectButton->setObjectName("connectButton");
        connectButton->setGeometry(QRect(80, 60, 51, 16));
        loginButton = new QPushButton(centralwidget);
        loginButton->setObjectName("loginButton");
        loginButton->setGeometry(QRect(190, 80, 41, 16));
        registerButton = new QPushButton(centralwidget);
        registerButton->setObjectName("registerButton");
        registerButton->setGeometry(QRect(250, 80, 51, 16));
        ipaddresstext = new QLabel(centralwidget);
        ipaddresstext->setObjectName("ipaddresstext");
        ipaddresstext->setGeometry(QRect(10, 0, 71, 16));
        porttext = new QLabel(centralwidget);
        porttext->setObjectName("porttext");
        porttext->setGeometry(QRect(10, 40, 49, 16));
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        label->setGeometry(QRect(190, 0, 49, 16));
        passwordtext = new QLabel(centralwidget);
        passwordtext->setObjectName("passwordtext");
        passwordtext->setGeometry(QRect(190, 30, 51, 20));
        loginWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(loginWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 329, 21));
        loginWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(loginWindow);
        statusbar->setObjectName("statusbar");
        loginWindow->setStatusBar(statusbar);

        retranslateUi(loginWindow);

        QMetaObject::connectSlotsByName(loginWindow);
    } // setupUi

    void retranslateUi(QMainWindow *loginWindow)
    {
        loginWindow->setWindowTitle(QCoreApplication::translate("loginWindow", "MainWindow", nullptr));
        connectButton->setText(QCoreApplication::translate("loginWindow", "Connect", nullptr));
        loginButton->setText(QCoreApplication::translate("loginWindow", "Login", nullptr));
        registerButton->setText(QCoreApplication::translate("loginWindow", "Register", nullptr));
        ipaddresstext->setText(QCoreApplication::translate("loginWindow", "ip address", nullptr));
        porttext->setText(QCoreApplication::translate("loginWindow", "port", nullptr));
        label->setText(QCoreApplication::translate("loginWindow", "login", nullptr));
        passwordtext->setText(QCoreApplication::translate("loginWindow", "password", nullptr));
    } // retranslateUi

};

namespace Ui {
    class loginWindow: public Ui_loginWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINWINDOW_H
