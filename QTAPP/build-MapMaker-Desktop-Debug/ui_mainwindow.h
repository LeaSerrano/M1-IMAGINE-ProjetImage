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
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen_Project;
    QAction *actionNew_Project;
    QAction *actionSave_Project;
    QWidget *centralwidget;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_2;
    QVBoxLayout *verticalLayout;
    QPushButton *generateButton;
    QPushButton *saveMapButton;
    QVBoxLayout *dataList;
    QPushButton *dataButton;
    QSpacerItem *horizontalSpacer;
    QVBoxLayout *verticalLayout_2;
    QLabel *currentMapLabel;
    QLabel *currentMap;
    QWidget *gridLayoutWidget;
    QGridLayout *mapDisplayers;
    QLabel *projectName;
    QMenuBar *menubar;
    QMenu *menuFile;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1076, 619);
        actionOpen_Project = new QAction(MainWindow);
        actionOpen_Project->setObjectName(QString::fromUtf8("actionOpen_Project"));
        actionNew_Project = new QAction(MainWindow);
        actionNew_Project->setObjectName(QString::fromUtf8("actionNew_Project"));
        actionSave_Project = new QAction(MainWindow);
        actionSave_Project->setObjectName(QString::fromUtf8("actionSave_Project"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        horizontalLayoutWidget = new QWidget(centralwidget);
        horizontalLayoutWidget->setObjectName(QString::fromUtf8("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(240, 0, 826, 571));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        generateButton = new QPushButton(horizontalLayoutWidget);
        generateButton->setObjectName(QString::fromUtf8("generateButton"));
        generateButton->setMinimumSize(QSize(150, 50));
        generateButton->setMaximumSize(QSize(55555, 50));

        verticalLayout->addWidget(generateButton);

        saveMapButton = new QPushButton(horizontalLayoutWidget);
        saveMapButton->setObjectName(QString::fromUtf8("saveMapButton"));
        saveMapButton->setMinimumSize(QSize(150, 50));
        saveMapButton->setMaximumSize(QSize(55555, 50));

        verticalLayout->addWidget(saveMapButton);

        dataList = new QVBoxLayout();
        dataList->setObjectName(QString::fromUtf8("dataList"));

        verticalLayout->addLayout(dataList);

        dataButton = new QPushButton(horizontalLayoutWidget);
        dataButton->setObjectName(QString::fromUtf8("dataButton"));
        dataButton->setMinimumSize(QSize(150, 50));

        verticalLayout->addWidget(dataButton);


        horizontalLayout->addLayout(verticalLayout);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        currentMapLabel = new QLabel(horizontalLayoutWidget);
        currentMapLabel->setObjectName(QString::fromUtf8("currentMapLabel"));
        currentMapLabel->setMinimumSize(QSize(0, 50));
        currentMapLabel->setMaximumSize(QSize(16777215, 50));
        QFont font;
        font.setPointSize(15);
        font.setBold(true);
        currentMapLabel->setFont(font);
        currentMapLabel->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(currentMapLabel);

        currentMap = new QLabel(horizontalLayoutWidget);
        currentMap->setObjectName(QString::fromUtf8("currentMap"));
        currentMap->setMinimumSize(QSize(500, 500));
        currentMap->setMaximumSize(QSize(500, 500));
        currentMap->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(currentMap);


        horizontalLayout->addLayout(verticalLayout_2);

        gridLayoutWidget = new QWidget(centralwidget);
        gridLayoutWidget->setObjectName(QString::fromUtf8("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(9, 59, 201, 511));
        mapDisplayers = new QGridLayout(gridLayoutWidget);
        mapDisplayers->setObjectName(QString::fromUtf8("mapDisplayers"));
        mapDisplayers->setContentsMargins(0, 0, 0, 0);
        projectName = new QLabel(centralwidget);
        projectName->setObjectName(QString::fromUtf8("projectName"));
        projectName->setGeometry(QRect(11, 5, 125, 50));
        projectName->setMinimumSize(QSize(125, 50));
        projectName->setMaximumSize(QSize(125, 30));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1076, 22));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menuFile->addAction(actionNew_Project);
        menuFile->addAction(actionOpen_Project);
        menuFile->addAction(actionSave_Project);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        actionOpen_Project->setText(QCoreApplication::translate("MainWindow", "Open Project....", nullptr));
        actionNew_Project->setText(QCoreApplication::translate("MainWindow", "New Project...", nullptr));
        actionSave_Project->setText(QCoreApplication::translate("MainWindow", "Save Project", nullptr));
        generateButton->setText(QCoreApplication::translate("MainWindow", "Generate", nullptr));
        saveMapButton->setText(QCoreApplication::translate("MainWindow", "Save Map", nullptr));
        dataButton->setText(QCoreApplication::translate("MainWindow", "Save Data", nullptr));
        currentMapLabel->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        currentMap->setText(QCoreApplication::translate("MainWindow", "currentMap", nullptr));
        projectName->setText(QCoreApplication::translate("MainWindow", "No project loaded", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
