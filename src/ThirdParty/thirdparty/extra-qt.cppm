/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
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

export module thirdparty.extra:qt;

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