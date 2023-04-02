#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QMainWindow>
#include <QFile>
#include <QDir>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QTextStream>
#include <QPrinter>
#include <QPrintDialog>
#include <QPixmap>
#include <QProcess>
#include <QEventLoop>
#include <QTimer>
#include <QThread>
#include <QDoubleSpinBox>
#include <QVBoxLayout>

#include "./App/ProjectManager.h"
#include "./App/DataManager.hpp"
#include "./App/MapManager.h"

#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionNew_Project_triggered();

    void on_actionOpen_Project_triggered();

    void on_generateButton_clicked();

    void on_dataButton_clicked();

    void on_saveMapButton_clicked();

private:
    Ui::MainWindow *ui;
    QString projectPath = "";

    ProjectManager* projectManager;
    DataManager* dataManager;
    MapManager* mapManager;

    vector<QPushButton*> mapDisplayers;

    vector<QHBoxLayout*> dataInputLayouts;

    string currentMap;
    int mapStatus;

    void routine();

    void clickOnMap(string mapId);

    void displayMap(QString mapPath, QPushButton* label);
    void displayMaps();

    void updateCurrentMap(string id);

    void setupUI();

    bool checkData();
    void setupDataInput();
    void colorDataInput();
};
#endif // MAINWINDOW_H
