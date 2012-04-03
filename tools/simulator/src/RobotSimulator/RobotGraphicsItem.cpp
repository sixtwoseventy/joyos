#include "RobotGraphicsItem.h"
/**
  * Initialize the robot graphics. Maybe should contain some parameters such as wheel spacing, etc
  */
RobotGraphicsItem::RobotGraphicsItem(QGraphicsObject *parent) :
    QGraphicsObject(parent)
{
}

QRectF RobotGraphicsItem::boundingRect(){
    //TODO make this more reasonable..
    return QRectF(0,0, 100,100);
}

/*
 * Draw the robot graphic on the screen
 */
void RobotGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){

}

void RobotGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event){
    QGraphicsObject::mousePressEvent(event);
}

void RobotGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    QGraphicsObject::mouseReleaseEvent(event);
}


void RobotGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    QGraphicsObject::mouseMoveEvent(event);
}
