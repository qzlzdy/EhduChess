#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QGraphicsPixmapItem>
#include <QHostAddress>
#include <QImage>
#include <QPixmap>
#include <QTransform>

MainWindow::MainWindow(QWidget *parent):
QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    emitSysLog(tr("System started"));
    scene = new QGraphicsScene(this);
    initBoard();
    engine = new ChessEngine(this);
    connect(engine, &ChessEngine::emitLog, this, &MainWindow::emitEngineLog);
    server = new TcpServer(this);
    connect(server, &TcpServer::emitLog, this, &MainWindow::emitServerLog);
    connect(server, &TcpServer::setPosition, engine, &ChessEngine::setPosition);
    connect(server, &TcpServer::updateBoard, this, &MainWindow::updateBoard);
    connect(engine, &ChessEngine::setBestmove, server, &TcpServer::setBestmove);
    index = 0;
}

MainWindow::~MainWindow(){
    delete server;
    delete engine;
    delete ui;
}

void MainWindow::emitSysLog(const QString &msg){
    ui->textEdit->append(tr("[sys] ") + msg);
}

void MainWindow::emitGuiLog(const QString &msg){
    ui->textEdit->append(tr("[gui] ") + msg);
}

void MainWindow::emitEngineLog(const QString &msg){
    ui->textEdit->append(tr("[engine] ") + msg);
}

void MainWindow::emitServerLog(const QString &msg){
    ui->textEdit->append(tr("[server] ") + msg);
}

void MainWindow::initBackground(){
    QImage eimg(":/backgrounds/even.png");
    QImage oimg(":/backgrounds/odd.png");
    QPixmap epix = QPixmap::fromImage(eimg.scaled(75, 75));
    QPixmap opix = QPixmap::fromImage(oimg.scaled(75, 75));
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            QPixmap &pix = (i + j) % 2 == 0 ? opix : epix;
            scene->addPixmap(pix)->setOffset(75 * i, 75 * j);
        }
    }
}

void MainWindow::initBoard(){
    initBackground();
    ui->graphicsView->setScene(scene);
    ui->graphicsView->show();
    emitGuiLog(tr("Board initialized"));
    updateBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
}

void MainWindow::updatePiece(uint8_t p){
    QString filename = ":/pieces/";
    switch(p){
    case ' ':
        ++index;
        return;
    case 'r':
        filename += "black/rook";
        break;
    case 'n':
        filename += "black/knight";
        break;
    case 'b':
        filename += "black/bishop";
        break;
    case 'q':
        filename += "black/queen";
        break;
    case 'k':
        filename += "black/king";
        break;
    case 'p':
        filename += "black/pawn";
        break;
    case 'R':
        filename += "white/rook";
        break;
    case 'N':
        filename += "white/knight";
        break;
    case 'B':
        filename += "white/bishop";
        break;
    case 'Q':
        filename += "white/queen";
        break;
    case 'K':
        filename += "white/king";
        break;
    case 'P':
        filename += "white/pawn";
        break;
    default:
        emitSysLog(tr("Unknown piece type: ") + QString::number(p));
        break;
    }
    filename += ".png";
    QImage img(filename);
    QPixmap pix = QPixmap::fromImage(img.scaled(75, 75));
    int x = index % 8;
    int y = index / 8;
    pieces.push(scene->addPixmap(pix));
    pieces.top()->setOffset(75 * x, 75 * y);
    ++index;
}

void MainWindow::updateBoard(const QString &fen){
    while(!pieces.empty()){
        scene->removeItem(pieces.top());
        pieces.pop();
    }
    int i = 0;
    index = 0;
    while(i < fen.length()){
        if(fen[i].isDigit()){
            int count = fen[i].toLatin1() - '0';
            for(int j = 0; j < count; ++j){
                updatePiece(' ');
            }
        }
        else if(fen[i].isLetter()){
            updatePiece(fen[i].toLatin1());
        }
        ++i;
    }
    emitGuiLog(tr("Board updated"));
}
