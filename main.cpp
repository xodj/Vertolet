#include "vertolet.h"

#include <QApplication>
#include <QLoggingCategory>

int main(int argc, char *argv[])
{
    QLoggingCategory::setFilterRules(QStringLiteral("qt.modbus* = true"));
    QApplication a(argc, argv);
    Vertolet w;
    w.show();
    return a.exec();
}
