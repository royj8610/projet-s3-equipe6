/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "qchartview.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_7;
    QSpacerItem *horizontalSpacer;
    QLabel *totalTimeLabel;
    QLabel *numTreeLabel;
    QSpacerItem *horizontalSpacer_2;
    QChartView *graphB;
    QLabel *moitieLabelB;
    QLabel *moitieLabelA;
    QHBoxLayout *horizontalLayout_6;
    QVBoxLayout *verticalLayout;
    QLabel *stateLabelA;
    QLabel *cycleTimeLabelA;
    QSpacerItem *verticalSpacer_2;
    QVBoxLayout *verticalLayout_2;
    QPushButton *startButton;
    QPushButton *stopButton;
    QPushButton *resetButton;
    QVBoxLayout *verticalLayout_3;
    QLabel *stateLabelB;
    QLabel *cycleTimeLabelB;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_5;
    QComboBox *comboBoxPort;
    QHBoxLayout *horizontalLayout_9;
    QChartView *graphA;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(800, 556);
        MainWindow->setAcceptDrops(false);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer);

        totalTimeLabel = new QLabel(centralWidget);
        totalTimeLabel->setObjectName(QString::fromUtf8("totalTimeLabel"));

        horizontalLayout_7->addWidget(totalTimeLabel);

        numTreeLabel = new QLabel(centralWidget);
        numTreeLabel->setObjectName(QString::fromUtf8("numTreeLabel"));

        horizontalLayout_7->addWidget(numTreeLabel);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_2);


        gridLayout->addLayout(horizontalLayout_7, 0, 0, 1, 2);

        graphB = new QChartView(centralWidget);
        graphB->setObjectName(QString::fromUtf8("graphB"));

        gridLayout->addWidget(graphB, 3, 1, 1, 1);

        moitieLabelB = new QLabel(centralWidget);
        moitieLabelB->setObjectName(QString::fromUtf8("moitieLabelB"));

        gridLayout->addWidget(moitieLabelB, 2, 1, 1, 1);

        moitieLabelA = new QLabel(centralWidget);
        moitieLabelA->setObjectName(QString::fromUtf8("moitieLabelA"));

        gridLayout->addWidget(moitieLabelA, 2, 0, 1, 1);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        stateLabelA = new QLabel(centralWidget);
        stateLabelA->setObjectName(QString::fromUtf8("stateLabelA"));
        stateLabelA->setLayoutDirection(Qt::LeftToRight);

        verticalLayout->addWidget(stateLabelA);

        cycleTimeLabelA = new QLabel(centralWidget);
        cycleTimeLabelA->setObjectName(QString::fromUtf8("cycleTimeLabelA"));

        verticalLayout->addWidget(cycleTimeLabelA);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);


        horizontalLayout_6->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        startButton = new QPushButton(centralWidget);
        startButton->setObjectName(QString::fromUtf8("startButton"));

        verticalLayout_2->addWidget(startButton);

        stopButton = new QPushButton(centralWidget);
        stopButton->setObjectName(QString::fromUtf8("stopButton"));

        verticalLayout_2->addWidget(stopButton);

        resetButton = new QPushButton(centralWidget);
        resetButton->setObjectName(QString::fromUtf8("resetButton"));

        verticalLayout_2->addWidget(resetButton);


        horizontalLayout_6->addLayout(verticalLayout_2);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        stateLabelB = new QLabel(centralWidget);
        stateLabelB->setObjectName(QString::fromUtf8("stateLabelB"));

        verticalLayout_3->addWidget(stateLabelB, 0, Qt::AlignRight);

        cycleTimeLabelB = new QLabel(centralWidget);
        cycleTimeLabelB->setObjectName(QString::fromUtf8("cycleTimeLabelB"));

        verticalLayout_3->addWidget(cycleTimeLabelB, 0, Qt::AlignRight);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);


        horizontalLayout_6->addLayout(verticalLayout_3);


        gridLayout->addLayout(horizontalLayout_6, 12, 0, 2, 2);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        horizontalLayout_8->addWidget(label_5, 0, Qt::AlignLeft);

        comboBoxPort = new QComboBox(centralWidget);
        comboBoxPort->setObjectName(QString::fromUtf8("comboBoxPort"));

        horizontalLayout_8->addWidget(comboBoxPort);

        horizontalLayout_8->setStretch(1, 1);

        gridLayout->addLayout(horizontalLayout_8, 16, 0, 1, 1);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));

        gridLayout->addLayout(horizontalLayout_9, 16, 1, 1, 1);

        graphA = new QChartView(centralWidget);
        graphA->setObjectName(QString::fromUtf8("graphA"));

        gridLayout->addWidget(graphA, 3, 0, 1, 1);

        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Interface Identification", nullptr));
        totalTimeLabel->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        numTreeLabel->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        moitieLabelB->setText(QCoreApplication::translate("MainWindow", "Moiti\303\251 B", nullptr));
        moitieLabelA->setText(QCoreApplication::translate("MainWindow", "Moiti\303\251 A", nullptr));
        stateLabelA->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        cycleTimeLabelA->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        startButton->setText(QCoreApplication::translate("MainWindow", "Start", nullptr));
        stopButton->setText(QCoreApplication::translate("MainWindow", "Stop", nullptr));
        resetButton->setText(QCoreApplication::translate("MainWindow", "Reset", nullptr));
        stateLabelB->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        cycleTimeLabelB->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "Port:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
