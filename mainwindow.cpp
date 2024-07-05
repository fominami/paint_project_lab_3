#include "mainwindow.h"
#include <QToolBar>
#include <QAction>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QIcon>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), canvas(new Canvas(this))
{

    QToolBar *toolbar = addToolBar("Shapes");

    QPushButton *rectangleButton = new QPushButton();
    QIcon iconr("rectangle.png");
    rectangleButton->setIcon(iconr);
    rectangleButton->setIconSize(iconr.actualSize(QSize(32, 32)));
    QPushButton *ellipseButton = new QPushButton();
    QIcon icone("ellipse.png");
    ellipseButton->setIcon(icone);
    ellipseButton->setIconSize(icone.actualSize(QSize(32, 32)));
    QPushButton *triangleButton = new QPushButton();
    QIcon icont("triangle.png");
    triangleButton->setIcon(icont);
    triangleButton->setIconSize(icont.actualSize(QSize(32, 32)));
    QPushButton *connectionButton = new QPushButton();
    QIcon iconc("connect.png");
    connectionButton->setIcon(iconc);
    connectionButton->setIconSize(iconc.actualSize(QSize(32, 32)));
    QPushButton *moveButton= new QPushButton();
    QIcon iconm("move.png");
    moveButton->setIcon(iconm);
    moveButton->setIconSize(iconm.actualSize(QSize(32, 32)));
   QPushButton *deleteButton = new QPushButton();
    QIcon icond("delete.png");
    deleteButton->setIcon(icond);
    deleteButton->setIconSize(icond.actualSize(QSize(32, 32)));
    QPushButton *saveButton = new QPushButton();
    QIcon icons("save.png");
    saveButton->setIcon(icons);
   saveButton->setIconSize(icons.actualSize(QSize(32, 32)));
    QPushButton *loadButton = new QPushButton();
   QIcon iconl("load.png");
   loadButton->setIcon(iconl);
   loadButton->setIconSize(iconl.actualSize(QSize(32, 32)));


    toolbar->addWidget(rectangleButton);
    toolbar->addWidget(ellipseButton);
    toolbar->addWidget(triangleButton);
    toolbar->addWidget(connectionButton);
    toolbar->addWidget(moveButton);
    toolbar->addWidget(deleteButton);
    toolbar->addWidget(saveButton);
    toolbar->addWidget(loadButton);


    connect(rectangleButton, &QPushButton::clicked, canvas, &Canvas::setDrawRectangle);
    connect(ellipseButton, &QPushButton::clicked, canvas, &Canvas::setDrawEllipse);
    connect(triangleButton, &QPushButton::clicked, canvas, &Canvas::setDrawTriangle);
    connect(connectionButton, &QPushButton::clicked, canvas, &Canvas::setDrawConnection);
    connect(moveButton, &QPushButton::clicked, canvas, &Canvas::setMoveShape);
    connect(deleteButton, &QPushButton::clicked, canvas, &Canvas::setDeleteShape);
    QObject::connect(saveButton, &QPushButton::clicked, [&]() {
        QString fileName=QFileDialog::getSaveFileName(this, "Save File", "", "Text Files (*.txt)");
        if (!fileName.isEmpty())
        {
            canvas->saveToFile(fileName);
        }
    });

    QObject::connect(loadButton, &QPushButton::clicked, [&]() {
        QString fileName= QFileDialog::getOpenFileName(this, "Load File", "", "Text Files (*.txt)");
        if (!fileName.isEmpty())
        {
            canvas->loadFromFile(fileName);
        }
    });
    canvas->setFocusPolicy(Qt::StrongFocus);
    canvas->setFocus();
    setCentralWidget(canvas);
}

MainWindow::~MainWindow()
{
}
