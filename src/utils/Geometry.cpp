#include <widgets/DrawingWidget.h>
#include <constants.h>

#include <math.h>

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

void DrawingWidget::drawArrow(QPainter& painter, QPointF start, QPointF end) {

  painter.setRenderHint(QPainter::Antialiasing, true);

  qreal arrowSize = penSize[penType] * 2;

  QLineF line(end, start);

  double angle = std::atan2(-line.dy(), line.dx());
  QPointF arrowP1 = line.p1() + QPointF(sin(angle + M_PI / 3) * arrowSize,
                                        cos(angle + M_PI / 3) * arrowSize);
  QPointF arrowP2 = line.p1() + QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize,
                                        cos(angle + M_PI - M_PI / 3) * arrowSize);

  painter.drawLine(line.p1(), arrowP1);
  painter.drawLine(line.p1(), arrowP2);

}

void DrawingWidget::drawLineToFunc(qint64 id, qreal pressure) {
    //printf("%d %d %d %d\n", penType, penStyle, lineStyle, penSize[penType]);
    if(startPoint.x() < 0 || startPoint.y() < 0){
        return;
    }
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    if(penType == ERASER){
        painter.setCompositionMode(QPainter::CompositionMode_Clear);
        pressure = 1.0;
    }

    pen.setWidth(penSize[penType]*pressure);

    if (lineStyle == FILLED && (penStyle == TRIANGLE || penStyle == CIRCLE || penStyle == RECTANGLE)){
        painter.setBrush(QBrush(pen.color()));
        pen.setWidth(0);
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
        case VECTOR:
            painter.drawLine(startPoint, endPoint);
            drawArrow(painter, startPoint, endPoint);
            break;
        case VECTOR2:
            painter.drawLine(startPoint, endPoint);
            drawArrow(painter, startPoint, endPoint);
            drawArrow(painter, endPoint, startPoint);
            break;
        case CIRCLE:
            rad = QLineF(startPoint, endPoint).length();
            painter.drawEllipse(startPoint, rad, rad);
            break;
        case RECTANGLE:
            painter.drawRect(QRectF(startPoint,endPoint));
            break;
        case TRIANGLE:
            path.moveTo(startPoint);
            path.lineTo(endPoint);
            path.lineTo(QPointF(startPoint.x(), endPoint.y()));
            path.lineTo(startPoint);
            painter.drawPath(path);
            break;
    }
    switch(penStyle){
        case SPLINE:
            rad = penSize[penType]*2;
            update(QRectF(
                last_end, endPoint
            ).toRect().normalized().adjusted(-rad, -rad, +rad, +rad));
            break;
        case VECTOR:
        case VECTOR2:
            rad = penSize[penType] *2;
            update(QRectF(
                startPoint, endPoint
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

}

void DrawingWidget::drawRecognizedShape(
    int decision,
    const StrokeVariables &variables,
    const StrokeResult &result){
    painter.begin(&image);

    painter.setPen(pen);

    const bool isClosedShape =
        decision == RECOG_CIRCLE ||
        decision == RECOG_TRIANGLE ||
        decision == RECOG_SQUARE;

    if (lineStyle == FILLED && isClosedShape){
        painter.setBrush(pen.color());
    }
    else {
        painter.setBrush(Qt::NoBrush);
    }

    switch (decision) {
    case RECOG_LINE:
        painter.drawLine(
            variables.points[0],
            variables.points[variables.pointCount - 1]);
        break;

    case RECOG_CIRCLE:
        painter.drawEllipse(
            result.circleCenter,
            result.circleRadius,
            result.circleRadius);
        break;

    case RECOG_TRIANGLE:
    {
        QPolygonF triangle;

        for (int i = 0; i < 3; i++) {
            triangle << result.idealCorners[i];
        }

        painter.drawPolygon(triangle);
        break;
    }

    case RECOG_SQUARE:
    {
        QPolygonF rectangle;

        for (int i = 0; i < 4; i++) {
            rectangle << result.idealCorners[i];
        }

        painter.drawPolygon(rectangle);
        break;
    }

    default:
        break;
    }

    painter.end();
    update();
}

int DrawingWidget::performStrokeRecognition(qint64 id){
   QMap<qint64, QPointF> values = geo.load(id).values;
   printf("SIZE: %lld", values.size());
   int decision = stroke_recognition(
            values,
            recognitionVariables,
            recognitionResult);

    printf("Recognition decision: %d\n", decision);
    return decision;
}

void DrawingWidget::applyRecognitionResult(int decision , QImage &backgroundImage){
    // Merge the user's freehand drawing with the previous canvas.
    QImage freehandImage = backgroundImage.copy();
    QPainter freehandPainter(&freehandImage);
    freehandPainter.drawImage(QPointF(0, 0), image.toImage());
    freehandPainter.end();
    // save the freehandImage to history.
    addImage(freehandImage);
    image.fill(QColor("transparent"));
    drawRecognizedShape(
    decision,
    recognitionVariables,
    recognitionResult);
}