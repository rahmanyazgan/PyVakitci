#include <QApplication>
#include <QStyleFactory>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("PyVakitci");
    app.setOrganizationName("PyVakitci");
    app.setWindowIcon(QIcon(":/resimler/cami.ico"));
    app.setStyle(QStyleFactory::create("Fusion"));

    MainWindow w;
    w.show();

    QStringList args = app.arguments();
    int idx = args.indexOf("--grab-test");
    if (idx >= 0 && idx + 1 < args.size()) {
        w.testEkranGoruntuleriAl(args[idx + 1]);
    }

    return app.exec();
}

