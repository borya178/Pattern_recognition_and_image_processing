#include "median_filter_plugin.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MedianFilterPlugin w;
    w.show();
    return a.exec();
}
