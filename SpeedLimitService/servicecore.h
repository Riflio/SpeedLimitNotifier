#ifndef SERVICECORE_H
#define SERVICECORE_H

#include <QObject>
#include <QtAndroid>
#include <QGeoSatelliteInfoSource>
#include <QGeoPositionInfoSource>
#include <QRemoteObjectHost>
#include <QTimer>
#include <QSound>

#include "servicemessenger.h"

class ServiceCore : public QObject
{
    Q_OBJECT
public:
    explicit ServiceCore(QObject *parent = nullptr);

signals:

private slots:
    void onServiceStarted();
    void onSatellitesUpdated(const QList<QGeoSatelliteInfo> &satInfo);
    void onPositionUpdated(const QGeoPositionInfo& posInfo);
    void soundPlay();
    void soundStop();

private:
    QGeoSatelliteInfoSource * _geoSat;
    QGeoPositionInfoSource * _gepPos;

    QTimer * _t1;

    QRemoteObjectHost * _srcNode;
    ServiceMessenger * _serviceMessenger;

    QTimer * _soundRepeater;

    QSound * _soundOverSpeed;

    QGeoPositionInfo _lastGeoInfo;

    int _satellitesInUse;
};

#endif // SERVICECORE_H
