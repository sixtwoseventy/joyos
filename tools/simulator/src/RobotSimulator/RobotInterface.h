#ifndef ROBOTINTERFACE_H
#define ROBOTINTERFACE_H
/*
*
*
*/
#include <QObject>

class RobotInterface : public QObject
{
    Q_OBJECT
public:
    explicit RobotInterface(QObject *parent = 0);
    
signals:
    
public slots:
    
};

#endif // ROBOTINTERFACE_H
