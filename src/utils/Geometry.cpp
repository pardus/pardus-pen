#include "../widgets/DrawingWidget.h"
#include "../tools.h"

#define last_end  geo.last_end[id]
#define last_begin  geo.last_begin[id]

static int rad = 0;

#define startPoint geo.first(id)
#define endPoint geo.last(id)

void DrawingWidget::drawFunc(qint64 id, qreal pressure) {
    int fpenStyle =  penStyle;
    if (penType == ERASER) {
        penStyle = SPLINE;
    }
    if(penStyle == SPLINE) {
        if (fpressure > 0){
            pressure = fpressure;
        }
        if(lineStyle != NORMAL) {
            pressure = 1.0;
        }
        drawLineToFunc(id, pressure);
    } else {
        image.fill(QColor("transparent"));
        for (auto it = geo.values.begin(); it != geo.values.end(); ++it) {
            drawLineToFunc(it.key(), 1.0);
        }
    }
    penStyle = fpenStyle;
}

void DrawingWidget::drawLineToFunc(qint64 id, qreal pressure) {
    if(startPoint.x() < 0 || startPoint.y() < 0){
        return;
    }
    painter.begin(&image);
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

    it = std::next(it, values.size() - values.size() % 100);
    nextIt = it;


    switch(penStyle){
        case SPLINE:
            if(lineStyle == NORMAL) {
                painter.drawLine(startPoint, endPoint);
                break;
            }
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
    switch(penStyle){
        case SPLINE:
            rad = penSize[penType]*2;
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
            rad = QLineF(startPoint, endPoint).length();
            #define frad QLineF(last_begin, last_end).length()
            rad = MAX(rad, frad) + penSize[penType];
            update(QRectF(
                startPoint,startPoint
            ).toRect().normalized().adjusted(-rad, -rad, +rad, +rad));
            break;
     }

    last_begin = startPoint;
    last_end = endPoint;

    painter.end();

}
