#include "signalobject.h"
#include <QObject>

class signalObject : QObject {
    Q_OBJECT

    signals:
        void setText(QString text);

};
