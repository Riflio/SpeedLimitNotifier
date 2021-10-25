#ifndef SERVICEMESSENGER_H
#define SERVICEMESSENGER_H


#include "rep_servicemessenger_source.h"

class ServiceMessenger : public ServiceMessengerSource
{
Q_OBJECT

signals:
    void settingsChanged();

public slots:
    void changeSettings() override {
        emit settingsChanged();
    }

};


#endif // SERVICEMESSENGER_H
