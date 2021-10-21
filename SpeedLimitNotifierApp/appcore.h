#ifndef APPCORE_H
#define APPCORE_H

#include <QObject>
#include <QRemoteObjectNode>
#include "rep_servicemessenger_replica.h"

class AppCore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool hasSatellites READ hasSatellites WRITE setHasSatellites NOTIFY hasSatellitesChanged)
    Q_PROPERTY(double speed READ speed WRITE setSpeed NOTIFY speedChanged)

public:
    explicit AppCore(QObject *parent = nullptr);

    double speed() const;
    void setSpeed(double newSpeed);

    bool hasSatellites() const;
    void setHasSatellites(bool newHasSatelites);

signals:
    void speedChanged();
    void hasSatellitesChanged();

public slots:
    void startService();

private slots:
    void onSatellitesUpdated(const QList<QGeoSatelliteInfo> &satInfo);
    void onPositionUpdated(const QGeoPositionInfo &posInfo);

private:
    QRemoteObjectNode _repNode;
    QSharedPointer<ServiceMessengerReplica> _messenger;
    double _speed;
    bool _hasSatellites;
};

#endif // APPCORE_H
