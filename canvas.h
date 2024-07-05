#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QVector>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFile>
#include <QDataStream>
#include <QString>

enum ShapeType { None, Rectangle, Ellipse, Triangle };

struct Shape {
    ShapeType type;
    QRect rect;
};

struct Connection {
    QPoint start;
    QPoint end;
};

class Canvas : public QWidget
{
    Q_OBJECT

public:

    explicit Canvas(QWidget *parent = nullptr);// предотвращает преобразование  указателя QWidget* в объект Canvas
    void saveToFile(const QString &fileName);
    void loadFromFile(const QString &fileName);

public slots:
    void setDrawRectangle();
    void setDrawEllipse();
    void setDrawTriangle();
    void setDrawConnection();
    void setMoveShape();
    void setDeleteShape();
    void resetDrawingMode();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    ShapeType currentShapeType;
    bool drawing;
    QPoint startPoint;
    QRect currentRect;
    QVector<Shape> shapes;

    bool drawingConnection;
    QPoint connectionStartPoint;
    Shape* connectionStartShape;
    Shape* connectionEndShape; // Новое поле для хранения конечной фигуры связи
    bool connectionInProgress; // Флаг для отслеживания состояния рисования связи
    QVector<Connection> connections;

    bool movingShape;
    Shape* selectedShape;
    QPoint previousMousePos;

    bool deletingShape;

    Shape* shapeAtPoint(const QPoint &point);
    void moveShape(const QPoint &offset);
    void cancelDrawingConnection(); // Метод для отмены рисования связи
};

#endif // CANVAS_H
