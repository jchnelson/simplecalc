#include <map>
#include <string>
#include <functional>
#include <cmath>

#include <QApplication>
#include <QMainWindow>
#include <QGridLayout>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QFont>
#include <QDebug>
#include <QString>

#include "calculator.h"



using std::string;
using std::function;

bool entering_number = false;
bool hanging_op = false;

void opPressed(QLabel*& top,
               std::pair<const char, QPushButton*>& bpair)
{
    if (entering_number)  // op only valid if number precedes
    {
        QString newtext = top->text() + ' ' + bpair.first + ' ';
        top->setText(newtext);
    }
    else if (!entering_number && !hanging_op)
    {
        QString newtext = top->text() + ' ' + bpair.first + ' ';
        top->setText(newtext);    
    }
    entering_number = false;
    hanging_op = true;
    
}

void numberPressed(QLabel*& top,
                   std::pair<const char, QPushButton*>& bpair)
{     // i guess map keys are always const, I forgot
    
    if (entering_number || hanging_op)
    {
        QString newtext = top->text() + bpair.first;
        top->setText(newtext);
        hanging_op = false;
    }
    else
    {
        top->setText(QString(bpair.first));
    }
    entering_number = true;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QMainWindow w;
    
    
    QWidget* bigboss = new QWidget(&w);
    w.setCentralWidget(bigboss);

    
    Calculator bob;
    
    QGridLayout* glayout = new QGridLayout(bigboss);
    glayout->setContentsMargins(0,0,0,0);
    glayout->setSpacing(2);
    
    qInfo() << bob.calculate("23.4 * 5");
    
    
    QLabel* toplabel = new QLabel("0.00");
    toplabel->setAlignment(Qt::AlignHCenter);
    QFont font("Times New Roman", 16, QFont::Bold);
    QFont butfont("Times New Roman", 14);
    toplabel->setFont(font);
    glayout->addWidget(toplabel,0,0,1,4);
    
    char keys[5][4] = {
        {'e', ' ', ' ', '/'},
        {'7', '8', '9', '*'},
        {'4', '5', '6', '-'},
        {'1', '2', '3', '+'},
        {' ', '0', '.', '='},
        };
    
    std::map<const char, QPushButton*> buttons;
    
    for (int i = 0; i != 5; ++i)
    {
        for (int j = 0; j != 4; ++j)
        {
            QPushButton* but = new QPushButton(QString(keys[i][j]));
            but->setMaximumWidth(60);
            but->setMinimumHeight(60);
            but->setFont(butfont);
            buttons.insert({keys[i][j], but});
            glayout->addWidget(but, i+1, j);       
        }
    }
    string ops = "+-*^/";
    for (auto& butpair : buttons)
    {
        if (isdigit(butpair.first) || butpair.first == '.')
            QObject::connect(butpair.second,
                             &QPushButton::clicked,
                             [&] () mutable { numberPressed(toplabel, butpair); });
        if (ops.find(butpair.first) != string::npos)
            QObject::connect(butpair.second,
                             &QPushButton::clicked,
                             [&] () mutable { opPressed(toplabel, butpair); });
    }  

    w.show();
    return a.exec();
}


