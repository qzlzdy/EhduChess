#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class TcpServer: public QObject{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = nullptr);
signals:
    void emitLog(const QString &msg);
    void setPosition(const QString &fen);
    void updateBoard(const QString &fen);
public slots:
    void setBestmove(const QString &bestmove);
private:
    QTcpServer server;
    QTcpSocket *client;
private slots:
    void tcpConnected();
    void tcpRecv();
    void tcpSent(qint64 bytes);
};

#endif // TCPSERVER_H
