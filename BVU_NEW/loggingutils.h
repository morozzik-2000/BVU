// LoggingUtils.h
#pragma once
#include <QString>
#include <QDir>
#include <QStandardPaths>
#include <QDateTime>
class LoggingUtils {
public:
    static QString getLogDirectory() {
        static QString logDirPath;

        if (logDirPath.isEmpty()) {
            QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
            QDir logDir(desktopPath + "/MIUS_2_Logs");

            if (!logDir.exists()) {
                logDir.mkpath(".");
            }

            logDirPath = logDir.absolutePath();
        }

        return logDirPath;
    }

    static QString createLogFilePath(const QString& prefix) {
        return getLogDirectory() + "/" + prefix + "_" +
               QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".txt";
    }
};
