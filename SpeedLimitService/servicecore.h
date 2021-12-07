#ifndef SERVICECORE_H
#define SERVICECORE_H

#include <QObject>
#include <QtAndroid>
#include <QGeoSatelliteInfoSource>
#include <QGeoPositionInfoSource>
#include <QRemoteObjectHost>
#include <QTimer>
#include <QSound>
#include <QSettings>

#include "servicemessenger.h"

class ServiceCore : public QObject
{
    Q_OBJECT
public:
    explicit ServiceCore(QObject *parent = nullptr);

    enum ENotifySignalTypes {
        NST_NONE = 0,
        NST_TONE = 1,
        NST_FILE = 2
    };

    bool init();

signals:

private slots:
    void onServiceStarted();
    void onSatellitesUpdated(const QList<QGeoSatelliteInfo> &satInfo);
    void onPositionUpdated(const QGeoPositionInfo& posInfo);
    void soundPlay();
    void soundStop();
    void onSettingsChanged();

private:
    QGeoSatelliteInfoSource * _geoSat;
    QGeoPositionInfoSource * _gepPos;

    QTimer * _delayInitTimer;

    QRemoteObjectHost * _srcNode;
    ServiceMessenger * _serviceMessenger;

    QTimer * _soundRepeater;
    QSound * _soundOverSpeed;

    QGeoPositionInfo _lastGeoInfo;
    int _satellitesInUse;

    double _speedLimit;
    int _notifySignalType;
    QSettings * _settings;
};

#endif // SERVICECORE_H
