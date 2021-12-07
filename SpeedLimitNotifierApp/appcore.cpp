#include "appcore.h"

#include <QtAndroid>
#include <QAndroidIntent>
#include <QRemoteObjectNode>
#include <QDebug>

#include "rep_servicemessenger_replica.h"


AppCore::AppCore(QObject *parent):
    QObject(parent), _speed(0.0), _satellitesCount(-1), _limit(100),
    _enabled(false), _lastErrorCode(ServiceErrors::SE_NONE), _lastErrorText(""),
    _notifySignalType(1), _notifySignal(nullptr)
{
    _repNode.connectToNode(QUrl(QStringLiteral("local:replica")));
    _messenger = QSharedPointer<ServiceMessengerReplica>(_repNode.acquire<ServiceMessengerReplica>());

    QObject::connect(_messenger.data(), &ServiceMessengerReplica::positionUpdated, this, &AppCore::onPositionUpdated);
    QObject::connect(_messenger.data(), &ServiceMessengerReplica::satellitesUpdated, this, &AppCore::onSatellitesUpdated);

    _notifySignal = new QSound("", this);

    _settings = new QSettings("SpeedLimitNotifier.conf", QSettings::IniFormat, this);

    //-- Убедимся, что есть доступ для записи файла настроек
    QtAndroid::PermissionResult hasPermSettingsWrite = QtAndroid::checkPermission(QString("android.permission.WRITE_EXTERNAL_STORAGE"));
    if( hasPermSettingsWrite==QtAndroid::PermissionResult::Denied ) {
        QtAndroid::PermissionResultMap resultHash = QtAndroid::requestPermissionsSync(QStringList({"android.permission.WRITE_EXTERNAL_STORAGE"}));
        if ( resultHash["android.permission.WRITE_EXTERNAL_STORAGE"] == QtAndroid::PermissionResult::Denied ) {
            setLastError(ServiceErrors::SE_NO_SETTINGS_WRITE);
        }
    }

    setLimit(_settings->value("speedLimit", 100.0).toDouble());
    setEnabled(_settings->value("serviceEnabled", false).toBool());
    setNotifySignalType(_settings->value("notifySignalType", 1).toInt());
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

int AppCore::notifySignalType() const
{
    return _notifySignalType;
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
            if ( resultHash["android.permission.ACCESS_FINE_LOCATION"] == QtAndroid::PermissionResult::Denied ) {
                _enabled = false;
                emit enabledChanged();
                setLastError(ServiceErrors::SE_NO_GPS_ACCESS);
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
        case ServiceErrors::SE_NO_GPS_ACCESS: _lastErrorText = tr("Has no GPS access permission!"); break;
        case ServiceErrors::SE_NO_GPS_DEVICES: _lastErrorText = tr("Has no finded GPS devices!"); break;
        case ServiceErrors::SE_NO_SETTINGS_WRITE: _lastErrorText = tr("Has no storage write permission!"); break;
        case ServiceErrors::SE_NONE: default: _lastErrorText = "";  break;
    }

    emit lastErrorChanged();
}

void AppCore::setNotifySignalType(int newNotifySignalType)
{
    if ( _notifySignalType==newNotifySignalType ) { return; }
    _notifySignalType = newNotifySignalType;

    _settings->setValue("notifySignalType", _notifySignalType);
    _settings->sync();

    emit notifySignalTypeChanged();
}

//---------------------------------------------------------------------------

/**
* @brief Проиграем тестовый звук
*/
void AppCore::notifySignalTest()
{
    if ( _notifySignalType==NST_TONE ) {
        QAndroidJniObject activity = QtAndroid::androidActivity();
        activity.callMethod<void>("testNotifySignalTone");
    } else
    if ( _notifySignalType==NST_FILE ) {
        _notifySignal->play("qrc:/Assets/Sounds/bell-ringing-04.wav");
    }
}

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


