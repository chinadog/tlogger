#include "tlogger.h"

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context);
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        TDebug().debug() << localMsg.constData();
        break;
    case QtInfoMsg:
        TDebug().info() << localMsg.constData();
        break;
    case QtWarningMsg:
        TDebug().warning() << localMsg.constData();
        break;
    case QtCriticalMsg:
//        TDebug().debug() << localMsg.constData();
        break;
    case QtFatalMsg:
//        TDebug().debug() << localMsg.constData();
        abort();
    }
}
