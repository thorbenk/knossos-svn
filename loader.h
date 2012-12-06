#ifndef LOADER_H
#define LOADER_H

#include <QObject>
#include <QThread>

class Loader : public QObject
{
    Q_OBJECT
public:
    explicit Loader(QObject *parent = 0);
    
signals:
    
public Q_SLOTS:
    void start();

Q_SIGNALS:
    void finished();
    
};

#endif // LOADER_H
