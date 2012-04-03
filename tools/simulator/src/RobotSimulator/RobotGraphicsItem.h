#ifndef ROBOTGRAPHICSITEM_H
#define ROBOTGRAPHICSITEM_H

#include <QGraphicsItem>

class RobotGraphicsItem : public QGraphicsObject
{

public:
    RobotGraphicsItem(QGraphicsObject * parent = 0);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect();
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    
signals:
    
public slots:
    
};

#endif // ROBOTGRAPHICSITEM_H
