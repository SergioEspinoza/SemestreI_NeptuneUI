
#include <QtQml/qqmlextensionplugin.h>
#include <qqml.h>
#include "cancontroller.h"

static QObject *canControllerSingletonFactory(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(scriptEngine)
    Q_UNUSED(engine)

    return new CanController();
}

class CanControllerPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface/1.0")
public:
    virtual void registerTypes(const char *uri)
    {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("com.yazaki.CanController"));
        Q_UNUSED(uri);

        qmlRegisterSingletonType<CanController>(uri, 1, 0, "CanController", canControllerSingletonFactory);
    }
};

#include "plugin.moc"
