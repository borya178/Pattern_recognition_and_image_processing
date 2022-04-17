#include "affine_plugin.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AffinePlugin w;
    w.show();
    return a.exec();
}
