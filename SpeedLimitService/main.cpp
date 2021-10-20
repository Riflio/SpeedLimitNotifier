#include <QAndroidService>
#include <QDebug>

int main(int argc, char *argv[])
{
    QAndroidService app(argc, argv);

    qWarning()<<"SpeedLimitService started!";

    return app.exec();
}
