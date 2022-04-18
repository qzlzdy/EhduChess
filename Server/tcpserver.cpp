#include "tcpserver.h"

#include "mainwindow.h"
#include <cstring>
#include <QHostAddress>

using namespace std;

TcpServer::TcpServer(QObject *parent): QObject(parent), server(this){
    client = nullptr;
    connect(&server, &QTcpServer::newConnection, this, &TcpServer::tcpConnected);
    server.setMaxPendingConnections(1);
    server.listen(QHostAddress::Any, 2233);
}

void TcpServer::setBestmove(const QString &bestmove){
    client->write(bestmove.toUtf8().constData());
}

void TcpServer::tcpConnected(){
    client = server.nextPendingConnection();
    emit emitLog(tr("Connection established"));
    connect(client, &QTcpSocket::readyRead, this, &TcpServer::tcpRecv);
}

void TcpServer::tcpRecv(){
    char buffer[80];
    qint64 length = client->readLine(buffer, 80);
    emit emitLog(QString::number(length) + tr(" bytes received"));
    if(memcmp(buffer, "fen", 3) == 0){
        emit setPosition(QString::fromUtf8(buffer, length));
    }
    else{
        emit updateBoard(QString::fromUtf8(buffer, length));
    }
}

void TcpServer::tcpSent(qint64 bytes){
    emit emitLog(QString::number(bytes) + tr(" bytes sent"));
}
