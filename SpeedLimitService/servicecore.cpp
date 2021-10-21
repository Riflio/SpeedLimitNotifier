#include "servicecore.h"



#include <QDebug>

ServiceCore::ServiceCore(QObject *parent) : QObject(parent), _satellitesInUse(0)
{    
    qDebug()<<"ServiceCore";

    _srcNode = new QRemoteObjectHost(QUrl(QStringLiteral("local:replica")), this);
    _serviceMessenger = new ServiceMessenger();
    _srcNode->enableRemoting(_serviceMessenger);


    _t1 = new QTimer(this);

    connect(_t1, &QTimer::timeout, this, &ServiceCore::onServiceStarted);

    _t1->setInterval(4000);
    _t1->setSingleShot(true);
    _t1->start();

    _soundOverSpeed = new QSound(":/Assets/Sounds/bell-ringing-04.wav", this);

    _soundRepeater = new QTimer(this);
    _soundRepeater->setSingleShot(false);
    _soundRepeater->setInterval(3000);
    connect(_soundRepeater, &QTimer::timeout, this, &ServiceCore::soundPlay);

}

/**
* @brief Обновился список использующихся спутников
* @param satInfo
*/
void ServiceCore::onSatellitesUpdated(const QList<QGeoSatelliteInfo>& satInfo)
{
    _satellitesInUse = satInfo.count();

    qInfo()<<"Satellites in use count:"<<_satellitesInUse;

    _serviceMessenger->satellitesUpdated(satInfo);
}

/**
* @brief Обновилась инфа о положении
* @param posInfo
*/
void ServiceCore::onPositionUpdated(const QGeoPositionInfo &posInfo)
{
    _lastGeoInfo = posInfo;

    qDebug()<<"Position updated:"<<_lastGeoInfo;
    qInfo()<<"Speed m/s:"<< _lastGeoInfo.attribute(QGeoPositionInfo::GroundSpeed);

    _serviceMessenger->positionUpdated(_lastGeoInfo);


    double speed = _lastGeoInfo.attribute(QGeoPositionInfo::GroundSpeed) * 3.6;

    if ( _satellitesInUse>1 && speed>40 ) {
        soundPlay();
    } else {
        soundStop();
    }
}

/**
* @brief Включаем звук превышения скорости
*/
void ServiceCore::soundPlay()
{
    qDebug()<<"soundPlay";
    _soundOverSpeed->play();
    _soundRepeater->start();
}

void ServiceCore::soundStop()
{
    _soundOverSpeed->stop();
    _soundRepeater->stop();
}

/**
* @brief Сервис запущен
*/
void ServiceCore::onServiceStarted()
{
    qDebug()<<"onServiceStarted";

    QtAndroid::PermissionResult hasPermLocation = QtAndroid::checkPermission(QString("android.permission.ACCESS_FINE_LOCATION"));
    if( hasPermLocation==QtAndroid::PermissionResult::Denied ) {
        QtAndroid::PermissionResultMap resultHash = QtAndroid::requestPermissionsSync(QStringList({"android.permission.ACCESS_FINE_LOCATION"}));
        if (resultHash["android.permission.ACCESS_FINE_LOCATION"] == QtAndroid::PermissionResult::Denied ) {
            qWarning()<<"Denied GPS access!";
        }
    }


    _geoSat = QGeoSatelliteInfoSource::createDefaultSource(this);
    connect(_geoSat, &QGeoSatelliteInfoSource::satellitesInUseUpdated, this, &ServiceCore::onSatellitesUpdated);

    _geoSat->setUpdateInterval(5000);



    _gepPos = QGeoPositionInfoSource::createDefaultSource(this);

    connect(_gepPos, QOverload<QGeoPositionInfoSource::Error>::of(&QGeoPositionInfoSource::error), [=](QGeoPositionInfoSource::Error positioningError){ qWarning()<<"GPS error"<<positioningError; });

    if ( _gepPos ) {

        qDebug()<<"Supported position methods:";
        int supportedPosMethods = _gepPos->supportedPositioningMethods();

        if ( supportedPosMethods!=QGeoPositionInfoSource::NoPositioningMethods ) {

            if ( supportedPosMethods & QGeoPositionInfoSource::SatellitePositioningMethods ) { qDebug()<<"---Satellite"; }
            if ( supportedPosMethods & QGeoPositionInfoSource::NonSatellitePositioningMethods ) { qDebug()<<"---Non satellite"; }
            if ( supportedPosMethods & QGeoPositionInfoSource::AllPositioningMethods ) { qDebug()<<"---Satellite + Non satellite"; }

            qDebug()<<"Go connect";
            connect(_gepPos, &QGeoPositionInfoSource::positionUpdated, this, &ServiceCore::onPositionUpdated);


            qDebug()<<"Go start updating. Min interval:"<<_gepPos->minimumUpdateInterval();

            _gepPos->setUpdateInterval(1000);

            _geoSat->startUpdates();
            _gepPos->startUpdates();

            qDebug()<<"Start GPS updating...";

         } else {
             qDebug()<<"--- has no supported methods"<<supportedPosMethods;
         }
    } else { //TODO: Error to UI
         qWarning()<<"Unable access to GPS component.";
     }



}


