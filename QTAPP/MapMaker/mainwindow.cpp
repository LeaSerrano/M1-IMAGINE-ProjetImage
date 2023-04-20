#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionNew_Project_triggered()
{
    dataManager = new DataManager(); dataManager->isQT = true;
    mapManager = new MapManager();

    projectPath.clear();
    projectPath = QFileDialog::getExistingDirectory(this,"Choose a folder");
    projectManager = new ProjectManager(projectPath.toStdString());
    QDir dir(projectPath);

    ui->projectName->setText(dir.dirName());

    routine();
}


void MainWindow::on_actionOpen_Project_triggered()
{
    on_actionNew_Project_triggered();
}

void MainWindow::clickOnMap(string mapId)
{
    autoroutine = false;
    updateCurrentMap(mapId);
}

void MainWindow::updateCurrentMap(string id)
{
    currentMap = id;
    ImageBase* map = MapManager::instance->get(currentMap);

    if(map != nullptr)
    {
        QImage image(map->getData(),map->getWidth(),map->getHeight(),map->getColor() ? QImage::Format_RGB888 : QImage::Format_Grayscale8);
        QPixmap pixmap = QPixmap::fromImage(image);

        ui->currentMap->setPixmap(pixmap);
        ui->currentMap->setScaledContents(true);
    }
    else
    {
        ui->currentMap->clear();
        ui->currentMap->setText(mapStatus == 1 ? "Generating..." : "NO DATA\nClick on 'Generate' to launch the generation process");
    }
    
    ui->currentMapLabel->setText(QString::fromStdString(currentMap));
}

void MainWindow::displayMap(QString mapPath,QPushButton* button)
{
    QPixmap map(projectPath + "/MAPS/" + mapPath);
    button->setIcon(map);
}

void MainWindow::displayMaps()
{
    int size = (int)(ui->mapDisplayers->geometry().width() * 0.3); //ui->mapDisplayers->geometry().height() / MapManager::instance->maps.size();
    int colCount = (int)(ui->mapDisplayers->geometry().height() / (float)size);
    int c = 0;
    for (map<string,ImageBase*>::const_iterator it=MapManager::instance->maps.begin(); it!=MapManager::instance->maps.end(); it++)
    {
        QPushButton* button = new QPushButton(this);
        QString path = QString::fromStdString(it->first) + (it->second->getColor() ? ".ppm" : ".pgm");
        displayMap(path,button);

        button->setMinimumSize(size,size);button->setMaximumSize(size,size);
        connect(button, &QPushButton::clicked, this, [=]() { this->clickOnMap(it->first); });

        mapDisplayers.push_back(button);
        ui->mapDisplayers->addWidget(button,c%colCount,c/colCount);
        button->setIconSize(button->size());
        c++;
    }
}

void MainWindow::setupUI()
{
    displayMaps();
}

void MainWindow::setupDataInput()
{
    for(int i = 0; i < dataInputLayouts.size();i++)
    {
        delete dataInputLayouts.at(i)->itemAt(0)->widget();
        delete dataInputLayouts.at(i)->itemAt(0)->widget();
        delete dataInputLayouts.at(i);
    }
    dataInputLayouts.clear();

    for(int i = 0; i < MapManager::mapDatas[currentMap].size();i++)
    {
        string dataId = MapManager::mapDatas[currentMap].at(i);

        QDoubleSpinBox* box = new QDoubleSpinBox(this);
        box->setMaximum(dataManager->ranges[dataId].max);
        box->setMinimum(dataManager->ranges[dataId].min);
        if(dataManager->valueExists(dataId))
        {
            box->setValue(dataManager->getValue(dataId));
        }
        else
        {
            box->setValue(dataManager->ranges[dataId].base);
        }

        QLabel* dataTitle = new QLabel(this);
        dataTitle->setText(QString::fromStdString(dataId));

        QHBoxLayout* dataLayout = new QHBoxLayout(this);
        dataLayout->addWidget(dataTitle);
        dataLayout->addWidget(box);

        ui->dataList->addLayout(dataLayout);
        dataInputLayouts.push_back(dataLayout);
    }
}

void MainWindow::colorDataInput()
{
    for(int i = 0; i < dataInputLayouts.size();i++)
    {
        string dataId = MapManager::mapDatas[currentMap].at(i);
        QLabel* label = qobject_cast<QLabel*>(dataInputLayouts.at(i)->itemAt(0)->widget());

        if(dataManager->valueExists(MapManager::mapDatas[currentMap].at(i)))
        {
            label->setStyleSheet("color: green;");
        }
        else
        {
            label->setStyleSheet("color: red;");
        }
    }
}

bool MainWindow::checkData()
{
    bool b = true;

    for(int i = 0; i < MapManager::mapDatas[currentMap].size();i++)
    {
        if(!dataManager->valueExists(MapManager::mapDatas[currentMap].at(i))){b = false;}
    }

    return b;
}

void MainWindow::displayFinalMap()
{
    int width = ui->currentMap->width();
    int height = ui->currentMap->height();

    QPoint point = ui->currentMap->mapToGlobal(QPoint(0,0));
    int tcx = point.x();// - (width * 0.85);
    int tcy = point.y();// - ((height * 0.9)/2.0);

    if(interestTypesButtons.size() <= 0)
    {
        int c = 0;
        for (unordered_map<string,vector<string>>::const_iterator it=PointOfInterestMap::interestTypes.begin(); it!=PointOfInterestMap::interestTypes.end(); it++)
        {
            for(string ip : it->second)
            {
                QPushButton* button = new QPushButton(this);

                button->setText(QString::fromStdString(ip));
                button->setMinimumSize(50,25);button->setMaximumSize(50,25);
                connect(button, &QPushButton::clicked, this, [=]()
                        {
                            interestType = (interestType == ip) ? "_" : ip; //Allows to hide points if clicked a second time of the same button
                        });

                interestTypesButtons.push_back(button);
                ui->interestTypesButtons->addWidget(button);
                c++;
            }
        }
    }

    interestPoints = PointOfInterestMap::generatePointsList();

    do
    {
        if(interestType != "")
        {
            for(int i = 0; i < interestLabels.size(); i++) { delete interestLabels.at(i);}
            interestLabels.clear();

            for(int i = 0; i < interestPoints.size(); i++)
            {
                if(interestPoints.at(i)->stringId == interestType)
                {
                    int x = (interestPoints.at(i)->x / dataManager->getValue("map_size")) * width;
                    int y = (interestPoints.at(i)->y / dataManager->getValue("map_size")) * height;
                    float dx = interestPoints.at(i)->name.size()*6;

                    QLabel* pointLabel = new QLabel(this);
                    pointLabel->setText(interestPoints.at(i)->name.c_str());
                    pointLabel->setStyleSheet("QLabel { background-color : white; color : black; }");
                    pointLabel->setGeometry(tcx + x - (dx/2.0),tcy + y,dx,10);
                    pointLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
                    pointLabel->setFont(QFont("Arial", 6, QFont::Bold));
                    pointLabel->show();
                    interestLabels.push_back(pointLabel);
                }
            }

            interestType = "";
        }

        QEventLoop loop;
        QTimer::singleShot(100, &loop, &QEventLoop::quit); // Pause for 1 second
        loop.exec();
    }while(displayFinal);
}

void MainWindow::routine()
{
    setupUI();

    int currentMapId = 0; autoroutine = true;
    while(currentMapId < MapManager::mapIdCount)
    {
        if(autoroutine)
        {
            currentMap = MapManager::mapIds[currentMapId];
            if(mapManager->mapExists(currentMap)){currentMapId++; continue;}
        }

        mapStatus = 0; updateCurrentMap(currentMap);
        if(autoroutine && !mapManager->mapExists(currentMap) && (MapManager::mapPreGenerate[currentMapId] || MapManager::mapDatas[MapManager::mapIds[currentMapId]].size() <= 0))
        {
            mapStatus = 1;
            updateCurrentMap(MapManager::mapIds[currentMapId]);
            mapManager->generateMap(currentMap);
        }
        setupDataInput();
        do
        {
            if(!displayFinal)
            {
                updateCurrentMap(currentMap);
            }

            QEventLoop loop;
            QTimer::singleShot(100, &loop, &QEventLoop::quit); // Pause for 1 second
            loop.exec();
        }while(mapStatus != 2);

        displayMaps();
        currentMapId++;
    }
}

void MainWindow::on_generateButton_clicked()
{
    if(!checkData()){colorDataInput(); return;}

    mapStatus = 1;    updateCurrentMap(currentMap);
    mapManager->generateMap(currentMap);
    updateCurrentMap(currentMap);
}

void MainWindow::on_dataButton_clicked()
{
    for(int i = 0; i < dataInputLayouts.size();i++)
    {
        string dataId = MapManager::mapDatas[currentMap].at(i);
        QDoubleSpinBox* input = qobject_cast<QDoubleSpinBox*>(dataInputLayouts.at(i)->itemAt(1)->widget());
        dataManager->setValue(dataId,input->value());
    }
    projectManager->saveData();
    colorDataInput();
}


void MainWindow::on_saveMapButton_clicked()
{
    if(!mapManager->mapExists(currentMap)){return;}

    mapManager->saveMap(currentMap);
    mapStatus = 2;
}


void MainWindow::on_finalButton_clicked()
{
    displayFinal = true;
    displayFinalMap();
}

