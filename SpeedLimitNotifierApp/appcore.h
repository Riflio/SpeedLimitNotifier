#ifndef APPCORE_H
#define APPCORE_H

#include <QObject>
#include <QRemoteObjectNode>
#include <QSettings>
#include <QSound>
#include "rep_servicemessenger_replica.h"

class AppCore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int satellitesCount READ satellitesCount  NOTIFY satellitesCountChanged)
    Q_PROPERTY(double speed READ speed WRITE setSpeed NOTIFY speedChanged)
    Q_PROPERTY(double limit READ limit WRITE setLimit NOTIFY limitChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString lastErrorText READ lastErrorText NOTIFY lastErrorChanged)
    Q_PROPERTY(int notifySignalType READ notifySignalType WRITE setNotifySignalType NOTIFY notifySignalTypeChanged)
    Q_PROPERTY(bool autoload READ autoload WRITE setAutoload NOTIFY autoloadChanged)

    Q_ENUMS(ENotifySignalTypes);

public:
    explicit AppCore(QObject *parent = nullptr);

    enum ENotifySignalTypes {
        NST_NONE = 0,
        NST_TONE = 1,
        NST_FILE = 2
    };


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

    int notifySignalType() const;
    void setNotifySignalType(int newNotifySignalType);

    bool autoload() const;
    void setAutoload(bool newAutoload);

signals:
    void speedChanged();
    void satellitesCountChanged();
    void limitChanged();
    void enabledChanged();
    void lastErrorChanged();
    void notifySignalTypeChanged();
    void autoloadChanged();

public slots:
    void notifySignalTest();

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
    int _notifySignalType;
    QSound * _notifySignal;
    bool _autoload;
};

#endif // APPCORE_H
