#include "rotate_plugin.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RotatePlugin w;
    w.show();
    return a.exec();
}
