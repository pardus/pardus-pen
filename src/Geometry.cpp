#include "DrawingWidget.h"

static QPointF last_end = QPointF(0,0);
static QPointF last_begin = QPointF(0,0);

static int rad = 0;
static int frad = 0;

#define startPoint geo.first(id)
#define endPoint geo.last(id)

void DrawingWidget::drawLineToFunc(qint64 id, qreal pressure) {
    if(startPoint.x() < 0 || startPoint.y() < 0){
        return;
    }
    if (fpressure > 0){
        pressure = fpressure;
    }
    int fpenStyle =  penStyle;
    if (penType == ERASER) {
        fpenStyle = SPLINE;
    }

    switch(fpenStyle){
        case SPLINE:
            painter.begin(&image);
            break;
        case LINE:
        case CIRCLE:
        case RECTANGLE:
        case TRIANGLE:
            image = imageBackup;
            painter.begin(&image);
            break;
    }
    penColor.setAlpha(255);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    switch(penType){
        case PEN:
            break;
        case ERASER:
            painter.setCompositionMode(QPainter::CompositionMode_Clear);
            pressure = 1.0;
            break;
        case MARKER:
            penColor.setAlpha(127);
            break;
    }

    QPen pen = QPen(penColor, penSize[penType]*pressure, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    switch(lineStyle){
        case NORMAL:
            pen.setStyle(Qt::SolidLine);
            break;
        case DOTLINE:
            pen.setStyle(Qt::DotLine);
            break;
        case LINELINE:
            pen.setStyle(Qt::DashLine);
            break;
    }
    if(penType == ERASER) {
        pen.setStyle(Qt::SolidLine);
    }
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    QMap<qint64, QPointF> values = geo.load(id).values;
    QMap<qint64, QPointF>::const_iterator it = values.constBegin();
    QMap<qint64, QPointF>::const_iterator nextIt = it;
    ++nextIt;
    QPainterPath path;

    int size = values.size() % 100;
    it = std::next(it, values.size() - size);
    nextIt = it;
    printf("%d\n", size);


    switch(fpenStyle){
        case SPLINE:
            path.moveTo(it.value());
            while (nextIt != values.constEnd()) {
                path.lineTo(nextIt.value());
                ++it;
                ++nextIt;
            }
            painter.drawPath(path);
            break;
        case LINE:
            painter.drawLine(startPoint, endPoint);
            break;
        case CIRCLE:
            rad = QLineF(startPoint, endPoint).length();
            painter.drawEllipse(startPoint, rad, rad);
            break;
        case RECTANGLE:
            painter.drawRect(QRectF(startPoint,endPoint));
            break;
        case TRIANGLE:
            painter.drawLine(startPoint, endPoint);
            painter.drawLine(startPoint, QPointF(startPoint.x(), endPoint.y()));
            painter.drawLine(QPointF(startPoint.x(), endPoint.y()), endPoint);
            break;
    }
    switch(fpenStyle){
        case SPLINE:
            rad = penSize[penType];
            update(QRectF(
                last_end, endPoint
            ).toRect().normalized().adjusted(-rad, -rad, +rad, +rad));
            break;
        case LINE:
        case TRIANGLE:
        case RECTANGLE:
            rad = penSize[penType];
            update(QRectF(
                last_begin, last_end
            ).toRect().normalized().adjusted(-rad, -rad, +rad, +rad));
            update(QRectF(
                startPoint, endPoint
            ).toRect().normalized().adjusted(-rad, -rad, +rad, +rad));
            break;
        case CIRCLE:
            rad = QLineF(startPoint, endPoint).length() + penSize[penType];
            frad = QLineF(last_begin, last_end).length() + penSize[penType];
            update(QRectF(
                startPoint,startPoint
            ).toRect().normalized().adjusted(-rad, -rad, +rad, +rad));
            update(QRectF(
                startPoint,startPoint
            ).toRect().normalized().adjusted(-frad, -frad, +frad, +frad));
            break;
     }

    last_begin = startPoint;
    last_end = endPoint;

    painter.end();

}
