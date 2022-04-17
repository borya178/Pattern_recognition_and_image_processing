#include "cartoon_plugin.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CartoonPlugin w;
    w.show();
    return a.exec();
}
