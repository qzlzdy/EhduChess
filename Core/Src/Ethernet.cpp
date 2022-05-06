/*
 * Ethernet.cpp
 *
 *  Created on: 2022年4月6日
 *      Author: qzlzdy
 */
#include "Ethernet.h"

using namespace std;

err_t tcpConnected(void *arg, tcp_pcb *tpcb, err_t err){
	return err;
}

err_t tcpSent(void *arg, tcp_pcb *tpcb, u16_t len){
	return ERR_OK;
}

err_t tcpRecv(void *arg, tcp_pcb *tpcb, pbuf *p, err_t err){
	string bestmove(reinterpret_cast<const char *>(p->payload), p->len);
	tcp_recved(tpcb, bestmove.length());
	pbuf_free(p);
	ehdu::Ethernet::getInstance()->getController()->setBestmove(bestmove);
	return err;
}

namespace ehdu{

Ethernet::~Ethernet(){
	tcp_close(tpcb);
}

void Ethernet::setPosition(const string &fen){
	tcp_write(tpcb, fen.c_str(), fen.length(), TCP_WRITE_FLAG_COPY);
	tcp_output(tpcb);
}

void Ethernet::updatePieces(const string &fen){
	tcp_write(tpcb, fen.c_str(), fen.length(), TCP_WRITE_FLAG_COPY);
	tcp_output(tpcb);
}

Ethernet::Ethernet(){
	tpcb = tcp_new();
	tcp_sent(tpcb, tcpSent);
	tcp_recv(tpcb, tcpRecv);
	ip_addr_t addr = IPADDR4_INIT_BYTES(169, 254, 82, 59);
	tcp_connect(tpcb, &addr, 2233, tcpConnected);
}

}
