#include "appcore.h"

#include <QtAndroid>
#include <QAndroidIntent>
#include <QRemoteObjectNode>
#include <QDebug>

#include "rep_servicemessenger_replica.h"


AppCore::AppCore(QObject *parent):
    QObject(parent), _speed(0.0), _satellitesCount(-1), _limit(100),
    _enabled(false), _lastErrorCode(ServiceErrors::SE_NONE), _lastErrorText("")
{
    _repNode.connectToNode(QUrl(QStringLiteral("local:replica")));
    _messenger = QSharedPointer<ServiceMessengerReplica>(_repNode.acquire<ServiceMessengerReplica>());

    QObject::connect(_messenger.data(), &ServiceMessengerReplica::positionUpdated, this, &AppCore::onPositionUpdated);
    QObject::connect(_messenger.data(), &ServiceMessengerReplica::satellitesUpdated, this, &AppCore::onSatellitesUpdated);

    _settings = new QSettings("SpeedLimitNotifier.conf", QSettings::IniFormat, this);

    setLimit(_settings->value("speedLimit", 100.0).toDouble());
    setEnabled(_settings->value("serviceEnabled", false).toBool());
}

//--GETTERS-------------------------------------------------------------------------

double AppCore::speed() const
{
    return _speed;
}


int AppCore::satellitesCount() const
{
    return _satellitesCount;
}

double AppCore::limit() const
{
    return _limit;
}

bool AppCore::enabled() const
{
    return _enabled;
}

const QString& AppCore::lastErrorText() const
{
    return _lastErrorText;
}

//--SETTERS-------------------------------------------------------------------------

void AppCore::setSpeed(double newSpeed)
{
    if ( qFuzzyCompare(_speed, newSpeed) ) { return; }
    _speed = newSpeed;
    emit speedChanged();
}

void AppCore::setSatellitesCount(int satellitesCount)
{
    if ( _satellitesCount==satellitesCount ) { return; }
    _satellitesCount = satellitesCount;
    emit satellitesCountChanged();
}

void AppCore::setLimit(double newLimit)
{
    if ( qFuzzyCompare(_limit, newLimit) ) { return; }
    _limit = newLimit;

    _settings->setValue("speedLimit", _limit);
    _settings->sync();

    emit limitChanged();
    _messenger->changeSettings();
}

void AppCore::setEnabled(bool newEnabled)
{
    if (_enabled == newEnabled) { return; }
    _enabled = newEnabled;

    if ( _enabled ) {

        setLastError(ServiceErrors::SE_NONE); //-- Ну, попробуем сбросить и начать заново.

        QtAndroid::PermissionResult hasPermLocation = QtAndroid::checkPermission(QString("android.permission.ACCESS_FINE_LOCATION"));
        if( hasPermLocation==QtAndroid::PermissionResult::Denied ) {
            QtAndroid::PermissionResultMap resultHash = QtAndroid::requestPermissionsSync(QStringList({"android.permission.ACCESS_FINE_LOCATION"}));
            if (resultHash["android.permission.ACCESS_FINE_LOCATION"] == QtAndroid::PermissionResult::Denied ) {
                _enabled = false;
                emit enabledChanged();
                return;
            }
        }

        QAndroidJniObject::callStaticMethod<void>("ru/pavelk/SpeedLimitNotifier/SpeedLimitNotifierService", "toStartService", "(Landroid/content/Context;)V", QtAndroid::androidActivity().object());

        bool res = _messenger->waitForSource();

        if ( !res ) { //TODO: Выяснить, когда это возможно и решить что с этим делать

        }
    } else {
        QAndroidJniObject::callStaticMethod<void>("ru/pavelk/SpeedLimitNotifier/SpeedLimitNotifierService", "toStopService", "(Landroid/content/Context;)V", QtAndroid::androidActivity().object());
    }

    _settings->setValue("serviceEnabled", _enabled);
    _settings->sync();


    emit enabledChanged();
    _messenger->changeSettings();
}

void AppCore::setLastError(int code)
{
    _lastErrorCode = code;

    switch (code) {
        case ServiceErrors::SE_NO_GPS_ACCESS: _lastErrorText = tr("Has no GPS access!"); break;
        case ServiceErrors::SE_NO_GPS_DEVICES: _lastErrorText = tr("Has no GPS devices!"); break;
        case ServiceErrors::SE_NONE: default: _lastErrorText = "";  break;
    }

    emit lastErrorChanged();
}


//---------------------------------------------------------------------------


/**
* @brief Сервис обновил инфу о спутниках
* @param satInfo
*/
void AppCore::onSatellitesUpdated(const QList<QGeoSatelliteInfo>& satInfo)
{
    setSatellitesCount(satInfo.count());
}

/**
* @brief Сервис обновил инфу о текущем местоположении
* @param posInfo
*/
void AppCore::onPositionUpdated(const QGeoPositionInfo& posInfo)
{
    double speed = posInfo.attribute(QGeoPositionInfo::GroundSpeed); //-- Значение в м/с
    if ( speed!=speed ) { return; }
    setSpeed(speed*3.6);
}

/**
* @brief Сервису поплохело
* @param code
*/
void AppCore::onServiceErrored(int code) //TODO: Вывод сообщения ошибки в UI
{
    setLastError(code);
    setEnabled(false);
}

