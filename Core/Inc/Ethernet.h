/*
 * Ethernet.h
 *
 *  Created on: 2022年4月6日
 *      Author: qzlzdy
 */

#ifndef INC_ETHERNET_H_
#define INC_ETHERNET_H_

#include "ChessEngine.h"
#include "Singleton.h"

#include "lwip.h"

err_t tcpConnected(void *arg, tcp_pcb *tpcb, err_t err);
err_t tcpSent(void *arg, tcp_pcb *tpcb, u16_t len);
err_t tcpRecv(void *arg, tcp_pcb *tpcb, pbuf *p, err_t err);

namespace ehdu{

class Ethernet final:
public ChessEngine, public Singleton<Ethernet>{
public:
	Ethernet(const Ethernet &) = delete;
	Ethernet &operator=(const Ethernet &) = delete;
	Ethernet(Ethernet &&) = default;
	Ethernet &operator=(Ethernet &&) = default;
	~Ethernet();
	void setPosition(const std::string &fen) override;
private:
	enum NetState{
		NET_INIT,
		NET_IDLE,
		NET_SENT,
		NET_RECVED,
	};
	Ethernet();
	NetState state;
	tcp_pcb *tpcb;
	friend class Singleton<Ethernet>;
	friend err_t ::tcpConnected(void *arg, tcp_pcb *tpcb, err_t err);
	friend err_t ::tcpSent(void *arg, tcp_pcb *tpcb, u16_t len);
	friend err_t ::tcpRecv(void *arg, tcp_pcb *tpcb, struct pbuf *p, err_t err);
};

}

#endif /* INC_ETHERNET_H_ */
