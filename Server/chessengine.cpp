#include "chessengine.h"

#include "mainwindow.h"
#include <cstring>
#include <QState>

using namespace std;

ChessEngine::ChessEngine(QObject *parent):
QObject(parent), engine(this), machine(this){
    initStateMachine();
    connect(&engine, &QProcess::readyRead,
            this, &ChessEngine::engineRecv);
    QString stockfish =
        "D:/src/Qt/EhduChessServer/stockfish/stockfish_14.1_win_x64_avx2.exe";
    engine.start(stockfish);
    engine.waitForStarted();
}

ChessEngine::~ChessEngine(){
    engine.write("quit\n");
    engine.waitForFinished();
}

void ChessEngine::setPosition(const QString &fen){
    engine.write(("position " + fen + "\n").toUtf8().constData());
    emit emitLog(tr("Start calculating"));
    emit startCalculate();
    engine.write("go depth 20\n");
}

void ChessEngine::initStateMachine(){
    QState *init = new QState;
    QState *started = new QState;
    QState *acknowledged = new QState;
    QState *configurated = new QState;
    QState *idle = new QState;
    QState *computing = new QState;
    init->addTransition(this, &ChessEngine::engineStarted, started);
    connect(started, &QState::entered, this, &ChessEngine::sendUci);
    started->addTransition(this, &ChessEngine::uciok, acknowledged);
    connect(acknowledged, &QState::entered, this, &ChessEngine::configEngine);
    acknowledged->addTransition(this, &ChessEngine::readyok, configurated);
    connect(configurated, &QState::entered, this, &ChessEngine::startNewGame);
    configurated->addTransition(idle);
    idle->addTransition(this, &ChessEngine::startCalculate, computing);
    computing->addTransition(this, &ChessEngine::finishCalculate, idle);
    machine.addState(init);
    machine.addState(started);
    machine.addState(acknowledged);
    machine.addState(configurated);
    machine.addState(idle);
    machine.addState(computing);
    machine.setInitialState(init);
    machine.start();
}

#include <iostream>
using namespace std;

void ChessEngine::engineRecv(){
    char buffer[256];
    qint64 length = engine.readLine(buffer, 256);
    while(length != 0){
        if(length > 9 && memcmp(buffer, "Stockfish", 9) == 0){
            emit engineStarted();
        }
        else if(length == 5 && memcmp(buffer, "uciok", 5) == 0){
            emit uciok();
        }
        else if(length == 7 && memcmp(buffer, "readyok", 7) == 0){
            emit readyok();
        }
        else if(length > 8 && memcmp(buffer, "bestmove", 8) == 0){
            emit finishCalculate();
            emit emitLog(QString::fromUtf8(buffer, length - 2));
            emit setBestmove(QString::fromUtf8(buffer, length - 2));
        }
        length = engine.readLine(buffer, 256);
    }
}

void ChessEngine::sendUci(){
    engine.write("uci\n");
}

void ChessEngine::configEngine(){
    engine.write("setoption name Threads value 4\n");
    engine.write("setopton name UCI_AnalyseMode value true\n");
    engine.write("isready\n");
}

void ChessEngine::startNewGame(){
    engine.write("ucinewgame\n");
    engine.waitForBytesWritten();
}
