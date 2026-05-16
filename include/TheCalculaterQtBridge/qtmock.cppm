/**
 * @file qtmock.cppm
 * @author prbegd
 * @date 2026-04-12
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
module;
#include <QApplication>
#include <QByteArray>
#include <QDateTime>
#include <QDesktopServices>
#include <QDialog>
#include <QIcon>
#include <QLocale>
#include <QMessageBox>
#include <QObject>
#include <QProcess>
#include <QResource>
#include <QString>
#include <QStringList>
#include <QSysInfo>
#include <QUrl>
#include <QUuid>
#include <QWidget>
#include <QtLogging>


export module TheCalculaterQtBridge.qtmock;

export {
    using ::QApplication;
    using ::QByteArray;
    using ::QCoreApplication;
    using ::QDateTime;
    using ::QDesktopServices;
    using ::QDialog;
    using ::QGuiApplication;
    using ::QIcon;
    using ::QLocale;
    using ::QMessageBox;
    using ::QObject;
    using ::QProcess;
    using ::QResource;
    using ::QString;
    using ::QStringList;
    using ::QSysInfo;
    using ::QUrl;
    using ::QUuid;
    using ::QWidget;

    using ::QtMsgType;
}
export namespace Qt {
    using ::Qt::DateFormat;
}