#include "canvas.h"
#include <QPainter>
#include <QCursor>
#include <QFile>
#include <QDataStream>
#include <algorithm>

Canvas::Canvas(QWidget *parent)
    : QWidget(parent), currentShapeType(None), drawing(false),
    drawingConnection(false), connectionStartShape(nullptr),
    connectionEndShape(nullptr), connectionInProgress(false),
    movingShape(false), selectedShape(nullptr), deletingShape(false)
{
    setMouseTracking(true);// отслеживания движения мыши
}

void Canvas::setDrawRectangle()
{
    currentShapeType = Rectangle;
    drawingConnection = false;
    movingShape = false;
    deletingShape = false;
}

void Canvas::setDrawEllipse()
{
    currentShapeType = Ellipse;
    drawingConnection = false;
    movingShape = false;
    deletingShape = false;
}

void Canvas::setDrawTriangle()
{
    currentShapeType = Triangle;
    drawingConnection = false;
    movingShape = false;
    deletingShape = false;
}

void Canvas::setDrawConnection()
{
    currentShapeType = None;
    drawingConnection = true;
    connectionStartShape = nullptr;
    connectionEndShape = nullptr;
    connectionInProgress = false;
    movingShape = false;
    deletingShape = false;
}

void Canvas::setMoveShape()
{
    currentShapeType = None;
    drawingConnection = false;
    movingShape = true;
    selectedShape = nullptr;
    deletingShape = false;
}

void Canvas::setDeleteShape()
{
    currentShapeType = None;
    drawingConnection = false;
    movingShape = false;
    deletingShape = true;
}

void Canvas::resetDrawingMode() {
    currentShapeType = ShapeType::None;
    movingShape = false;
    deletingShape = false;
    drawing = false; // Сбрасываем флаг рисования
    update();
}
void Canvas::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);// помогает избежать warning а неиспользовании

    QPainter painter(this);
    for (const Shape &shape : shapes)
    {
        switch (shape.type)
        {
        case Rectangle:
            painter.drawRect(shape.rect);
            break;
        case Ellipse:
            painter.drawEllipse(shape.rect);
            break;
        case Triangle:
        {
            QPoint points[3] = {
            QPoint((shape.rect.left() + shape.rect.right()) / 2, shape.rect.top()),
            QPoint(shape.rect.left(), shape.rect.bottom()),
             QPoint(shape.rect.right(), shape.rect.bottom())};
            painter.drawPolygon(points, 3);
            break;
        }
        default:
            break;
        }
    }

    for (const Connection &connection : connections)
    {
        painter.drawLine(connection.start, connection.end);
    }

    if (drawing)
    {
        switch (currentShapeType)
        {
        case Rectangle:
            painter.drawRect(currentRect);
            break;
        case Ellipse:
            painter.drawEllipse(currentRect);
            break;
        case Triangle:
        {
            QPolygon triangle;
            QPoint points[3] = {
                                QPoint((currentRect.left() + currentRect.right()) / 2, currentRect.top()),
                                QPoint(currentRect.left(), currentRect.bottom()),
                                QPoint(currentRect.right(), currentRect.bottom())};
            painter.drawPolygon(points, 3);
            break;
        }
        default:
            break;
        }
    }

    if (drawingConnection && connectionInProgress && connectionStartShape)
    {
        painter.drawLine(connectionStartShape->rect.center(), currentRect.bottomRight());
    }
}

void Canvas::mousePressEvent(QMouseEvent *event)
{
    if (deletingShape)
    {
        Shape* shape = shapeAtPoint(event->pos());
        if (shape)
        {
            auto it = std::find_if(shapes.begin(), shapes.end(), [shape](const Shape &s) { return &s == shape; });
            if (it != shapes.end())
            {
                shapes.erase(it);
                connections.erase(std::remove_if(connections.begin(), connections.end(),[shape](const Connection &connection)
                {
                return connection.start == shape->rect.center() || connection.end == shape->rect.center();
                 }), connections.end());
                update();
            }
        }
    }
    else if (drawingConnection)
    {
        if (!connectionInProgress)
        {
            Shape* shape = shapeAtPoint(event->pos());
            if (shape)
            {
                connectionStartShape = shape;
                connectionStartPoint = shape->rect.center();
                connectionInProgress = true;
                update();
            }
        }
        else
        {
            Shape* shape = shapeAtPoint(event->pos());
            if (shape && shape != connectionStartShape)
            {
                connections.append({connectionStartShape->rect.center(), shape->rect.center()});
                connectionInProgress = false;
                update();
            }
            else
            {
                cancelDrawingConnection();
            }
        }
    }
    else if (movingShape)
    {
        selectedShape = shapeAtPoint(event->pos());
        if (selectedShape)
        {
            setCursor(Qt::ClosedHandCursor);
            previousMousePos = event->pos();
        }
    }
    else if (event->button() == Qt::LeftButton && currentShapeType != None)
    {
        drawing = true;
        startPoint = event->pos();
        currentRect = QRect(startPoint, QSize());
    }
}

void Canvas::mouseMoveEvent(QMouseEvent *event)
{
    if (drawing)
    {
        currentRect.setBottomRight(event->pos());
        update();
    }
    else if (drawingConnection && connectionInProgress)
    {
        currentRect.setBottomRight(event->pos());
        update();
    }
    else if (movingShape && selectedShape)
    {
        QPoint delta = event->pos() - previousMousePos;
        previousMousePos = event->pos();

        moveShape(delta);
    }
}


void Canvas::mouseReleaseEvent(QMouseEvent *event)
{
    if (drawing)
    {
        if (event->button() == Qt::LeftButton)
        {
            drawing = false;
            currentRect.setBottomRight(event->pos());
            shapes.append({currentShapeType, currentRect});
            update();
        }
    }
    if (movingShape && selectedShape)
    {
        if (event->button() == Qt::LeftButton)
        {
            setCursor(Qt::ArrowCursor);
            selectedShape = nullptr;
        }
    }
}

void Canvas::keyPressEvent(QKeyEvent *event)
{


    if (event->key() == Qt::Key_Escape)
    {

        if (currentShapeType != ShapeType::None)
        {
            // Если в данный момент рисуется фигура, сбросить рисование
            drawing =false;
            update();
            // Обновить виджет для отмены рисования
        }
    }
    else
    {
        Canvas::keyPressEvent(event);
    }

}

Shape* Canvas::shapeAtPoint(const QPoint &point)
{
    for (Shape &shape : shapes)
    {
        if (shape.rect.contains(point))
        {
            return &shape;
        }
    }
    return nullptr;
}

void Canvas::moveShape(const QPoint &offset)
{
    if (selectedShape)
    {
        selectedShape->rect.translate(offset);

        // Обновление связей
        for (Connection &connection : connections)
        {
            if (connection.start == selectedShape->rect.center() - offset)
            {
                connection.start = selectedShape->rect.center();
            }
            if (connection.end == selectedShape->rect.center() - offset)
            {
                connection.end = selectedShape->rect.center();
            }
        }

        update();
    }
}




void Canvas::cancelDrawingConnection()
{
    connectionInProgress = false;
    connectionStartShape = nullptr;
    update();
}
void Canvas::saveToFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qWarning() << "Cannot open file for writing:" << file.errorString();
        return;
    }

    QTextStream out(&file);

    // Сохранение фигур
    out << shapes.size() << "\n";
    for (const Shape &shape : shapes)
    {
        out << static_cast<int>(shape.type) << " " << shape.rect.x() << " " << shape.rect.y() << " "
            << shape.rect.width() << " " << shape.rect.height() << "\n";
    }

    // Сохранение связей
    out << connections.size() << "\n";
    for (const Connection &connection : connections)
    {
        out << connection.start.x() << " " << connection.start.y() << " "
            << connection.end.x() << " " << connection.end.y() << "\n";
    }

    file.close();
}

void Canvas::loadFromFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Cannot open file for reading:" << file.errorString();
        return;
    }

    QTextStream in(&file);

    // Очистка текущих фигур и связей
    shapes.clear();
    connections.clear();

    // Загрузка фигур
    int shapeCount;
    in >> shapeCount;
    for (int i = 0; i < shapeCount; ++i)
    {
        int type, x, y, width, height;
        in >> type >> x >> y >> width >> height;
        Shape shape;
        shape.type = static_cast<ShapeType>(type);
        shape.rect = QRect(x, y, width, height);
        shapes.append(shape);
    }

    // Загрузка связей
    int connectionCount;
    in >> connectionCount;
    for (int i = 0; i < connectionCount; ++i)
    {
        int startX, startY, endX, endY;
        in >> startX >> startY >> endX >> endY;
        Connection connection;
        connection.start = QPoint(startX, startY);
        connection.end = QPoint(endX, endY);
        connections.append(connection);
    }

    file.close();
    update(); // Обновление холста после загрузки данных
}




