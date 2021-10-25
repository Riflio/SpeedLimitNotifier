#ifndef APPCORE_H
#define APPCORE_H

#include <QObject>
#include <QRemoteObjectNode>
#include <QSettings>
#include "rep_servicemessenger_replica.h"

class AppCore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int satellitesCount READ satellitesCount  NOTIFY satellitesCountChanged)
    Q_PROPERTY(double speed READ speed WRITE setSpeed NOTIFY speedChanged)
    Q_PROPERTY(double limit READ limit WRITE setLimit NOTIFY limitChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString lastErrorText READ lastErrorText NOTIFY lastErrorChanged)

public:
    explicit AppCore(QObject *parent = nullptr);

    double speed() const;
    void setSpeed(double newSpeed);

    int satellitesCount() const;
    void setSatellitesCount(int satellitesCount);

    double limit() const;
    void setLimit(double newLimit);

    bool enabled() const;
    void setEnabled(bool newEnabled);

    const QString& lastErrorText() const;
    void setLastError(int code);

signals:
    void speedChanged();
    void satellitesCountChanged();
    void limitChanged();
    void enabledChanged();
    void lastErrorChanged();

public slots:


private slots:
    void onSatellitesUpdated(const QList<QGeoSatelliteInfo> &satInfo);
    void onPositionUpdated(const QGeoPositionInfo &posInfo);
    void onServiceErrored(int code);

private:
    QRemoteObjectNode _repNode;
    QSharedPointer<ServiceMessengerReplica> _messenger;
    double _speed;
    int _satellitesCount;
    double _limit;
    QSettings * _settings;
    bool _enabled;

    int _lastErrorCode;
    QString _lastErrorText;
};

#endif // APPCORE_H
