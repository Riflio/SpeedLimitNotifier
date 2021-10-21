#include "appcore.h"

#include <QtAndroid>
#include <QAndroidIntent>
#include <QRemoteObjectNode>
#include <QDebug>

#include "rep_servicemessenger_replica.h"


AppCore::AppCore(QObject *parent) : QObject(parent), _speed(0.0), _hasSatellites(false)
{

    _repNode.connectToNode(QUrl(QStringLiteral("local:replica")));
    _messenger =  QSharedPointer<ServiceMessengerReplica>(_repNode.acquire<ServiceMessengerReplica>());

    QObject::connect(_messenger.data(), &ServiceMessengerReplica::positionUpdated, this, &AppCore::onPositionUpdated);
    QObject::connect(_messenger.data(), &ServiceMessengerReplica::satellitesUpdated, this, &AppCore::onSatellitesUpdated);

}

//--GETTERS-------------------------------------------------------------------------

double AppCore::speed() const
{
    return _speed;
}


bool AppCore::hasSatellites() const
{
    return _hasSatellites;
}

//--SETTERS-------------------------------------------------------------------------

void AppCore::setSpeed(double newSpeed)
{
    if ( qFuzzyCompare(_speed, newSpeed) ) { return; }
    _speed = newSpeed;
    emit speedChanged();
}

void AppCore::setHasSatellites(bool newHasSatelites)
{
    if ( _hasSatellites==newHasSatelites ) { return; }
    _hasSatellites = newHasSatelites;
    emit hasSatellitesChanged();
}

//---------------------------------------------------------------------------

/**
* @brief Запускаем фоновый сервис
*/
void AppCore::startService()
{
    qInfo()<<"Starting service...";


    QAndroidIntent serviceIntent(QtAndroid::androidActivity().object(),"ru.pavelk.SpeedLimitNotifier.SpeedLimitNotifierService");
    QAndroidJniObject result = QtAndroid::androidActivity().callObjectMethod("startService", "(Landroid/content/Intent;)Landroid/content/ComponentName;", serviceIntent.handle().object());

    bool res = _messenger->waitForSource();

    if ( !res ) {

    }

}

/**
* @brief Сервис обновил инфу о спутниках
* @param satInfo
*/
void AppCore::onSatellitesUpdated(const QList<QGeoSatelliteInfo>& satInfo)
{
    qDebug()<<"Sat updated, count"<<satInfo.count();

    setHasSatellites((satInfo.count()>0));
}

/**
* @brief Сервис обновил инфу о текущем местоположении
* @param posInfo
*/
void AppCore::onPositionUpdated(const QGeoPositionInfo& posInfo)
{
    qDebug()<<"Pos updated";
    double speed = posInfo.attribute(QGeoPositionInfo::GroundSpeed); //-- Значение в м/с

    setSpeed(speed*3.6);
}


