#include <map>
#include <string>
#include <functional>

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
bool hanging_neg = false;
bool anything_entered = false;
bool last_open_parenth = false;
int open_parenth = 0;


void clearResetFlags(QLabel*& top)
{
    top->setText("0.00");
    entering_number = false;
    hanging_op = false;
    hanging_neg = false;
    anything_entered = false;
    open_parenth = 0;
    last_open_parenth = false;
}

void doCalc(QLabel*& top,
            std::pair<const char, QPushButton*>& bpair, Calculator& c)
{
    if (entering_number)
    {
        QString newtext = c.calculate(top->text().toStdString());
        top->setText(newtext);
    }
}

void opPressed(QLabel*& top,
               std::pair<const char, QPushButton*>& bpair)
{
    if (bpair.first == '-' && !anything_entered && !hanging_neg)
    {
        qInfo() << "no append neg";
        QString newtext(bpair.first);
        top->setText(newtext);
        entering_number = true;
        hanging_neg = true;
        anything_entered = true;
        last_open_parenth = false;
    }
    else if ( !hanging_neg && (hanging_op || !entering_number) )
    {
        qInfo() << "append neg";
        QString newtext = top->text() + bpair.first;
        top->setText(newtext);
        entering_number = true;
        hanging_neg = true;
        anything_entered = true;
        last_open_parenth = false;
    }
    else if ( !hanging_op && ((bpair.first == '-' && !entering_number && !hanging_neg)
                               || (entering_number && !hanging_neg) ))
        // allow unary negative, but not in the middle of a number
    {
        qInfo() << "op";
        QString newtext = top->text() + bpair.first;
        top->setText(newtext);
        entering_number = false;
        hanging_op = true;
        anything_entered = true;
        last_open_parenth = false;
    }
//    if (entering_number)  // op only valid if number precedes
//    {
//        QString newtext = top->text()+ bpair.first;
//        top->setText(newtext);
//    }
//    else if (!entering_number && !hanging_op)
//    {
//        QString newtext = top->text() + bpair.first;
//        top->setText(newtext);    
//    }
    

    
}

void numberPressed(QLabel*& top,
                   std::pair<const char, QPushButton*>& bpair)
{     // i guess map keys are always const, I forgot
    
    if (entering_number || hanging_op || last_open_parenth)
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
    anything_entered = true;
    last_open_parenth = false;
    hanging_neg = false;
}

void openParenthPressed(QLabel*& top,
                   std::pair<const char, QPushButton*>& bpair)
{       // parentheses are valid when there's a hanging op,
        // or when both flags are false
    
    if (anything_entered && (hanging_op ||
            (!entering_number && !hanging_op) || last_open_parenth))
    {
        QString newtext = top->text() + bpair.first;
        top->setText(newtext);
        hanging_op = false;
        entering_number = false;
        last_open_parenth = true;
        hanging_neg = false;
        ++open_parenth;
    }
    else if (!anything_entered)
    {
        top->setText(QString(bpair.first));
        hanging_op = false;
        entering_number = false;
        last_open_parenth = true;
        hanging_neg = false;
        ++open_parenth;      
    }

}

void closeParenthPressed(QLabel*& top,
                        std::pair<const char, QPushButton*>& bpair)
{       // parentheses are valid when there's a hanging op,
    // or when both flags are false
    
    if (open_parenth > 0 && (entering_number || (!entering_number && !hanging_op)))
    {
        QString newtext = top->text() + bpair.first;
        top->setText(newtext);
        hanging_op = false;
        entering_number = true;
        last_open_parenth = false;
        hanging_neg = false;
        --open_parenth;
    }
    
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QMainWindow w;  
    QWidget* bigboss = new QWidget(&w);
    w.setCentralWidget(bigboss);

    QGridLayout* glayout = new QGridLayout(bigboss);
    glayout->setContentsMargins(0,0,0,0);
    glayout->setSpacing(2);
    
    QLabel* toplabel = new QLabel("0.00");
    toplabel->setAlignment(Qt::AlignHCenter);
    QFont font("Times New Roman", 16, QFont::Bold);
    QFont butfont("Times New Roman", 14);
    toplabel->setFont(font);
    glayout->addWidget(toplabel,0,0,1,4);
    
    Calculator bob;
    
    char keys[5][4] = {
        {'e', '(', ')', '/'},
        {'7', '8', '9', '*'},
        {'4', '5', '6', '-'},
        {'1', '2', '3', '+'},
        {'C', '0', '.', '='},
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
    string ops = "+-*e/";
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
        if (butpair.first == '=')
            QObject::connect(butpair.second,
                             &QPushButton::clicked,
                             [&] () mutable { doCalc(toplabel, butpair, bob); });
        if (butpair.first == 'C')
            QObject::connect(butpair.second,
                             &QPushButton::clicked,
                             [&] () mutable { clearResetFlags(toplabel); });
        if (butpair.first == '(')
            QObject::connect(butpair.second,
                             &QPushButton::clicked,
                             [&] () mutable { openParenthPressed(toplabel, butpair); });
        if (butpair.first == ')')
            QObject::connect(butpair.second,
                             &QPushButton::clicked,
                             [&] () mutable { closeParenthPressed(toplabel, butpair); });
    }
    

    w.show();
    return a.exec();
}
