#include "appcore.h"

#include <QtAndroid>
#include <QAndroidIntent>
#include <QDebug>


AppCore::AppCore(QObject *parent) : QObject(parent)
{

}

void AppCore::startService()
{
    qInfo()<<"Starting service...";

    QAndroidIntent serviceIntent(QtAndroid::androidActivity().object(),"ru.pavelk.SpeedLimitNotifier.SpeedLimitNotifierService");
    QAndroidJniObject result = QtAndroid::androidActivity().callObjectMethod("startService", "(Landroid/content/Intent;)Landroid/content/ComponentName;", serviceIntent.handle().object());
}
