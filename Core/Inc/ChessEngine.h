/*
 * ChessEngine.h
 *
 *  Created on: 2022年4月6日
 *      Author: qzlzdy
 */

#ifndef INC_CHESSENGINE_H_
#define INC_CHESSENGINE_H_

#include <BackwardInterface.h>
#include <string>

namespace ehdu{

class ChessEngine: public BackwardInterface{
public:
	ChessEngine() = default;
	ChessEngine(const ChessEngine &) = default;
	ChessEngine &operator=(const ChessEngine &) = default;
	ChessEngine(ChessEngine &&) = default;
	ChessEngine &operator=(ChessEngine &&) = default;
	virtual ~ChessEngine() = default;
	virtual void setPosition(const std::string &fen) = 0;
	virtual void updatePieces(const std::string &fen) = 0;
};

}

#endif /* INC_CHESSENGINE_H_ */
