#ifndef SERVICEMESSENGER_REP_H
#define SERVICEMESSENGER_REP_H

#include <QList>
#include <QGeoSatelliteInfo>
#include <QGeoPositionInfo>

Q_DECLARE_METATYPE(QGeoSatelliteInfo)
Q_DECLARE_METATYPE(QList<QGeoSatelliteInfo>)

enum ServiceErrors {
    SE_NONE,
    SE_NO_GPS_DEVICES,
    SE_NO_GPS_ACCESS
};


#endif // SERVICEMESSENGER_REP_H
