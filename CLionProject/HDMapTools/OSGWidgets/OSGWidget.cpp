//
// Created by zhihui on 3/27/19.
//

#include <iostream>

#include <QTimer>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QTextStream>
#include <QGridLayout>
#include <QProgressDialog>

#include <osg/Light>
#include <osg/Point>
#include <osgQt/GraphicsWindowQt>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/StateSetManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/TerrainManipulator>
#include <osg/ShapeDrawable>
#include <osg/ValueObject>

#include "OSGWidget.h"
#include "NodeNames.h"
#include "NodeTreeSearch.h"
#include "NodeTreeHandler.h"
#include "TextController.h"
#include "ENUCoorConv.hpp"
#include "VMapDrawable.h"
#include "../Common/VMapToJsonSingleton.h"
#include "../Common/VectorMapSingleton.h"
#include "../Common/VMapToCSVSingleton.h"
#include "Color.h"
#include "ProgressBarWorker.h"

OSGWidget::OSGWidget(QWidget *parent) : QWidget(parent),
                                        mainView(nullptr),
                                        rootNode(nullptr),
                                        lineEditor(nullptr),
                                        lineModification(nullptr),
                                        lineDeletion(nullptr),
                                        traceLineEditor(nullptr),
                                        traceLineModification(nullptr),
                                        traceLineDeletion(nullptr),
                                        pavementEditor(nullptr),
                                        pavementDeletion(nullptr),
                                        drivingArrowEditor(nullptr),
                                        drivingArrowModification(nullptr),
                                        drivingArrowDeletion(nullptr),
                                        trafficLightsEditor(nullptr),
                                        trafficLightsDeletion(nullptr),
                                        roadLinesEditor(nullptr),
                                        roadLinesModification(nullptr),
                                        roadLinesDeletion(nullptr),
                                        allPoints(new osg::Vec3Array),
                                        updateTimer(new QTimer(this)),
                                        openFileInfo(),
                                        progressBarWorker(new ProgressBarWorker())
//                                        ,progressDialog(nullptr)
{
    connect(updateTimer, &QTimer::timeout, this, &OSGWidget::updateFrame);
    connect(this, &OSGWidget::showProgressBarSignal, progressBarWorker, &ProgressBarWorker::showProgressBar);
    connect(this, &OSGWidget::closeProgressBarSignal, progressBarWorker, &ProgressBarWorker::closeProgressBar);
    progressBarWorker->moveToThread(&progressBarThread);
    connect(&progressBarThread, &QThread::finished, progressBarWorker, &QObject::deleteLater);
    // progressBarThread.start();
    updateTimer->start(30);
}

OSGWidget::~OSGWidget() {
    progressBarThread.quit();
    progressBarThread.wait();
}

void OSGWidget::init() {
    /*
     * 初始化场景树
     */
    initSceneGraph();
    initCamera();
    initEditor();
}

void OSGWidget::reset() {

    osg::ref_ptr<osg::Switch> pointCloudNode =
            dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(rootNode, pointCloudNodeName));
    pointCloudNode->removeChildren(0, pointCloudNode->getNumChildren());

//    osg::ref_ptr<osg::Switch> dataNode =
//            dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(rootNode, dataNodeName));
//    dataNode->removeChildren(0, dataNode->getNumChildren());

    {
        osg::ref_ptr<osg::Switch> vectorItemNode =
                dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(rootNode, vectorItemNodeName));
        vectorItemNode->removeChildren(0, vectorItemNode->getNumChildren());

        osg::ref_ptr<osg::Switch> traceItemNode =
                dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(rootNode, traceItemNodeName));
        traceItemNode->removeChildren(0, traceItemNode->getNumChildren());

        osg::ref_ptr<osg::Switch> pavementItemNode =
                dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(rootNode, pavementItemNodeName));
        pavementItemNode->removeChildren(0, pavementItemNode->getNumChildren());

        osg::ref_ptr<osg::Switch> drivingArrowItemNode =
                dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(rootNode, drivingArrowItemNodeName));
        drivingArrowItemNode->removeChildren(0, drivingArrowItemNode->getNumChildren());

        osg::ref_ptr<osg::Switch> trafficLightsItemNode =
                dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(rootNode, trafficLightsItemNodeName));
        trafficLightsItemNode->removeChildren(0, trafficLightsItemNode->getNumChildren());

        osg::ref_ptr<osg::Switch> roadLinesItemNode =
                dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(rootNode, roadLinesItemNodeName));
        roadLinesItemNode->removeChildren(0, roadLinesItemNode->getNumChildren());
    }

//    osg::ref_ptr<osg::Switch> textNode =
//            dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(rootNode, textNodeName));
//    textNode->removeChildren(0, textNode->getNumChildren());

    {
        osg::ref_ptr<osg::Switch> pointTextNode =
                dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(rootNode, pointTextNodeName));
        pointTextNode->removeChildren(0, pointTextNode->getNumChildren());

        osg::ref_ptr<osg::Switch> lineTextNode =
                dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(rootNode, lineTextNodeName));
        lineTextNode->removeChildren(0, lineTextNode->getNumChildren());

        osg::ref_ptr<osg::Switch> traceLineTextNode =
                dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(rootNode, traceLineTextNodeName));
        traceLineTextNode->removeChildren(0, traceLineTextNode->getNumChildren());

        osg::ref_ptr<osg::Switch> pavementTextNode =
                dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(rootNode, pavementTextNodeName));
        pavementTextNode->removeChildren(0, pavementTextNode->getNumChildren());

        osg::ref_ptr<osg::Switch> drivingArrowTextNode =
                dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(rootNode, drivingArrowTextNodeName));
        drivingArrowTextNode->removeChildren(0, drivingArrowTextNode->getNumChildren());

        osg::ref_ptr<osg::Switch> trafficLightsTextNode =
                dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(rootNode, trafficLightsTextNodeName));
        trafficLightsTextNode->removeChildren(0, trafficLightsTextNode->getNumChildren());
    }

    osg::ref_ptr<osg::Switch> tempNode =
            dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(rootNode, tempNodeName));
    tempNode->removeChildren(0, tempNode->getNumChildren());

    osg::ref_ptr<osg::Switch> virtualPlaneNode =
            dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(rootNode, virtualPlaneNodeName));
    virtualPlaneNode->removeChildren(0, virtualPlaneNode->getNumChildren());

    dataZ.clear();
    dataIntensity.clear();
    dataColor.clear();

    allPoints->clear();

    VectorMapSingleton::getInstance()->clear();
}

void OSGWidget::readPCDataFromFile(const QFileInfo &fileInfo,
                                   bool hasIntensity,
                                   bool hasRGB,
                                   const QString &originalPCDFileName) {
    openFileInfo = fileInfo;

    // 到场景树中查找结点 rootNode
    this->originalPCDFileName = originalPCDFileName;
    osg::ref_ptr<osg::Switch> pointCloudNode =
            dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(rootNode, pointCloudNodeName));
    // 创建一个 geode 结点
    osg::ref_ptr<osg::Geode> geode;

    // fileSuffix： 文件后缀 根据后缀 读取相应格式的文件 同时加入到 geode 结点中
    QString fileSuffix = fileInfo.suffix();
    if (fileSuffix == "pcd") {
        // 读取 .pcd 格式的文件
        geode = readPCDataFromPCDFile(fileInfo, hasIntensity, hasRGB);
    } else if (fileSuffix == "txt") {
        // 读取 .txt 格式的文件
        geode = readPCDataFromTXTFile(fileInfo, hasIntensity, hasRGB);
    } else if (fileSuffix == "las") {
        // 读取 .las 格式的文件
        geode = readPCDataFromLASFlile(fileInfo, hasIntensity, hasRGB);
    }

    // 当geode 结点不为空时 把该结点命名为 源数据结点 并把该结点设置为根结点 pointCloudNode 的子结点
    if (geode != nullptr) {
        geode->setName("CloudPoints");
        pointCloudNode->addChild(geode.get());
    }
}

void OSGWidget::loadVectorMap() {
    QString fileName;

    fileName = openFileInfo.fileName();

    // 如果文件的后缀为 pcd 则把该文件的源文件名赋给 fileName
    if (openFileInfo.suffix() == "pcd") {
        fileName = originalPCDFileName;
    }

    // 调试用上面一个，因为是外围build编译，部署发布用下面，数据和可执行文件在同一层
//    QString fileDir = "../../data/" + fileName + "/";

    // 创建文件磁盘路径
    QString fileDir = QDir::currentPath() + "/data/" + fileName + "/";
    QDir dir(fileDir);

    // 创建过滤器
    QStringList filters;
    filters << "*.data";
    dir.setNameFilters(filters);

    // 迭代该磁盘的文件 根据 category（类别）加载相应的数据 get到实例并更新
    unsigned long category = Category::NONE;
    QFileInfoList list = dir.entryInfoList();
    for (const QFileInfo &fileInfo : list) {
        std::string filePath = fileInfo.filePath().toStdString();
        std::string fileName = fileInfo.fileName().toStdString();

        if (fileName == "point.data") {
            category |= Category::POINT;
            std::vector<mdc::Point> points = mdc::parse<mdc::Point>(filePath);
            VectorMapSingleton::getInstance()->update(points);
        } else if (fileName == "line.data") {
            category |= Category::LINE;
            std::vector<mdc::Line> lines = mdc::parse<mdc::Line>(filePath);
            VectorMapSingleton::getInstance()->update(lines);
        } else if (fileName == "traceLine.data") {
            category |= Category::TraceLine;
            std::vector<mdc::TraceLine> traceLines = mdc::parse<mdc::TraceLine>(filePath);
            VectorMapSingleton::getInstance()->update(traceLines);
        } else if (fileName == "pavement.data") {
            category |= Category::Pavement;
            std::vector<mdc::Pavement> pavements = mdc::parse<mdc::Pavement>(filePath);
            VectorMapSingleton::getInstance()->update(pavements);
        } else if (fileName == "drivingArrow.data") {
            category |= Category::DrivingArrow;
            std::vector<mdc::DrivingArrow> drivingArrows = mdc::parse<mdc::DrivingArrow>(filePath);
            VectorMapSingleton::getInstance()->update(drivingArrows);
        } else if (fileName == "trafficLights.data") {
            category |= Category::TrafficLights;
            std::vector<mdc::TrafficLights> trafficLights = mdc::parse<mdc::TrafficLights>(filePath);
            VectorMapSingleton::getInstance()->update(trafficLights);
        } else if (fileName == "roadLines.data") {
            category |= Category::RoadLines;
            std::vector<mdc::RoadLines> roadLines = mdc::parse<mdc::RoadLines>(filePath);
            VectorMapSingleton::getInstance()->update(roadLines);
        }
    }

    std::cout << "load data: " << std::bitset<32>(category) << " done!" << std::endl;

    std::cout << "load data: ------------------------------------------" << std::endl;
    VectorMapSingleton::getInstance()->printAllPoints();
    VectorMapSingleton::getInstance()->printAllLines();
    VectorMapSingleton::getInstance()->printAllTraceLines();
    VectorMapSingleton::getInstance()->printAllPavements();
    VectorMapSingleton::getInstance()->printAllDrivingArrows();
    VectorMapSingleton::getInstance()->printAllTrafficLights();
    VectorMapSingleton::getInstance()->printAllRoadLines();
    std::cout << "--------------------------------" << std::endl;

    // 该函数用于在场景中画出 容器已经加载了的实例
    initVectorMap();
}

void OSGWidget::initTerrainManipulator() {
    initManipulator();
}

void OSGWidget::activeLineEditor(bool isActive) {
    if (isActive) {
        mainView->addEventHandler(lineEditor);
    } else {
        mainView->removeEventHandler(lineEditor);
    }
}

void OSGWidget::activeLineModification(bool isActive) {
    if (isActive) {
        mainView->addEventHandler(lineModification);
    } else {
        mainView->removeEventHandler(lineModification);
    }
}

void OSGWidget::activeLineDeletion(bool isActive) {
    if (isActive) {
        mainView->addEventHandler(lineDeletion);
    } else {
        mainView->removeEventHandler(lineDeletion);
    }
}

void OSGWidget::activeTraceLineEditor(bool isActive) {
    if (isActive) {
        mainView->addEventHandler(traceLineEditor);
    } else {
        mainView->removeEventHandler(traceLineEditor);
    }
}

void OSGWidget::activeTraceLineModification(bool isActive) {
    if (isActive) {
        mainView->addEventHandler(traceLineModification);
    } else {
        mainView->removeEventHandler(traceLineModification);
    }
}

void OSGWidget::activeTraceLineDeletion(bool isActive) {
    if (isActive) {
        mainView->addEventHandler(traceLineDeletion);
    } else {
        mainView->removeEventHandler(traceLineDeletion);
    }
}

void OSGWidget::activePavementEditor(bool isActive) {
    if (isActive) {
        mainView->addEventHandler(pavementEditor);
    } else {
        mainView->removeEventHandler(pavementEditor);
    }
}

void OSGWidget::activePavementDeletion(bool isActive) {
    if (isActive) {
        mainView->addEventHandler(pavementDeletion);
    } else {
        mainView->removeEventHandler(pavementDeletion);
    }
}

void OSGWidget::activeDrivingArrowEditor(bool isActive) {
    if (isActive) {
        mainView->addEventHandler(drivingArrowEditor);
    } else {
        mainView->removeEventHandler(drivingArrowEditor);
    }
}

void OSGWidget::activeDrivingArrowModification(bool isActive) {
    if (isActive) {
        mainView->addEventHandler(drivingArrowModification);
    } else {
        mainView->removeEventHandler(drivingArrowModification);
    }
}

void OSGWidget::activeDrivingArrowDeletion(bool isActive) {
    if (isActive) {
        mainView->addEventHandler(drivingArrowDeletion);
    } else {
        mainView->removeEventHandler(drivingArrowDeletion);
    }
}

void OSGWidget::activeTrafficLightsEditor(bool isActive) {
    if (isActive) {
        mainView->addEventHandler(trafficLightsEditor);
    } else {
        mainView->removeEventHandler(trafficLightsEditor);
    }
}

void OSGWidget::activeTrafficLightsDeletion(bool isActive) {
    if (isActive) {
        mainView->addEventHandler(trafficLightsDeletion);
    } else {
        mainView->removeEventHandler(trafficLightsDeletion);
    }
}

void OSGWidget::activeRoadLinesEditor(bool isActive) {
    if (isActive) {
        mainView->addEventHandler(roadLinesEditor);
    } else {
        mainView->removeEventHandler(roadLinesEditor);
    }
}

void OSGWidget::activeRoadLinesModification(bool isActive) {
    if (isActive) {
        mainView->addEventHandler(roadLinesModification);
    } else {
        mainView->removeEventHandler(roadLinesModification);
    }
}

void OSGWidget::activeRoadLinesDeletion(bool isActive) {
    if (isActive) {
        mainView->addEventHandler(roadLinesDeletion);
    } else {
        mainView->removeEventHandler(roadLinesDeletion);
    }
}

void OSGWidget::activeColorByZ(bool isActive) {
    colorPointCloudDataByZ(isActive);
}

void OSGWidget::activeColorByIntensity(bool isActive) {
    colorPointCloudDataByIntensity(isActive);
}

void OSGWidget::activeColorByTexture(bool isActive) {
    colorPointCloudDataByTexture(isActive);
}

void OSGWidget::transENU2LLH() const {
    std::vector<mdc::Point> points = VectorMapSingleton::getInstance()->findByFilter(
            [](const mdc::Point &point) { return true; });

    geodetic_converter::GeodeticConverter gc;
    gc.initialiseReference(22.5485150000, 114.0661120000, 0);
    for (auto &point : points) {
        double x, y, h;
        x = point.ly;
        y = point.bx;
        h = point.h;

        double latitude, longitude, altitude;
        latitude = longitude = altitude = 0;
        gc.enu2Geodetic(x, y, h, &latitude, &longitude, &altitude);

        point.b = latitude;
        point.l = longitude;
    }

    VectorMapSingleton::getInstance()->update(points);
}

void OSGWidget::saveVectorMap(const std::string &dirPath) const {
    transENU2LLH();
    VectorMapSingleton::getInstance()->saveToDir(dirPath);
}

void OSGWidget::transVectorMapToJson(const std::string &dirPath) const {

    VMapToJsonSingleton::getInstance()->transAllPavementsToJson(dirPath);
    VMapToJsonSingleton::getInstance()->transAllDrivingArrowToJson(dirPath);
    VMapToJsonSingleton::getInstance()->transAllTrafficLightsToJson(dirPath);
    VMapToJsonSingleton::getInstance()->transAllRoadLinesToJson(dirPath);

//    QString type = "SOLID_WHITE";
//    std::vector<mdc::Line> objects = VectorMapSingleton::getInstance()->findByFilter(
//            [](const mdc::Line &line) { return true; });
//    qDebug() << type << " size: " << objects.size();
//    QJsonArray jsonArray;
//    for (const auto &object : objects) {
//        size_t id = object.lID;
//        mdc::Point sPoint = VectorMapSingleton::getInstance()->findByID(mdc::Key<mdc::Point>(object.sPID));
//        mdc::Point ePoint = VectorMapSingleton::getInstance()->findByID(mdc::Key<mdc::Point>(object.ePID));
//        auto pos = lineToArray(sPoint, ePoint);
//        auto laneObject = getJsonObject(type, id, pos);
//        jsonArray.append(QJsonValue(laneObject));
//    }
//
//    outputJsonObject(type, jsonArray);

}

void OSGWidget::transVectorMapToCSV(const std::string &dirPath) const {
    VMapToCSVSingleton::getInstance()->transAllTraceLinesToCSV(dirPath);
}

void OSGWidget::transAllPointsToJSON(const std::string &dirPath) const {
    if (allPoints->empty()) {
        return;
    }
//    auto pointToArray = [](const osg::Vec3 &point) -> QJsonArray {
//        QJsonArray posArray;
//        posArray.append(point._v[0]);
//        posArray.append(point._v[1]);
//        posArray.append(point._v[2]);
//        return posArray;
//    };
//
//    auto getJsonObject = [](const QJsonArray &pos, const QJsonArray &color) -> QJsonObject {
//        QJsonObject laneObject;
//        laneObject.insert("Pos", pos);
//        laneObject.insert("Color", color);
//
//        return laneObject;
//    };
//
//    auto outputJsonObject = [&](const QString &type, const QJsonArray &array) {
//        QJsonObject json;
//        json.insert("Date", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
//        json.insert("Data", QJsonValue(array));
//
//        QJsonDocument document;
//        document.setObject(json);
//        QByteArray byteArray = document.toJson(QJsonDocument::Indented);
//
//        QString dirPathString = QString::fromStdString(dirPath);
//        QString filePath = dirPathString + "/" + type + ".json";
//        QFile file(filePath);
//        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
//            QMessageBox::warning(nullptr, "warning", "Can't save json file");
//            return;
//        }
//        QTextStream in(&file);
//        in << byteArray << "\n";
//        file.close();
//    };

//    {
//        QString type = "ALL_POINTS";
//        QJsonArray jsonArray;
//        size_t id = 0;
//        for (const auto &point: *allPointsID) {
//            auto pos = pointToArray(point);
//            auto pointObject = getJsonObject(type, id++, pos);
//            jsonArray.append(QJsonValue(pointObject));
//        }
//
//        outputJsonObject(type, jsonArray);
//    }
//    {
//        QString type = "ALL_COLORS";
//        QJsonArray jsonArray;
//        size_t id = 0;
//        osg::ref_ptr<osg::Vec3Array> colors = calculateColorArrayZ();
//        for (int i = 0; i < allPointsID->size(); ++i) {
//            auto pos = pointToArray(allPointsID->at(i));
//            auto color = pointToArray(colors->at(i));
//            auto pointObject = getJsonObject(pos, color);
//            jsonArray.append(QJsonValue(pointObject));
//        }
//
//        outputJsonObject(type, jsonArray);
//    }


}

void OSGWidget::paintEvent(QPaintEvent *) {
    frame();
}

void OSGWidget::initSceneGraph() {
    rootNode = new osg::Switch;
    rootNode->setName(rootNodeName);

    osg::ref_ptr<osg::Switch> pointCloudNode = new osg::Switch;
    pointCloudNode->setName(pointCloudNodeName);
    rootNode->addChild(pointCloudNode.get());

    osg::ref_ptr<osg::Switch> dataNode = new osg::Switch;
    dataNode->setName(dataNodeName);
    rootNode->addChild(dataNode.get());
    {
        osg::ref_ptr<osg::Switch> vectorItemNode = new osg::Switch;
        vectorItemNode->setName(vectorItemNodeName);
        dataNode->addChild(vectorItemNode.get());

        osg::ref_ptr<osg::Switch> traceItemNode = new osg::Switch;
        traceItemNode->setName(traceItemNodeName);
        dataNode->addChild(traceItemNode.get());

        osg::ref_ptr<osg::Switch> pavementItemNode = new osg::Switch;
        pavementItemNode->setName(pavementItemNodeName);
        dataNode->addChild(pavementItemNode.get());

        osg::ref_ptr<osg::Switch> drivingArrowItemNode = new osg::Switch;
        drivingArrowItemNode->setName(drivingArrowItemNodeName);
        dataNode->addChild(drivingArrowItemNode.get());

        osg::ref_ptr<osg::Switch> trafficLightsItemNode = new osg::Switch;
        trafficLightsItemNode->setName(trafficLightsItemNodeName);
        dataNode->addChild(trafficLightsItemNode.get());

        osg::ref_ptr<osg::Switch> roadLinesItemNode = new osg::Switch;
        roadLinesItemNode->setName(roadLinesItemNodeName);
        dataNode->addChild(roadLinesItemNode.get());
    }

    osg::ref_ptr<osg::Switch> textNode = new osg::Switch;
    textNode->setName(textNodeName);
    rootNode->addChild(textNode.get());
    {
        osg::ref_ptr<osg::Switch> pointTextNode = new osg::Switch;
        pointTextNode->setName(pointTextNodeName);
        textNode->addChild(pointTextNode.get());

        osg::ref_ptr<osg::Switch> lineTextNode = new osg::Switch;
        lineTextNode->setName(lineTextNodeName);
        textNode->addChild(lineTextNode.get());

        osg::ref_ptr<osg::Switch> traceLineTextNode = new osg::Switch;
        traceLineTextNode->setName(traceLineTextNodeName);
        textNode->addChild(traceLineTextNode.get());

        osg::ref_ptr<osg::Switch> pavementTextNode = new osg::Switch;
        pavementTextNode->setName(pavementTextNodeName);
        textNode->addChild(pavementTextNode.get());

        osg::ref_ptr<osg::Switch> drivingArrowTextNode = new osg::Switch;
        drivingArrowTextNode->setName(drivingArrowTextNodeName);
        textNode->addChild(drivingArrowTextNode.get());

        osg::ref_ptr<osg::Switch> trafficLightsTextNode = new osg::Switch;
        trafficLightsTextNode->setName(trafficLightsTextNodeName);
        textNode->addChild(trafficLightsTextNode);
    }

    osg::ref_ptr<osg::Switch> tempNode = new osg::Switch;
    tempNode->setName(tempNodeName);
    rootNode->addChild(tempNode.get());

    // 用于选点的虚拟平面，选点结束之后从场景树中删除
    osg::ref_ptr<osg::Switch> virtualPlaneNode = new osg::Switch;
    virtualPlaneNode->setName(virtualPlaneNodeName);
    rootNode->addChild(virtualPlaneNode.get());

    {
        //离散对象节点光照
        osg::ref_ptr<osg::Light> pcLight = new osg::Light;
        pcLight->setAmbient(osg::Vec4(.8f, .8f, .8f, .8f));
        osg::ref_ptr<osg::Point> pcps = new osg::Point(1.0f);

        osg::ref_ptr<osg::StateSet> pcss = new osg::StateSet;
        pcss->setMode(GL_LIGHTING, osg::StateAttribute::ON);
        pcss->setAttribute(pcLight, osg::StateAttribute::ON);
        pcss->setMode(GL_BLEND, osg::StateAttribute::ON);
        pcss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
        pcss->setMode(GL_MULTISAMPLE_ARB, osg::StateAttribute::ON);
        pcss->setAttribute(pcps, osg::StateAttribute::ON);

        pointCloudNode->setStateSet(pcss.get());
    }
}

void OSGWidget::initCamera() {
    osgViewer::ViewerBase::ThreadingModel threadingModel = osgViewer::ViewerBase::SingleThreaded;
    this->setThreadingModel(threadingModel);
    this->setKeyEventSetsDone(0);

    auto graphic_window = createGraphicsWindow(0, 0, 2000, 2000);
    auto traits = graphic_window->getTraits();

    mainView = new osgViewer::View;
    this->addView(mainView.get());

    auto camera = mainView->getCamera();
    camera->setGraphicsContext(graphic_window);
    camera->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    camera->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    camera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    camera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
    camera->setProjectionMatrixAsPerspective(30.f,
                                             static_cast<double>(traits->width) / static_cast<double>(traits->height),
                                             1.0, 1000.0);
    camera->setNearFarRatio(0.0000002);
    camera->setComputeNearFarMode(osg::CullSettings::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);
//  camera->setClearColor(osg::Vec4(0.84313, 0.84313, 0.89804, 1.0));

    //for outline effects
    {
        osg::DisplaySettings::instance()->setMinimumNumStencilBits(1);
        unsigned int clearMask = camera->getClearMask();
        camera->setClearMask(clearMask | GL_STENCIL_BUFFER_BIT);
        camera->setClearStencil(0);
    }

    mainView->addEventHandler(new osgViewer::StatsHandler);
    mainView->addEventHandler(new osgGA::StateSetManipulator(camera->getStateSet()));

    osg::ref_ptr<NodeTreeHandler> nodeTreeHandler = new NodeTreeHandler(rootNode.get());
    mainView->addEventHandler(nodeTreeHandler.get());

    osg::ref_ptr<TextController> textController = new TextController(rootNode.get());
    mainView->addEventHandler(textController.get());

    mainView->setSceneData(rootNode.get());
    mainView->setCameraManipulator(new osgGA::TrackballManipulator);

    QWidget *widget = graphic_window->getGLWidget();
    auto *grid = new QGridLayout;
    grid->addWidget(widget);
    this->setLayout(grid);
}

void OSGWidget::initEditor() {
    lineEditor = new LineEditor(rootNode);
    lineModification = new LineModification(rootNode);
    lineDeletion = new LineDeletion(rootNode);

    traceLineEditor = new TraceLineEditor(rootNode);
    traceLineModification = new TraceLineModification(rootNode);
    traceLineDeletion = new TraceLineDeletion(rootNode);

    pavementEditor = new PavementEditor(rootNode);
    pavementDeletion = new PavementDeletion(rootNode);

    drivingArrowEditor = new DrivingArrowEditor(rootNode);
    drivingArrowModification = new DrivingArrowModification(rootNode);
    drivingArrowDeletion = new DrivingArrowDeletion(rootNode);

    trafficLightsEditor = new TrafficLightsEditor(rootNode);
    trafficLightsDeletion = new TrafficLightsDeletion(rootNode);

    roadLinesEditor = new RoadLinesEditor(rootNode);
    roadLinesModification = new RoadLinesModification(rootNode);
    roadLinesDeletion = new RoadLinesDeletion(rootNode);
}

void OSGWidget::initManipulator() {
    osg::ref_ptr<osgGA::TerrainManipulator> terrainManipulator = new osgGA::TerrainManipulator;
    terrainManipulator->setHomePosition(rootNode->getBound().center() + osg::Vec3(0.0, 0.0, 300.0),
                                        rootNode->getBound().center(), osg::Vec3(0, 1, 0));
    mainView->setCameraManipulator(terrainManipulator.get());
}


void OSGWidget::initVectorMap() {
    drawVectorItems();
    drawTraceItems();
    drawPavementItems();
    drawDrivingArrowItems();
    drawTrafficLightsItems();
    drawRoadLinesItems();
    // 去掉独立的点在vectorItemNode的节点
    clearIrrelevantPiont();
}

osgQt::GraphicsWindowQt *
OSGWidget::createGraphicsWindow(int x, int y, int w, int h, const std::string &name, bool windowDecoration) {
    osg::DisplaySettings *ds = osg::DisplaySettings::instance().get();
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->windowName = name;
    traits->windowDecoration = windowDecoration;
    traits->x = x;
    traits->y = y;
    traits->width = w;
    traits->height = h;
    traits->doubleBuffer = true;
    traits->alpha = ds->getMinimumNumAlphaBits();
    traits->stencil = ds->getMinimumNumStencilBits();
    traits->sampleBuffers = ds->getMultiSamples();
    traits->samples = ds->getNumMultiSamples();

    return new osgQt::GraphicsWindowQt(traits.get());
}

osg::ref_ptr<osg::Geode> OSGWidget::readPCDataFromPCDFile(const QFileInfo &fileInfo, bool hasIntensity, bool hasRGB) {
    // pcd 文件不可能既有 RGB 又有 Intensity 的属性
    if (hasRGB) {
        // 采用 pcl 库 读取 pcd 文件
        return readPCLDataFromXYZRGBFile(fileInfo);
    } else {
        return readPCLDataFromXYZIFile(fileInfo);
    }
}

osg::ref_ptr<osg::Geode> OSGWidget::readPCDataFromTXTFile(const QFileInfo &fileInfo, bool hasIntensity, bool hasRGB) {
    if (hasIntensity && !hasRGB) {
        // 采用 Qt 的方法读取 txt 文件
        return readTXTDataFromIFile(fileInfo);
    } else if (!hasIntensity && hasRGB) {
        return readTXTDataFromRGBFile(fileInfo);
    } else if (hasIntensity && hasRGB) {
        return readTXTDataFromIRGBFile(fileInfo);
    } else {
        return readTXTDataFromFile(fileInfo);
    }
}

osg::ref_ptr<osg::Geode> OSGWidget::readPCDataFromLASFlile(const QFileInfo &fileInfo, bool hasIntensity, bool hasRGB) {
    if (hasIntensity && !hasRGB) {
        // 采用 Las 官网提供的例子 的方法读取 las 文件
        return readLASDataFromIFile(fileInfo);
    } else if (!hasIntensity && hasRGB) {
        return readLASDataFromRGBFile(fileInfo);
    } else if (hasIntensity && hasRGB) {
        return readLASDataFromIRGBFile(fileInfo);
    } else {
        return readLASDataFromFile(fileInfo);
    }
}

osg::ref_ptr<osg::Geode> OSGWidget::readPCLDataFromXYZIFile(const QFileInfo &fileInfo) {
    /*
     * pcl 库读取 pcd::XYZIFile文件的方法
     */
    pcl::PointCloud<pcl::PointXYZI>::Ptr pointCloud(new pcl::PointCloud<pcl::PointXYZI>);
    pcl::PCDReader reader;
    reader.read(fileInfo.filePath().toStdString(), *pointCloud);
    osg::ref_ptr<osg::Geode> geode = addMapPointCloud(pointCloud, osg::Vec3(0.4, 0.4, 0.4));
    geode->setName(fileInfo.fileName().toStdString());
    return geode;
}

osg::ref_ptr<osg::Geode> OSGWidget::readPCLDataFromXYZRGBFile(const QFileInfo &fileInfo) {

    pcl::PointCloud<pcl::PointXYZRGB>::Ptr pointCloud(new pcl::PointCloud<pcl::PointXYZRGB>);

    pcl::PCDReader reader;

    reader.read(fileInfo.filePath().toStdString(), *pointCloud);

    osg::ref_ptr<osg::Geode> geode = addMapPointCloud(pointCloud, osg::Vec3(0.4, 0.4, 0.4));

    geode->setName(fileInfo.fileName().toStdString());

    return geode;
}

osg::ref_ptr<osg::Geode> OSGWidget::readTXTDataFromIFile(const QFileInfo &fileInfo) {
    /*
     * Qt QTextStream 读取 txt::IFile 文件的方法： 先读取文件，把文件数据放入容器中， 然后从容器中取数据。
     * 这样做是为了加快文件的读取速度
     * osg::ref_ptr<osg::Geode> geode = addMapPointCloud(vertices, osg::Vec3(0.4, 0.4, 0.4));
     * 同时创建一个 geode 结点 存储该点云数据
     */
    QFile file(fileInfo.filePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "warning", "Can't open file!");
        return nullptr;
    }

    // 清空容器内 dataZ dataIntensity dataColor 的内容
    dataZ.clear();
    dataIntensity.clear();
    dataColor.clear();

    // 创建顶点
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;

    QTextStream in(&file);
    QString lineString;
    // while 循环遍历
    while (!in.atEnd()) {
        // 读一行
        lineString = in.readLine();
        // 以 正则表达式 [, ] 分割 字段
        QStringList sections = lineString.trimmed().split(QRegExp("[, ]"));
        double x, y, z;
        float i;
        x = sections[0].toDouble();
        y = sections[1].toDouble();
        z = sections[2].toDouble();
        if (sections.size() >= 4) {
            i = sections[3].toDouble();
            // 将强度放入 dataIntensity
            dataIntensity.push_back(i);
        }
        // 将点坐标放入顶点
        vertices->push_back(osg::Vec3d(x, y, z));
        // 将 z 坐标放入 dataZ
        dataZ.push_back(z);
    }
    // 关闭文件流
    file.close();

    // 添加一个点云结点
    osg::ref_ptr<osg::Geode> geode = addMapPointCloud(vertices, osg::Vec3(0.4, 0.4, 0.4));
    return geode;
}

osg::ref_ptr<osg::Geode> OSGWidget::readTXTDataFromRGBFile(const QFileInfo &fileInfo) {
    QFile file(fileInfo.filePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "warning", "Can't open file!");
        return nullptr;
    }

    dataZ.clear();
    dataIntensity.clear();
    dataColor.clear();

    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;

    QTextStream in(&file);
    QString lineString;
    // while 循环遍历
    while (!in.atEnd()) {
        lineString = in.readLine();
        QStringList sections = lineString.trimmed().split(QRegExp("[, ]"));
        double x, y, z;
        int r, g, b;

        x = sections[0].toDouble();
        y = sections[1].toDouble();
        z = sections[2].toDouble();

        r = sections[3].toInt();
        g = sections[4].toInt();
        b = sections[5].toInt();

        vertices->push_back(osg::Vec3d(x, y, z));
        dataZ.push_back(z);
        dataColor.emplace_back(osg::Vec3(r * 1.0 / 255, g * 1.0 / 255, b * 1.0 / 255));
    }
    file.close();

    osg::ref_ptr<osg::Geode> geode = addMapPointCloud(vertices, osg::Vec3(0.4, 0.4, 0.4));
    return geode;
}

osg::ref_ptr<osg::Geode> OSGWidget::readTXTDataFromIRGBFile(const QFileInfo &fileInfo) {
    QFile file(fileInfo.filePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "warning", "Can't open file!");
        return nullptr;
    }

    dataZ.clear();
    dataIntensity.clear();
    dataColor.clear();

    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;

    QTextStream in(&file);
    QString lineString;
    while (!in.atEnd()) {
        lineString = in.readLine();
        QStringList sections = lineString.trimmed().split(QRegExp("[, ]"));
        double x, y, z;
        int i;
        int r, g, b;

        x = sections[0].toDouble();
        y = sections[1].toDouble();
        z = sections[2].toDouble();

        i = sections[3].toInt();

        r = sections[4].toInt();
        g = sections[5].toInt();
        b = sections[6].toInt();

        vertices->push_back(osg::Vec3d(x, y, z));
        dataZ.push_back(z);
        dataIntensity.push_back(i);
        dataColor.emplace_back(osg::Vec3(r * 1.0 / 255, g * 1.0 / 255, b * 1.0 / 255));
    }
    file.close();

    osg::ref_ptr<osg::Geode> geode = addMapPointCloud(vertices, osg::Vec3(0.4, 0.4, 0.4));
    return geode;
}

osg::ref_ptr<osg::Geode> OSGWidget::readTXTDataFromFile(const QFileInfo &fileInfo) {
    QFile file(fileInfo.filePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "warning", "Can't open file!");
        return nullptr;
    }

    dataZ.clear();
    dataIntensity.clear();
    dataColor.clear();

    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;

    QTextStream in(&file);
    QString lineString;
    while (!in.atEnd()) {
        lineString = in.readLine();
        QStringList sections = lineString.trimmed().split(QRegExp("[, ]"));
        double x, y, z;

        x = sections[0].toDouble();
        y = sections[1].toDouble();
        z = sections[2].toDouble();

        vertices->push_back(osg::Vec3d(x, y, z));
        dataZ.push_back(z);
    }
    file.close();

    osg::ref_ptr<osg::Geode> geode = addMapPointCloud(vertices, osg::Vec3(0.4, 0.4, 0.4));
    return geode;
}

osg::ref_ptr<osg::Geode> OSGWidget::readLASDataFromIFile(const QFileInfo &fileInfo) {
    std::ifstream ifs;
    ifs.open(fileInfo.absoluteFilePath().toStdString().c_str(), std::ios::in | std::ios::binary);
    liblas::ReaderFactory f;
    liblas::Reader reader = f.CreateWithStream(ifs);
    const liblas::Header &header = reader.GetHeader();

    std::cout << "Compressed: " << (header.Compressed() ? "true" : "false") << std::endl;
    std::cout << "Signature: " << header.GetFileSignature() << '\n';
    std::cout << "Points count: " << header.GetPointRecordsCount() << '\n';

    dataZ.clear();
    dataIntensity.clear();
    dataColor.clear();

    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;

    while (reader.ReadNextPoint()) {
        const liblas::Point &p = reader.GetPoint();
        vertices->push_back(osg::Vec3d(p.GetX(), p.GetY(), p.GetZ()));
        dataZ.push_back(p.GetZ());
        dataIntensity.push_back(p.GetIntensity());
    }
    ifs.close();

    osg::ref_ptr<osg::Geode> geode = addMapPointCloud(vertices, osg::Vec3(0.4, 0.4, 0.4));

    return geode;
}

osg::ref_ptr<osg::Geode> OSGWidget::readLASDataFromRGBFile(const QFileInfo &fileInfo) {
    std::ifstream ifs;
    ifs.open(fileInfo.absoluteFilePath().toStdString().c_str(), std::ios::in | std::ios::binary);
    liblas::ReaderFactory f;
    liblas::Reader reader = f.CreateWithStream(ifs);
    const liblas::Header &header = reader.GetHeader();

    std::cout << "Compressed: " << (header.Compressed() ? "true" : "false") << std::endl;
    std::cout << "Signature: " << header.GetFileSignature() << '\n';
    std::cout << "Points count: " << header.GetPointRecordsCount() << '\n';

    dataZ.clear();
    dataIntensity.clear();
    dataColor.clear();

    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;

    while (reader.ReadNextPoint()) {
        const liblas::Point &p = reader.GetPoint();
        liblas::Color color = p.GetColor();
        vertices->push_back(osg::Vec3d(p.GetX(), p.GetY(), p.GetZ()));
        dataZ.push_back(p.GetZ());
        dataColor.emplace_back(osg::Vec3(color.GetRed() * 1.0 / 65535,
                                         color.GetGreen() * 1.0 / 65535,
                                         color.GetBlue() * 1.0 / 65535));
    }
    ifs.close();

    osg::ref_ptr<osg::Geode> geode = addMapPointCloud(vertices, osg::Vec3(0.4, 0.4, 0.4));

    return geode;
}

osg::ref_ptr<osg::Geode> OSGWidget::readLASDataFromIRGBFile(const QFileInfo &fileInfo) {
    std::ifstream ifs;
    ifs.open(fileInfo.absoluteFilePath().toStdString().c_str(), std::ios::in | std::ios::binary);
    liblas::ReaderFactory f;
    liblas::Reader reader = f.CreateWithStream(ifs);
    const liblas::Header &header = reader.GetHeader();

    std::cout << "Compressed: " << (header.Compressed() ? "true" : "false") << std::endl;
    std::cout << "Signature: " << header.GetFileSignature() << '\n';
    std::cout << "Points count: " << header.GetPointRecordsCount() << '\n';

    dataZ.clear();
    dataIntensity.clear();
    dataColor.clear();

    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;

    while (reader.ReadNextPoint()) {
        const liblas::Point &p = reader.GetPoint();
        liblas::Color color = p.GetColor();
        vertices->push_back(osg::Vec3d(p.GetX(), p.GetY(), p.GetZ()));
        dataZ.push_back(p.GetZ());
        dataIntensity.push_back(p.GetIntensity());
        dataColor.emplace_back(osg::Vec3(color.GetRed() * 1.0 / 65535,
                                         color.GetGreen() * 1.0 / 65535,
                                         color.GetBlue() * 1.0 / 65535));
    }
    ifs.close();

    osg::ref_ptr<osg::Geode> geode = addMapPointCloud(vertices, osg::Vec3(0.4, 0.4, 0.4));

    return geode;
}

osg::ref_ptr<osg::Geode> OSGWidget::readLASDataFromFile(const QFileInfo &fileInfo) {
    std::ifstream ifs;
    ifs.open(fileInfo.absoluteFilePath().toStdString().c_str(), std::ios::in | std::ios::binary);
    liblas::ReaderFactory f;
    liblas::Reader reader = f.CreateWithStream(ifs);
    const liblas::Header &header = reader.GetHeader();

    std::cout << "Compressed: " << (header.Compressed() ? "true" : "false") << std::endl;
    std::cout << "Signature: " << header.GetFileSignature() << '\n';
    std::cout << "Points count: " << header.GetPointRecordsCount() << '\n';

    dataZ.clear();
    dataIntensity.clear();
    dataColor.clear();

    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;

    while (reader.ReadNextPoint()) {
        const liblas::Point &p = reader.GetPoint();
        liblas::Color color = p.GetColor();
        vertices->push_back(osg::Vec3d(p.GetX(), p.GetY(), p.GetZ()));
        dataZ.push_back(p.GetZ());
    }
    ifs.close();

    osg::ref_ptr<osg::Geode> geode = addMapPointCloud(vertices, osg::Vec3(0.4, 0.4, 0.4));
    return geode;
}

osg::ref_ptr<osg::Geode> OSGWidget::addMapPointCloud(const pcl::PointCloud<pcl::PointXYZI>::Ptr &mapPointCloud,
                                                     osg::Vec3 color) {
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;

//    progressDialog = new QProgressDialog("Reading files...", QString(), 0, 0, nullptr);
//    progressDialog->setWindowModality(Qt::NonModal);
//    progressDialog->setMinimumDuration(0);

    dataZ.clear();
    dataIntensity.clear();
    dataColor.clear();
    allPoints->clear();

//    progressDialog->show();

    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    for (const auto &point : mapPointCloud->points) {
        vertices->push_back(osg::Vec3(point.x, point.y, point.z));
        allPoints->push_back(osg::Vec3(point.x, point.y, point.z));
        dataZ.push_back(point.z);
        dataIntensity.push_back(point.intensity);
        //qApp->processEvents(QEventLoop::DialogExec);
    }

    // progressDialog->cancel();
//    delete progressDialog;

    geom->setVertexArray(vertices.get());

    osg::ref_ptr<osg::Vec3Array> colors = new osg::Vec3Array;
    colors->push_back(color);
    geom->setColorArray(colors.get());
    geom->setColorBinding(osg::Geometry::BIND_OVERALL);

    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, static_cast<GLsizei>(vertices->size())));
    geode->addDrawable(geom.get());

    return geode;
}

osg::ref_ptr<osg::Geode> OSGWidget::addMapPointCloud(const pcl::PointCloud<pcl::PointXYZRGB>::Ptr &mapPointCloud,
                                                     osg::Vec3 color) {
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;

    dataZ.clear();
    dataIntensity.clear();
    dataColor.clear();
    allPoints->clear();

    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    for (const auto &point : mapPointCloud->points) {
        vertices->push_back(osg::Vec3(point.x, point.y, point.z));
        allPoints->push_back(osg::Vec3(point.x, point.y, point.z));
        dataZ.push_back(point.z);
        dataColor.emplace_back(osg::Vec3(point.r * 1.0 / 255, point.g * 1.0 / 255, point.b * 1.0 / 255));
    }
    geom->setVertexArray(vertices.get());

    osg::ref_ptr<osg::Vec3Array> colors = new osg::Vec3Array;
    for (const auto &color : dataColor) {
        colors->push_back(color);
    }
    geom->setColorArray(colors.get());
    geom->setColorBinding(osg::Geometry::BIND_OVERALL);

    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, static_cast<GLsizei>(vertices->size())));
    geode->addDrawable(geom.get());

    return geode;
}

osg::ref_ptr<osg::Geode> OSGWidget::addMapPointCloud(const osg::ref_ptr<osg::Vec3Array> &vertices,
                                                     osg::Vec3 color) const {
    // 创建叶子结点： geode 存储 vertice
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    // 创建几何图形结点： geom 存储 color
    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;

    // 把顶点 vertice 加入叶子结点
    geom->setVertexArray(vertices.get());

    // 创建一个颜色属性
    osg::ref_ptr<osg::Vec3Array> colors = new osg::Vec3Array;

    // 当容器内 dataColor 没有数据时，则添加颜色属性
    if (dataColor.empty()) {
        colors->push_back(color);
    } else {
        // 否则直接遍历容器 已经存在的颜色属性 数据
        for (const auto &color : dataColor) {
            colors->push_back(color);
        }
    }

    // 把 colors 加入到 geom 结点 并设定 绑定关系
    geom->setColorArray(colors.get());
    geom->setColorBinding(osg::Geometry::BIND_OVERALL);

    // 向几何图形结点 geom 添加一个基本集
    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, static_cast<GLsizei>(vertices->size())));

    // 把该几何图形加入到叶子结点
    geode->addDrawable(geom.get());

    return geode;
}

void OSGWidget::colorPointCloudDataByZ(bool isActive) const {
    osg::ref_ptr<osg::Geode> geode = dynamic_cast<osg::Geode *>(NodeTreeSearch::findNodeWithName(rootNode,
                                                                                                 "CloudPoints"));
    if (geode == nullptr) {
        return;
    }
    osg::ref_ptr<osg::Geometry> geom;
    osg::ref_ptr<osg::Vec3Array> colors;
    geom = dynamic_cast<osg::Geometry *>(geode->getDrawable(0));

    if (isActive) {
        colors = calculateColorArrayZ();
        geom->setColorArray(colors.get());
        geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
    } else {
        colors = new osg::Vec3Array;
        colors->push_back(osg::Vec3(0.4, 0.4, 0.4));
        geom->setColorArray(colors.get());
        geom->setColorBinding(osg::Geometry::BIND_OVERALL);
    }
}

void OSGWidget::colorPointCloudDataByIntensity(bool isActive) const {
    osg::ref_ptr<osg::Geode> geode = dynamic_cast<osg::Geode *>(NodeTreeSearch::findNodeWithName(rootNode,
                                                                                                 "CloudPoints"));
    if (geode == nullptr) {
        return;
    }
    if (!hasDataIntensity()) {
        isActive = false;
    }
    osg::ref_ptr<osg::Geometry> geom;
    osg::ref_ptr<osg::Vec3Array> colors;
    geom = dynamic_cast<osg::Geometry *>(geode->getDrawable(0));

    if (isActive) {
        colors = calculateColorArrayIntensity();
        geom->setColorArray(colors.get());
        geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
    } else {
        colors = new osg::Vec3Array;
        colors->push_back(osg::Vec3(0.4, 0.4, 0.4));
        geom->setColorArray(colors.get());
        geom->setColorBinding(osg::Geometry::BIND_OVERALL);
    }
}

void OSGWidget::colorPointCloudDataByTexture(bool isActive) const {
    osg::ref_ptr<osg::Geode>
            geode = dynamic_cast<osg::Geode *> (NodeTreeSearch::findNodeWithName(rootNode, "CloudPoints"));
    if (geode == nullptr) {
        return;
    }
    osg::ref_ptr<osg::Geometry> geom;
    osg::ref_ptr<osg::Vec3Array> colors;
    geom = dynamic_cast<osg::Geometry *>(geode->getDrawable(0));

    if (isActive) {
        colors = calculateColorArrayTexture();
        geom->setColorArray(colors.get());
        if (dataColor.empty()) {
            geom->setColorBinding(osg::Geometry::BIND_OVERALL);
        } else {
            geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
        }
    } else {
        colors = new osg::Vec3Array;
        colors->push_back(osg::Vec3(0.4, 0.4, 0.4));
        geom->setColorArray(colors.get());
        geom->setColorBinding(osg::Geometry::BIND_OVERALL);
    }
}

osg::ref_ptr<osg::Vec3Array> OSGWidget::calculateColorArrayZ() const {
    float minHeight = LONG_MAX, maxHeight = 0.0f;
    int range, heightRange;
    osg::ref_ptr<osg::Vec3Array> colors = new osg::Vec3Array;

    for (float height : dataZ) {
        if (height > maxHeight) {
            maxHeight = height;
        }
        if (height < minHeight) {
            minHeight = height;
        }
    }

    heightRange = static_cast<int>((maxHeight - minHeight) / ColorZList.size());
    if (heightRange <= 0) {
        heightRange = 1;
    }

    for (float height : dataZ) {
        range = static_cast<int>((height - minHeight) / heightRange);
        if (range >= ColorZList.size()) {
            range = ColorZList.size() - 1;
        }
        colors->push_back(ColorZList[range]);
    }
    return colors;
}

osg::ref_ptr<osg::Vec3Array> OSGWidget::calculateColorArrayIntensity() const {
    float minIntensity = LONG_MAX, maxIntensity = 0.0f;
    int range, intensityRange;
    osg::ref_ptr<osg::Vec3Array> colors = new osg::Vec3Array;

    for (float intensity : dataIntensity) {
        if (intensity > maxIntensity) {
            maxIntensity = intensity;
        }
        if (intensity < minIntensity) {
            minIntensity = intensity;
        }
    }
    intensityRange = static_cast<int>((maxIntensity - minIntensity) / ColorZList.size());

    for (float intensity : dataZ) {
        range = static_cast<int>((intensity - minIntensity) / intensityRange);
        if (range >= ColorZList.size()) {
            range = ColorZList.size() - 1;
        }
        colors->push_back(ColorZList[range]);
    }
    return colors;
}

osg::ref_ptr<osg::Vec3Array> OSGWidget::calculateColorArrayTexture() const {
    osg::ref_ptr<osg::Vec3Array> colors = new osg::Vec3Array;

    if (dataColor.empty()) {
        colors->push_back(osg::Vec3(0.4, 0.4, 0.4));
    } else {
        for (const auto &color : dataColor) {
            colors->push_back(color);
        }
    }

    return colors;
}

bool OSGWidget::hasDataIntensity() const {
    for (float intensity : dataIntensity) {
        if (fabsf(intensity - 0.0f) >= 1E-7) {
            return true;
        }
    }
    return false;
}

void OSGWidget::drawVectorItems() {
    osg::ref_ptr<osg::Switch> vectorItemNode =
            dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(rootNode, vectorItemNodeName));
    int vectorIndex = vectorItemNode->getNumChildren();
    std::string type("Uncertain");

    osg::ref_ptr<osg::Switch> vectorNode = new osg::Switch;
    vectorNode->setName(vectorItemName + std::to_string(vectorIndex++));
    vectorNode->setUserValue("itemType", type);

    std::vector<mdc::Point> points;
    std::vector<mdc::Line> lines;

    lines = VectorMapSingleton::getInstance()->findByFilter([](const mdc::Line &line) { return true; });

    // 画线
    for (const auto &line : lines) {
        osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;

        mdc::Point sPoint = VectorMapSingleton::getInstance()->findByID(mdc::Key<mdc::Point>(line.sPID));
        mdc::Point ePoint = VectorMapSingleton::getInstance()->findByID(mdc::Key<mdc::Point>(line.ePID));

        if (std::find_if(points.begin(),
                         points.end(),
                         [&sPoint](const mdc::Point &point) { return point.pID == sPoint.pID; }) == points.end()) {
            points.push_back(sPoint);
        }

        if (std::find_if(points.begin(),
                         points.end(),
                         [&ePoint](const mdc::Point &point) { return point.pID == ePoint.pID; }) == points.end()) {
            points.push_back(ePoint);
        }

        vertices->push_back(osg::Vec3d(sPoint.ly, sPoint.bx, sPoint.h));
        vertices->push_back(osg::Vec3d(ePoint.ly, ePoint.bx, ePoint.h));

        osg::ref_ptr<osg::Vec3Array> colors = new osg::Vec3Array;
        colors->push_back(osg::Vec3(1.0, 1.0, 1.0));

        osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
        geom->setVertexArray(vertices.get());
        geom->setColorArray(colors.get());
        geom->setColorBinding(osg::Geometry::BIND_OVERALL);
        geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, vertices->size()));

        osg::ref_ptr<osg::Geode> geode = new osg::Geode;
        geode->setName("Line_" + std::to_string(line.lID));
        geode->addDrawable(geom);

        vectorNode->addChild(geode);
    }

    // 画点
    for (const auto &point : points) {
        int localPointIndex = point.pID;
        osg::Vec3d localPoint(point.ly, point.bx, point.h);

        osg::ref_ptr<osg::Geode> pointGeode = new osg::Geode;
        pointGeode->setName("Sphere");
        pointGeode->setUserValue("ID", localPointIndex);
        pointGeode->setUserValue("pos", localPoint);

        osg::ref_ptr<osg::ShapeDrawable> pointSphere = new osg::ShapeDrawable(new osg::Sphere(localPoint, 0.15));
        pointGeode->addDrawable(pointSphere);
        vectorNode->addChild(pointGeode);
    }

    vectorItemNode->addChild(vectorNode);

    // 显示点ID
    osg::ref_ptr<osg::Switch> pointTextNode = dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(rootNode,
                                                                                                           pointTextNodeName));
    for (const auto &point : points) {
        osg::Vec3d pos(point.ly, point.bx, point.h);
        std::string name = "TSphere_" + std::to_string(point.pID);
        osg::Vec4 color(0.0, 1.0, 0.0, 0.5);

        osg::ref_ptr<osg::Geode> pointTextGeode = new osg::Geode;
        pointTextGeode->setName(name);

        osg::ref_ptr<osgText::Text> text = new osgText::Text;
        text->setCharacterSize(0.5);
        text->setAxisAlignment(osgText::TextBase::XY_PLANE);
        text->setPosition(pos);
        text->setText(std::to_string(point.pID));
        text->setColor(color);

        pointTextGeode->addDrawable(text);

        pointTextNode->addChild(pointTextGeode);
    }

    // 显示线ID
    osg::ref_ptr<osg::Switch> lineTextNode = dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(rootNode,
                                                                                                          lineTextNodeName));
    for (const auto &line:lines) {
        auto sPoint = VectorMapSingleton::getInstance()->findByID(mdc::Key<mdc::Point>(line.sPID));
        auto ePoint = VectorMapSingleton::getInstance()->findByID(mdc::Key<mdc::Point>(line.ePID));

        osg::Vec3d pos1(sPoint.ly, sPoint.bx, sPoint.h);
        osg::Vec3d pos2(ePoint.ly, ePoint.bx, ePoint.h);
        osg::Vec3d pos = (pos1 + pos2) / 2;
        std::string name = "TLine_" + std::to_string(line.lID);
        osg::Vec4 color(1.0, 0.0, 0.0, 0.5);

        osg::ref_ptr<osg::Geode> lineTextGeode = new osg::Geode;
        lineTextGeode->setName(name);

        osg::ref_ptr<osgText::Text> text = new osgText::Text;
        text->setCharacterSize(0.5);
        text->setAxisAlignment(osgText::TextBase::XY_PLANE);
        text->setPosition(pos);
        text->setText(std::to_string(line.lID));
        text->setColor(color);

        lineTextGeode->addDrawable(text);

        lineTextNode->addChild(lineTextGeode);
    }
}

void OSGWidget::drawTraceItems() {
    osg::ref_ptr<osg::Switch>
            traceItemNode = dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(rootNode, traceItemNodeName));
    int traceIndex = traceItemNode->getNumChildren();
    std::string type("TraceLine");

    osg::ref_ptr<osg::Switch> traceNode = new osg::Switch;
    traceNode->setName(traceLineItemName + std::to_string(traceIndex++));
    traceNode->setUserValue("itemType", type);

    std::vector<mdc::Point> points;
    std::vector<mdc::TraceLine> traceLines;

    traceLines = VectorMapSingleton::getInstance()->findByFilter([](const mdc::TraceLine &traceLine) { return true; });

    // 画线
    for (const auto &traceLine : traceLines) {
        osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;

        mdc::Point sPoint = VectorMapSingleton::getInstance()->findByID(mdc::Key<mdc::Point>(traceLine.sPID));
        mdc::Point ePoint = VectorMapSingleton::getInstance()->findByID(mdc::Key<mdc::Point>(traceLine.ePID));

        if (std::find_if(points.begin(),
                         points.end(),
                         [&sPoint](const mdc::Point &point) { return point.pID == sPoint.pID; }) == points.end()) {
            points.push_back(sPoint);
        }

        if (std::find_if(points.begin(),
                         points.end(),
                         [&ePoint](const mdc::Point &point) { return point.pID == ePoint.pID; }) == points.end()) {
            points.push_back(ePoint);
        }

        vertices->push_back(osg::Vec3d(sPoint.ly, sPoint.bx, sPoint.h));
        vertices->push_back(osg::Vec3d(ePoint.ly, ePoint.bx, ePoint.h));

        osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
        colors->push_back(traceLinesColor);

        osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
        geom->setVertexArray(vertices.get());
        geom->setColorArray(colors.get());
        geom->setColorBinding(osg::Geometry::BIND_OVERALL);
        geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, vertices->size()));

        osg::ref_ptr<osg::Geode> geode = new osg::Geode;
        geode->setName("TraceLine_" + std::to_string(traceLine.tlID));
        geode->addDrawable(geom);

        traceNode->addChild(geode);
    }

    // 画点
    for (const auto &point : points) {
        int localPointIndex = point.pID;
        osg::Vec3d localPoint(point.ly, point.bx, point.h);

        osg::ref_ptr<osg::Geode> pointGeode = new osg::Geode;
        pointGeode->setName("Sphere");
        pointGeode->setUserValue("ID", localPointIndex);
        pointGeode->setUserValue("pos", localPoint);

        osg::ref_ptr<osg::ShapeDrawable> pointSphere = new osg::ShapeDrawable(new osg::Sphere(localPoint, 0.15));
        pointSphere->setColor(traceLinesColor);
        pointGeode->addDrawable(pointSphere);
        traceNode->addChild(pointGeode);
    }

    traceItemNode->addChild(traceNode);

    // 显示点ID
    osg::ref_ptr<osg::Switch>
            pointTextNode = dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(rootNode, pointTextNodeName));
    for (const auto &point : points) {
        osg::Vec3d pos(point.ly, point.bx, point.h);
        std::string name = "TSphere_" + std::to_string(point.pID);

        osg::ref_ptr<osg::Geode> pointTextGeode = new osg::Geode;
        pointTextGeode->setName(name);

        osg::ref_ptr<osgText::Text> text = new osgText::Text;
        text->setCharacterSize(0.5);
        text->setAxisAlignment(osgText::TextBase::XY_PLANE);
        text->setPosition(pos);
        text->setText(std::to_string(point.pID));
        text->setColor(traceLinesColor);

        pointTextGeode->addDrawable(text);

        pointTextNode->addChild(pointTextGeode);
    }

    // 显示线ID
    osg::ref_ptr<osg::Switch> traceLineTextNode =
            dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(rootNode, traceLineTextNodeName));
    for (const auto &traceLine:traceLines) {
        auto sPoint = VectorMapSingleton::getInstance()->findByID(mdc::Key<mdc::Point>(traceLine.sPID));
        auto ePoint = VectorMapSingleton::getInstance()->findByID(mdc::Key<mdc::Point>(traceLine.ePID));

        osg::Vec3d pos1(sPoint.ly, sPoint.bx, sPoint.h);
        osg::Vec3d pos2(ePoint.ly, ePoint.bx, ePoint.h);
        osg::Vec3d pos = (pos1 + pos2) / 2;
        std::string name = "TTraceLine_" + std::to_string(traceLine.tlID);

        osg::ref_ptr<osg::Geode> lineTextGeode = new osg::Geode;
        lineTextGeode->setName(name);

        osg::ref_ptr<osgText::Text> text = new osgText::Text;
        text->setCharacterSize(0.5);
        text->setAxisAlignment(osgText::TextBase::XY_PLANE);
        text->setPosition(pos);
        text->setText(std::to_string(traceLine.tlID));
        text->setColor(traceLinesColor);

        lineTextGeode->addDrawable(text);

        traceLineTextNode->addChild(lineTextGeode);
    }
}

void OSGWidget::drawPavementItems() {
    osg::ref_ptr<osg::Switch> pavementItemNode = dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(rootNode,
                                                                                                              pavementItemNodeName));
    int pavementIndex = pavementItemNode->getNumChildren();
    std::string type("Pavement");

    osg::ref_ptr<osg::Switch> pavementNode = new osg::Switch;
    pavementNode->setName(pavementItemName + std::to_string(pavementIndex++));
    pavementNode->setUserValue("itemType", type);
    pavementItemNode->addChild(pavementNode);

    std::vector<mdc::Pavement> pavements;
    pavements = VectorMapSingleton::getInstance()->findByFilter([](const mdc::Pavement &pavement) { return true; });

    for (const auto &pavement : pavements) {
        std::vector<mdc::Point> points;
        mdc::Point point;
        point = VectorMapSingleton::getInstance()->findByID(mdc::Key<mdc::Point>(pavement.upperLeftCorner));
        points.push_back(point);
        point = VectorMapSingleton::getInstance()->findByID(mdc::Key<mdc::Point>(pavement.lowerLeftCorner));
        points.push_back(point);
        point = VectorMapSingleton::getInstance()->findByID(mdc::Key<mdc::Point>(pavement.upperRightCorner));
        points.push_back(point);
        point = VectorMapSingleton::getInstance()->findByID(mdc::Key<mdc::Point>(pavement.lowerRightCorner));
        points.push_back(point);
        point = VectorMapSingleton::getInstance()->findByID(mdc::Key<mdc::Point>(pavement.upperLeftCorner));
        points.push_back(point);

        // 人行道的线是逆时针链接
        osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
        for (const auto &point : points) {
            vertices->push_back(osg::Vec3d(point.ly, point.bx, point.h));
        }

        osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
        colors->push_back(pavementColor);

        osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
        geom->setVertexArray(vertices.get());
        geom->setColorArray(colors.get());
        geom->setColorBinding(osg::Geometry::BIND_OVERALL);
        geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, vertices->size()));

        osg::ref_ptr<osg::Geode> geode = new osg::Geode;
        geode->setName("Pavement_" + std::to_string(pavement.pID));
        geode->addDrawable(geom);

        pavementNode->addChild(geode);

        // 画点
        for (const auto &point : points) {
            int localPointIndex = point.pID;
            osg::Vec3d localPoint(point.ly, point.bx, point.h);

            osg::ref_ptr<osg::Geode> pointGeode = new osg::Geode;
            pointGeode->setName("Sphere");
            pointGeode->setUserValue("ID", localPointIndex);
            pointGeode->setUserValue("pos", localPoint);

            osg::ref_ptr<osg::ShapeDrawable> pointSphere = new osg::ShapeDrawable(new osg::Sphere(localPoint, 0.15f));
            pointSphere->setColor(pavementColor);
            pointGeode->addDrawable(pointSphere);
            pavementNode->addChild(pointGeode);
        }

        // 显示点ID
        osg::ref_ptr<osg::Switch> pointTextNode =
                dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(rootNode, pointTextNodeName));
        for (const auto &point : points) {
            osg::Vec3d pos(point.ly, point.bx, point.h);
            std::string name = "TSphere_" + std::to_string(point.pID);
            osg::Vec4 color(0.0, 1.0, 0.0, 0.5);

            osg::ref_ptr<osg::Geode> pointTextGeode = new osg::Geode;
            pointTextGeode->setName(name);

            osg::ref_ptr<osgText::Text> text = new osgText::Text;
            text->setCharacterSize(0.5);
            text->setAxisAlignment(osgText::TextBase::XY_PLANE);
            text->setPosition(pos);
            text->setText(std::to_string(point.pID));
            text->setColor(color);

            pointTextGeode->addDrawable(text);

            pointTextNode->addChild(pointTextGeode);
        }

        // 显示人行道ID
        osg::ref_ptr<osg::Switch> pavementTextNode = dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(
                rootNode, pavementTextNodeName));
        osg::Vec3d pos1(points[0].ly, points[0].bx, points[0].h);
        osg::Vec3d pos2(points[1].ly, points[1].bx, points[1].h);
        osg::Vec3d pos3(points[2].ly, points[2].bx, points[2].h);
        osg::Vec3d pos4(points[3].ly, points[3].bx, points[3].h);
        osg::Vec3d pos = ((pos1 + pos2) / 2 + (pos3 + pos4) / 2) / 2;
        std::string name = "TPavement_" + std::to_string(pavement.pID);

        osg::ref_ptr<osg::Geode> pavementTextGeode = new osg::Geode;
        pavementTextGeode->setName(name);

        osg::ref_ptr<osgText::Text> text = new osgText::Text;
        text->setCharacterSize(0.8);
        text->setAxisAlignment(osgText::TextBase::XY_PLANE);
        text->setPosition(pos);
        text->setText(std::to_string(pavement.pID));
        text->setColor(pavementColor);

        pavementTextGeode->addDrawable(text);

        pavementTextNode->addChild(pavementTextGeode);
    }
}

void OSGWidget::drawDrivingArrowItems() {
    osg::ref_ptr<osg::Switch> drivingArrowItemNode = dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(
            rootNode, drivingArrowItemNodeName));

    int drivingArrowIndex = drivingArrowItemNode->getNumChildren();
    std::string type("DrivingArrow");

    osg::ref_ptr<osg::Switch> drivingArrowNode = new osg::Switch;
    drivingArrowNode->setName(drivingArrowItemName + std::to_string(drivingArrowIndex++));
    drivingArrowNode->setUserValue("itemType", type);
    drivingArrowItemNode->addChild(drivingArrowNode);

    std::vector<mdc::DrivingArrow> drivingArrows;
    drivingArrows = VectorMapSingleton::getInstance()->findByFilter(
            [](const mdc::DrivingArrow &drivingArrow) { return true; });

    for (const auto &drivingArrow : drivingArrows) {
        mdc::Point point;
        point = VectorMapSingleton::getInstance()->findByID(mdc::Key<mdc::Point>(drivingArrow.centerPoint));

        // 画点
        {
            int localPointIndex = point.pID;
            osg::Vec3d localPoint(point.ly, point.bx, point.h);

            osg::ref_ptr<osg::Geode> drivingArrowGeode = new osg::Geode;
            drivingArrowGeode->setName("DrivingArrow_" + std::to_string(drivingArrow.daID));
            drivingArrowGeode->setUserValue("ID", localPointIndex);
            drivingArrowGeode->setUserValue("pos", localPoint);

            osg::ref_ptr<osg::ShapeDrawable> pointSphere = new osg::ShapeDrawable(new osg::Sphere(localPoint, 0.15f));
            pointSphere->setColor(drivingArrowColor);
            drivingArrowGeode->addDrawable(pointSphere);
            drivingArrowNode->addChild(drivingArrowGeode);
        }

        // 显示箭头ID
        osg::ref_ptr<osg::Switch> drivingArrowTextNode = dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(
                rootNode, drivingArrowTextNodeName));

        osg::Vec3d pos(point.ly, point.bx, point.h);
        std::string name = "TDrivingArrow_" + std::to_string(drivingArrow.daID);

        osg::ref_ptr<osg::Geode> drivingArrowTextGeode = new osg::Geode;
        drivingArrowTextGeode->setName(name);

        osg::ref_ptr<osgText::Text> text = new osgText::Text;
        text->setName("drivingArrowText");
        text->setCharacterSize(0.5);
        text->setAxisAlignment(osgText::TextBase::XY_PLANE);
        text->setPosition(pos);
        text->setText(drivingArrow.type);
        text->setColor(drivingArrowColor);

        drivingArrowTextGeode->addDrawable(text);

        drivingArrowTextNode->addChild(drivingArrowTextGeode);
    }
}

void OSGWidget::drawTrafficLightsItems() {
    osg::ref_ptr<osg::Switch> trafficLightsItemNode = dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(
            rootNode, trafficLightsItemNodeName));

    int trafficLightsIndex = trafficLightsItemNode->getNumChildren();
    std::string type("TrafficLights");

    osg::ref_ptr<osg::Switch> trafficLightsNode = new osg::Switch;
    trafficLightsNode->setName(trafficLightsItemName + std::to_string(trafficLightsIndex++));
    trafficLightsNode->setUserValue("itemType", type);
    trafficLightsItemNode->addChild(trafficLightsNode);

    std::vector<mdc::TrafficLights> trafficLights;
    trafficLights = VectorMapSingleton::getInstance()->findByFilter(
            [](const mdc::TrafficLights &trafficLights) { return true; });

    for (const auto &trafficLight : trafficLights) {
        mdc::Point point;
        point = VectorMapSingleton::getInstance()->findByID(mdc::Key<mdc::Point>(trafficLight.lightLocationPoint));

        {
            int localPointIndex = point.pID;
            osg::Vec3d localPoint(point.ly, point.bx, point.h);

            osg::ref_ptr<osg::Geode> trafficLightsGeode = new osg::Geode;
            trafficLightsGeode->setName("TrafficLights_" + std::to_string(trafficLight.tlID));
            trafficLightsGeode->setUserValue("ID", localPointIndex);
            trafficLightsGeode->setUserValue("pos", localPoint);

            osg::ref_ptr<osg::ShapeDrawable> pointSphere = new osg::ShapeDrawable(new osg::Sphere(localPoint, 0.15f));
            pointSphere->setColor(trafficLightsColor);
            trafficLightsGeode->addDrawable(pointSphere);
            trafficLightsNode->addChild(trafficLightsGeode);
        }

        // 显示交通信号灯ID
        osg::ref_ptr<osg::Switch> trafficLightsTextNode = dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(
                rootNode, trafficLightsTextNodeName));

        osg::Vec3d pos(point.ly, point.bx, point.h);
        std::string name = "TTrafficLights_" + std::to_string(trafficLight.tlID);

        osg::ref_ptr<osg::Geode> trafficLightsTextGeode = new osg::Geode;
        trafficLightsTextGeode->setName(name);

        osg::ref_ptr<osgText::Text> text = new osgText::Text;
        text->setCharacterSize(0.5);
        text->setAxisAlignment(osgText::TextBase::XY_PLANE);
        text->setPosition(pos);
        text->setText(std::to_string(trafficLight.tlID));
        text->setColor(trafficLightsColor);

        trafficLightsTextGeode->addDrawable(text);

        trafficLightsTextNode->addChild(trafficLightsTextGeode);

    }
}

void OSGWidget::drawRoadLinesItems() {
    osg::ref_ptr<osg::Switch> roadLinesItemNode = dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(rootNode,
                                                                                                               roadLinesItemNodeName));
    int roadLinesIndex = roadLinesItemNode->getNumChildren();
    std::string type("RoadLines");

    osg::ref_ptr<osg::Switch> roadLinesNode = new osg::Switch;
    roadLinesNode->setName(roadLinesItemName + std::to_string(roadLinesIndex++));
    roadLinesNode->setUserValue("itemType", type);
    roadLinesItemNode->addChild(roadLinesNode);

    std::vector<mdc::RoadLines> roadLines;
    roadLines = VectorMapSingleton::getInstance()->findByFilter([](const mdc::RoadLines &roadLine) { return true; });

    for (const auto &roadLine : roadLines) {
        std::vector<mdc::Point> points;
        mdc::Point point;

        for (const auto &id : roadLine.allPointsID) {
            point = VectorMapSingleton::getInstance()->findByID(mdc::Key<mdc::Point>(id));
            points.push_back(point);
        }

        //
        osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
        for (const auto &point : points) {
            vertices->push_back(osg::Vec3d(point.ly, point.bx, point.h));
        }

        osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
        colors->push_back(roadLinesColor);

        osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
        geom->setVertexArray(vertices.get());
        geom->setColorArray(colors.get());
        geom->setColorBinding(osg::Geometry::BIND_OVERALL);
        geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, vertices->size()));

        osg::ref_ptr<osg::Geode> geode = new osg::Geode;
        geode->setName("RoadLines_" + std::to_string(roadLine.rlID));
        geode->addDrawable(geom);

        roadLinesNode->addChild(geode);

        // 画点
        for (const auto &point : points) {
            int localPointIndex = point.pID;
            osg::Vec3d localPoint(point.ly, point.bx, point.h);

            osg::ref_ptr<osg::Geode> pointGeode = new osg::Geode;
            pointGeode->setName("Sphere");
            pointGeode->setUserValue("ID", localPointIndex);
            pointGeode->setUserValue("pos", localPoint);

            osg::ref_ptr<osg::ShapeDrawable> pointSphere = new osg::ShapeDrawable(new osg::Sphere(localPoint, 0.15f));
            pointSphere->setColor(roadLinesColor);
            pointGeode->addDrawable(pointSphere);
            roadLinesNode->addChild(pointGeode);
        }

        // 显示点ID
        osg::ref_ptr<osg::Switch> pointTextNode = dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(rootNode,
                                                                                                               pointTextNodeName));
        for (const auto &point : points) {
            osg::Vec3d pos(point.ly, point.bx, point.h);
            std::string name = "TSphere_" + std::to_string(point.pID);

            osg::ref_ptr<osg::Geode> pointTextGeode = new osg::Geode;
            pointTextGeode->setName(name);

            osg::ref_ptr<osgText::Text> text = new osgText::Text;
            text->setCharacterSize(0.5);
            text->setAxisAlignment(osgText::TextBase::XY_PLANE);
            text->setPosition(pos);
            text->setText(std::to_string(point.pID));
            text->setColor(roadLinesColor);

            pointTextGeode->addDrawable(text);

            pointTextNode->addChild(pointTextGeode);
        }
    }
}

void OSGWidget::clearIrrelevantPiont() {
    std::vector<mdc::Point> points;
    points = VectorMapSingleton::getInstance()->findByFilter([](const mdc::Point &point) { return true; });
    std::set<size_t> pointIDs;

    for (auto it = points.begin(); it != points.end();) {
        if (!(*it).fromPointLineID.empty() || !(*it).toPointLineID.empty()) {
            it = points.erase(it);
        } else {
            ++it;
        }
    }

    for (const auto &point : points) {
        pointIDs.insert(point.pID);
    }

    osg::ref_ptr<osg::Switch> vectorItemNode =
            dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(rootNode, vectorItemNodeName));
    osg::ref_ptr<osg::Switch> itemVectorNode = dynamic_cast<osg::Switch *>(vectorItemNode->getChild(0));
    osg::ref_ptr<osg::Geode> childNode;
    int ID;
    for (int i = 0; i < itemVectorNode->getNumChildren(); ++i) {
        childNode = dynamic_cast<osg::Geode *>(itemVectorNode->getChild(i));
        childNode->getUserValue("ID", ID);
        if (pointIDs.find(ID) != pointIDs.end()) {
            itemVectorNode->removeChild(childNode);
            i = -1;
        }
    }

    osg::ref_ptr<osg::Switch>
            pointTextNode = dynamic_cast<osg::Switch *>(NodeTreeSearch::findNodeWithName(rootNode, pointTextNodeName));
    for (int i = 0; i < pointTextNode->getNumChildren(); ++i) {
        childNode = dynamic_cast<osg::Geode *>(pointTextNode->getChild(i));
        if (childNode->getName().compare(0, 8, "TSphere_") == 0) {
            size_t deleteSphereID = std::stoi(childNode->getName().substr(8));
            if (pointIDs.find(deleteSphereID) != pointIDs.end()) {
                pointTextNode->removeChild(childNode);
                i = -1;
            }
        }
    }

}

template<class T>
void OSGWidget::drawVectorItems(const std::vector<T> &objects) {}

void OSGWidget::updateFrame() {
    QWidget::update();
}