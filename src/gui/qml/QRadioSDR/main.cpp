#include "CustomPlotItem.h"
#include <QApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    // Register customplotitem
    qmlRegisterType<CustomPlotItem>("CustomPlot", 1, 0, "CustomPlotItem");
    qmlRegisterType<CustomAxis>("CustomPlot", 1, 0, "CustomAxis");
    // load qml
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));
    // execute
    return app.exec();
}
