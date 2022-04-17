#ifndef MEDIANFILTERPLUGIN_H
#define MEDIANFILTERPLUGIN_H

#include <QObject>
#include <QtPlugin>

#include "editor_plugin_interface.h"

class MedianFilterPlugin: public QObject, public EditorPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID EDIT_PLUGIN_INTERFACE_IID);
    Q_INTERFACES(EditorPluginInterface);
public:
    QString name();
    void edit(const cv::Mat &input, cv::Mat &output);
};
#endif // MEDIANFILTERPLUGIN_H
