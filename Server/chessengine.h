#ifndef CHESSENGINE_H
#define CHESSENGINE_H

#include <QObject>
#include <QProcess>
#include <QStateMachine>

class ChessEngine: public QObject{
    Q_OBJECT
public:
    explicit ChessEngine(QObject *parent = nullptr);
    ~ChessEngine();
public slots:
    void setPosition(const QString &fen);
signals:
    void emitLog(const QString &msg);
    void setBestmove(const QString &bestmove);
    void engineStarted();
    void uciok();
    void readyok();
    void startCalculate();
    void finishCalculate();
private:
    void initStateMachine();
    QProcess engine;
    QStateMachine machine;
private slots:
    void engineRecv();
    void sendUci();
    void configEngine();
    void startNewGame();
};

#endif // CHESSENGINE_H
