#include "erode_plugin.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ErodePlugin w;
    w.show();
    return a.exec();
}
