#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "chessengine.h"
#include "tcpserver.h"
#include <stack>
#include <QGraphicsScene>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow: public QMainWindow{
    Q_OBJECT
public:
    enum COMPONENT_TYPE{
        ENGINE,
        GUI,
        SERVER,
        SYSTEM
    };
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    void initBackground();
    void initBoard();
    void updatePiece(uint8_t p);
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    std::stack<QGraphicsPixmapItem *> pieces;
    ChessEngine *engine;
    TcpServer *server;
    int index;
private slots:
    void emitSysLog(const QString &msg);
    void emitGuiLog(const QString &msg);
    void emitEngineLog(const QString &msg);
    void emitServerLog(const QString &msg);
    void updateBoard(const QString &fen);
};
#endif // MAINWINDOW_H
