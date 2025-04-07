#pragma once
#include <QCoreApplication>

#define QTTR(text) QCoreApplication::translate("Other", text)
#define QTTRD(text, disambiguation) QCoreApplication::translate("Other", text, disambiguation)
#define QTTRN(text, disambiguation, n) QCoreApplication::translate("Other", text, disambiguation, n)
#define QTTRC(context, text) QCoreApplication::translate(context, text)
#define QTTRCD(context, text, disambiguation) QCoreApplication::translate(context, text, disambiguation)
#define QTTRCN(context, text, disambiguation, n) QCoreApplication::translate(context, text, disambiguation, n)
