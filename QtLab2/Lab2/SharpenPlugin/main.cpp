#include "sharpen_plugin.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SharpenPlugin w;
    w.show();
    return a.exec();
}
