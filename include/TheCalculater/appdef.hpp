/**
 * @file appdef.hpp
 * @author prbegd
 * @brief TheCalculater application definition header file.
 * @date 2025-05-25
 * 
 * Copyright © 2025 prbegd & TheCalculater contributors
 * Licensed under the MIT License. See LICENSE in the project root for license information.
 * 
 */
#pragma once
#include <QCoreApplication>

#define QTTR(text) QCoreApplication::translate("Other", text)
#define QTTRD(text, disambiguation) QCoreApplication::translate("Other", text, disambiguation)
#define QTTRN(text, disambiguation, n) QCoreApplication::translate("Other", text, disambiguation, n)
#define QTTRC(context, text) QCoreApplication::translate(context, text)
#define QTTRCD(context, text, disambiguation) QCoreApplication::translate(context, text, disambiguation)
#define QTTRCN(context, text, disambiguation, n) QCoreApplication::translate(context, text, disambiguation, n)

#ifndef THECALCULATER_VERSION
#define THECALCULATER_VERSION "???"
#endif
#ifndef THECALCULATER_COMPILE_TIME
#define THECALCULATER_COMPILE_TIME "???"
#endif
#define THECALCULATER_VERSION_ALL "TheCalculater " THECALCULATER_VERSION ", CompileTime:" THECALCULATER_COMPILE_TIME