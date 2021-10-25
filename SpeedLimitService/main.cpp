#include <QAndroidService>
#include <QDebug>

#include "servicecore.h"



int main(int argc, char *argv[])
{
    QAndroidService app(argc, argv);

    qWarning()<<"SpeedLimitService started!";

    ServiceCore * serviceCore = new ServiceCore(nullptr);
    serviceCore->init();

    return app.exec();
}
