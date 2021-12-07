#include "servicecore.h"
#include <QDebug>

ServiceCore::ServiceCore(QObject *parent) : QObject(parent), _satellitesInUse(0), _speedLimit(100), _notifySignalType(1)
{    
    qDebug()<<"ServiceCore";

    _srcNode = new QRemoteObjectHost(QUrl(QStringLiteral("local:replica")), this);
    _serviceMessenger = new ServiceMessenger();


    _settings = new QSettings("SpeedLimitNotifier.conf", QSettings::IniFormat, this);


    _delayInitTimer = new QTimer(this); //-- После инициализации пождём немного, что бы дать всему просраться на Андроиде

    connect(_delayInitTimer, &QTimer::timeout, this, &ServiceCore::onServiceStarted);

    _soundOverSpeed = new QSound(":/Assets/Sounds/bell-ringing-04.wav", this);

    _soundRepeater = new QTimer(this);
    connect(_soundRepeater, &QTimer::timeout, this, &ServiceCore::soundPlay);

}

/**
* @brief Подготавливаем всё необходимое
* @return
*/
bool ServiceCore::init()
{
     _srcNode->enableRemoting(_serviceMessenger);

     _delayInitTimer->setInterval(10000);
     _delayInitTimer->setSingleShot(true);
     _delayInitTimer->start();

     _soundRepeater->setSingleShot(false);
     _soundRepeater->setInterval(3000);

     return true;
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

    if ( _satellitesInUse>=3 && _speedLimit>0 && speed>_speedLimit ) {
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
    if ( _notifySignalType==NST_FILE ) {
        _soundOverSpeed->play();
    } else
    if ( _notifySignalType==NST_TONE ) {
        QAndroidJniObject service = QtAndroid::androidService();
        if ( service.isValid() ) {
            service.callMethod<void>("notifySignal");
        } else {
            qWarning()<<"Service activity not valid O_o !";
            return;
        }
    } else {
        return;
    }

    _soundRepeater->start();
}

void ServiceCore::soundStop()
{
    if ( _notifySignalType==NST_FILE ) {
        _soundOverSpeed->stop();
    }

    _soundRepeater->stop();
}

void ServiceCore::onSettingsChanged()
{
    qDebug()<<"onSettingsChanged";

    _settings->sync();
    _speedLimit = _settings->value("speedLimit", 100.0).toDouble();
    _notifySignalType = _settings->value("notifySignalType", 1).toInt();

}

/**
* @brief Сервис запущен
*/
void ServiceCore::onServiceStarted()
{
    qDebug()<<"onServiceStarted";

    onSettingsChanged();
    connect(_serviceMessenger, &ServiceMessenger::settingsChanged, this, &ServiceCore::onSettingsChanged);

    _geoSat = QGeoSatelliteInfoSource::createDefaultSource(this);
    connect(_geoSat, &QGeoSatelliteInfoSource::satellitesInUseUpdated, this, &ServiceCore::onSatellitesUpdated);

    _geoSat->setUpdateInterval(5000);

    _gepPos = QGeoPositionInfoSource::createDefaultSource(this);

    connect(_gepPos, QOverload<QGeoPositionInfoSource::Error>::of(&QGeoPositionInfoSource::error), [=](QGeoPositionInfoSource::Error positioningError){ qWarning()<<"GPS error"<<positioningError; });

    if ( _gepPos ) {
        int supportedPosMethods = _gepPos->supportedPositioningMethods();

        if ( supportedPosMethods!=QGeoPositionInfoSource::NoPositioningMethods ) {

            if ( supportedPosMethods & QGeoPositionInfoSource::SatellitePositioningMethods ) { qDebug()<<"---Satellite"; }
            if ( supportedPosMethods & QGeoPositionInfoSource::NonSatellitePositioningMethods ) { qDebug()<<"---Non satellite"; }
            if ( supportedPosMethods & QGeoPositionInfoSource::AllPositioningMethods ) { qDebug()<<"---Satellite + Non satellite"; }

            connect(_gepPos, &QGeoPositionInfoSource::positionUpdated, this, &ServiceCore::onPositionUpdated);

            _gepPos->setUpdateInterval(1000);

            _geoSat->startUpdates();
            _gepPos->startUpdates();

         } else {
             emit _serviceMessenger->serviceErrored(ServiceErrors::SE_NO_GPS_DEVICES);
            return;
         }
    } else {
        emit _serviceMessenger->serviceErrored(ServiceErrors::SE_NO_GPS_ACCESS);
        return;
     }

    emit _serviceMessenger->serviceStarted();

}


