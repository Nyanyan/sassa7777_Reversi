//
//  othello.c
//  othello
//
//  Created by sasa on 2023/09/30.
//

#include "reversi.h"
#include "Objective-C-Wrapper.h"

void reset(void) {
	printf("[*]初期化中...\n");
	printf("DEPTH=%d\n", DEPTH);
	printf("Player: %d\n", botplayer);
	//printf("CPU Core count: %d\n", cpu_core);
	nowTurn = BLACK_TURN;
	nowIndex = 1;
	DEPTH = firstDEPTH;
	afterIndex = nowIndex+DEPTH;
	playerboard = 0x0000000810000000ULL;
	oppenentboard = 0x0000001008000000ULL;
	return;
}

int putstone(char y, char x) {
	tmpy = y;
	tmpx = x;
	uint64_t put = cordinate_to_bit(&x, &y);
	legalboard = makelegalBoard(&playerboard, &oppenentboard);
	if(canput(&put, &legalboard)) {
		reversebit(put);
		nowIndex++;
		afterIndex++;
		return 1;
	} else {
		return 0;
	}
}

uint64_t cordinate_to_bit(char *x, char *y) {
	return 0x8000000000000000ULL >> ((*y<<3)+*x);
}

bool canput(uint64_t *put, uint64_t *legalboard) {
	return ((*put & *legalboard) == *put);
}

uint64_t makelegalBoard(uint64_t *playerboard, uint64_t *oppenentboard) {
	uint64_t horizontalboard = (*oppenentboard & 0x7e7e7e7e7e7e7e7e);
	uint64_t verticalboard = (*oppenentboard & 0x00FFFFFFFFFFFF00);
	uint64_t allsideboard = (*oppenentboard & 0x007e7e7e7e7e7e00);
	uint64_t blankboard = ~(*playerboard | *oppenentboard);
	
	uint64_t tmp;
	uint64_t legalboard;
	
	//左
	tmp = horizontalboard & (*playerboard << 1);
	tmp |= horizontalboard & (tmp << 1);
	tmp |= horizontalboard & (tmp << 1);
	tmp |= horizontalboard & (tmp << 1);
	tmp |= horizontalboard & (tmp << 1);
	tmp |= horizontalboard & (tmp << 1);
	legalboard = blankboard & (tmp << 1);
	
	//右
	tmp = horizontalboard & (*playerboard >> 1);
	tmp |= horizontalboard & (tmp >> 1);
	tmp |= horizontalboard & (tmp >> 1);
	tmp |= horizontalboard & (tmp >> 1);
	tmp |= horizontalboard & (tmp >> 1);
	tmp |= horizontalboard & (tmp >> 1);
	legalboard |= blankboard & (tmp >> 1);

	//上
	tmp = verticalboard & (*playerboard << 8);
	tmp |= verticalboard & (tmp << 8);
	tmp |= verticalboard & (tmp << 8);
	tmp |= verticalboard & (tmp << 8);
	tmp |= verticalboard & (tmp << 8);
	tmp |= verticalboard & (tmp << 8);
	legalboard |= blankboard & (tmp << 8);
	
	//下
	tmp = verticalboard & (*playerboard >> 8);
	tmp |= verticalboard & (tmp >> 8);
	tmp |= verticalboard & (tmp >> 8);
	tmp |= verticalboard & (tmp >> 8);
	tmp |= verticalboard & (tmp >> 8);
	tmp |= verticalboard & (tmp >> 8);
	legalboard |= blankboard & (tmp >> 8);
	
	//右斜め上
	tmp = allsideboard & (*playerboard << 7);
	tmp |= allsideboard & (tmp << 7);
	tmp |= allsideboard & (tmp << 7);
	tmp |= allsideboard & (tmp << 7);
	tmp |= allsideboard & (tmp << 7);
	tmp |= allsideboard & (tmp << 7);
	legalboard |= blankboard & (tmp << 7);
	
	//左斜め上
	tmp = allsideboard & (*playerboard << 9);
	tmp |= allsideboard & (tmp << 9);
	tmp |= allsideboard & (tmp << 9);
	tmp |= allsideboard & (tmp << 9);
	tmp |= allsideboard & (tmp << 9);
	tmp |= allsideboard & (tmp << 9);
	legalboard |= blankboard & (tmp << 9);
	
	//右斜め下
	tmp = allsideboard & (*playerboard >> 9);
	tmp |= allsideboard & (tmp >> 9);
	tmp |= allsideboard & (tmp >> 9);
	tmp |= allsideboard & (tmp >> 9);
	tmp |= allsideboard & (tmp >> 9);
	tmp |= allsideboard & (tmp >> 9);
	legalboard |= blankboard & (tmp >> 9);
	
	//左斜め下
	tmp = allsideboard & (*playerboard >> 7);
	tmp |= allsideboard & (tmp >> 7);
	tmp |= allsideboard & (tmp >> 7);
	tmp |= allsideboard & (tmp >> 7);
	tmp |= allsideboard & (tmp >> 7);
	tmp |= allsideboard & (tmp >> 7);
	legalboard |= blankboard & (tmp >> 7);
	
	return legalboard;
}

void reversebit(uint64_t put) {
	uint64_t rev = 0;
	for (char i = 0; i<8; ++i) {
		uint64_t rev_ = 0;
		uint64_t mask = transfer(&put, &i);
		while ((mask != 0) && ((mask & oppenentboard) != 0)) {
			rev_ |= mask;
			mask = transfer(&mask, &i);
		}
		if((mask & playerboard) != 0) rev |= rev_;
	}
	//反転
	playerboard ^= (put | rev);
	oppenentboard ^= rev;
}


uint64_t revbit(uint64_t *put, uint64_t *playerboard, uint64_t *oppenentboard) {
	rev = 0;
	for (char i = 0; i<8; ++i) {
		uint64_t rev_ = 0;
		uint64_t mask = transfer(put, &i);
		while ((mask != 0) && ((mask & *oppenentboard) != 0)) {
			rev_ |= mask;
			mask = transfer(&mask, &i);
		}
		if((mask & *playerboard) != 0) rev |= rev_;
	}
	return rev;
}

uint64_t transfer(uint64_t *put, char *i) {
	switch (*i) {
		case 0:
			return (*put << 8) & 0xffffffffffffff00;
			break;
		case 1:
			return (*put << 7) & 0x7f7f7f7f7f7f7f00;
			break;
		case 2:
			return (*put >> 1) & 0x7f7f7f7f7f7f7f7f;
			break;
		case 3:
			return (*put >> 9) & 0x007f7f7f7f7f7f7f;
			break;
		case 4:
			return (*put >> 8) & 0x00ffffffffffffff;
			break;
		case 5:
			return (*put >> 7) & 0x00fefefefefefefe;
			break;
		case 6:
			return (*put << 1) & 0xfefefefefefefefe;
			break;
		case 7:
			return (*put << 9) & 0xfefefefefefefe00;
			break;
		default:
			printf("error\n");
			return 0;
			break;
	}
}

bool isPass(void) {
	return (makelegalBoard(&playerboard, &oppenentboard) == 0 && makelegalBoard(&oppenentboard, &playerboard) != 0);
}

bool isFinished(void) {
	return (makelegalBoard(&playerboard, &oppenentboard) == 0 && makelegalBoard(&oppenentboard, &playerboard) == 0);
}

void swapboard(void) {
	uint64_t tmp = playerboard;
	playerboard = oppenentboard;
	oppenentboard = tmp;
	nowTurn = -nowTurn;
}

int bitcount(uint64_t bits) {
	return __builtin_popcountll(bits);
}

void moveordering(uint64_t moveorder[64], int moveorder_score[64], uint64_t *playerboard, uint64_t *oppenentboard) {
	uint64_t legalboard = makelegalBoard(playerboard, oppenentboard);
	int putable_count = (int)__builtin_popcountll(legalboard);
	char j=0;
	uint64_t mask = 0x8000000000000000ULL;
	for (char i=0; i<64; ++i) {
		if(canput(&mask, &legalboard)) {
			moveorder[j] = mask;
			++j;
		} else {
			moveorder[i] = 0;
		}
		mask >>= 1;
	}
	for (char i=0; i<putable_count; ++i) {
		moveorder_score[i] = nega_alpha_move_order(DEPTH, -32767, 32767, playerboard, oppenentboard, &moveorder[i]);
	}
	int temp = 0;
	uint64_t temp2 = 0;
	for (char i = 0; i < putable_count-1; i++) {
		for (char j = 0; j < putable_count-1; j++) {
			if (moveorder_score[j+1] < moveorder_score[j]) {
				temp = moveorder_score[j];
				moveorder_score[j] = moveorder_score[j+1];
				moveorder_score[j+1] = temp;
				temp2 = moveorder[j];
				moveorder[j] = moveorder[j+1];
				moveorder[j+1] = temp2;
			}
		}
	}
}

int ai(void) {
	if (nowTurn == -botplayer) {
		return 0;
	}
	printf("[*]Botが考え中..\n");
	if(DEPTH >= 10 && nowIndex >= 44) DEPTH = 20;
	tmpx = -1;
	tmpy = -1;
	tmpbit = 0;
	think_percent = 0;
	update_hakostring();
	legalboard = makelegalBoard(&playerboard, &oppenentboard);
	int putable_count = (int)__builtin_popcountll(legalboard);
	think_count = 100/putable_count;
	nega_alpha(DEPTH, -32767, 32767, &playerboard, &oppenentboard);
	if(tmpx == -1 || tmpy == -1) error_hakostring();
	printf("(%d, %d)\n", tmpx, tmpy);
	think_percent = 100;
	update_hakostring();
	putstone(tmpy, tmpx);
	return 1;
}

int nega_alpha(char depth, int alpha, int beta, uint64_t *playerboard, uint64_t *oppenentboard) {
	if(!(depth)) return countscore(playerboard, oppenentboard, &afterIndex);
	
	uint64_t legalboard = makelegalBoard(playerboard, oppenentboard);
	if(!(legalboard)) {
		if(!(makelegalBoard(oppenentboard, playerboard))) return countscore(playerboard, oppenentboard, &afterIndex);
		else return -nega_alpha(depth-1, -beta, -alpha, oppenentboard, playerboard);
	}
	uint64_t rev = 0;
	int var, max_score = -32767;
	for (char i = 0; i<64; ++i) {
		if(canput(&moveorder_bit[i], &legalboard)) {
			rev = revbit(&moveorder_bit[i], playerboard, oppenentboard);
			*playerboard ^= (moveorder_bit[i] | rev);
			*oppenentboard ^= rev;
			var = -nega_alpha(depth-1, -beta, -alpha, oppenentboard, playerboard);
			*playerboard ^= (moveorder_bit[i] | rev);
			*oppenentboard ^= rev;
			if(depth == DEPTH) {
				think_percent += think_count;
				update_hakostring();
			}
			if (var >= beta) {
				return var;
			}
			if(var > alpha) {
				alpha = var;
				if(depth == DEPTH) {
					tmpx = moveorder[i][1];
					tmpy = moveorder[i][0];
				}
			}
			if(alpha > max_score) max_score = alpha;
		}
	}
	return max_score;
}

int nega_alpha_move_order(char depth, int alpha, int beta, uint64_t *playerboard, uint64_t *oppenentboard, uint64_t *put) {
	if(depth == 0) return countscore(playerboard, oppenentboard, &afterIndex);
	uint64_t rev = 0;
	int var;
	rev = revbit(put, playerboard, oppenentboard);
	*playerboard ^= (*put | rev);
	*oppenentboard ^= rev;
	var = countscore(playerboard, oppenentboard, &afterIndex);
	*playerboard ^= (*put | rev);
	*oppenentboard ^= rev;
	return var;
}

int winner(void) {
	if(nowTurn == BLACK_TURN) {
		blackc = bitcount(playerboard);
		whitec = bitcount(oppenentboard);
	} else {
		whitec = bitcount(playerboard);
		blackc = bitcount(oppenentboard);
	}
	if (blackc > whitec) {
		return 1;
	} else if (blackc < whitec) {
		return 2;
	} else {
		return 0;
	}
}

//evaluation

int score_stone(uint64_t *playerboard, uint64_t *oppenentboard) {
	int score = 0;
	uint64_t mask = 0x8000000000000000ULL;
	for (char i = 0; i<64; ++i) {
		if(*playerboard & mask) {
			score += scoreboard[i];
		} else if(*oppenentboard & mask) {
			score -= scoreboard[i];
		}
		mask >>= 1;
	}
	
	//左
	switch (*playerboard & LEFT_BOARD) {
		case 0x0000808080800000ULL:
			score += 2;
			break;
		case 0x0000800000800000ULL:
			if((*oppenentboard & LEFT_BOARD) == 0x0000008080000000ULL) score -= 4;
			break;
		case 0x0000800080800000ULL:
			score -= 2;
			break;
		case 0x0000808000800000ULL:
			score -= 2;
			break;
		case 0x0080808080808000ULL:
			score += 25;
			break;
		default:
			break;
	}
	switch (*oppenentboard & LEFT_BOARD) {
		case 0x0000808080800000ULL:
			score -= 2;
			break;
		case 0x0000800000800000ULL:
			if((*playerboard & LEFT_BOARD) == 0x0000008080000000ULL) score += 4;
			break;
		case 0x0000800080800000ULL:
			score += 2;
			break;
		case 0x0000808000800000ULL:
			score += 2;
			break;
		case 0x0080808080808000ULL:
			score -= 25;
			break;
		default:
			break;
	}
	//右
	switch (*playerboard & RIGHT_BOARD) {
		case 0x0000010101010000ULL:
			score += 2;
			break;
		case 0x0000010000010000ULL:
			if((*oppenentboard & RIGHT_BOARD) == 0x0000000101000000ULL) score -= 4;
			break;
		case 0x0000010001010000ULL:
			score -= 2;
			break;
		case 0x0000010100010000ULL:
			score -= 2;
			break;
		case 0x0001010101010100ULL:
			score += 25;
			break;
		default:
			break;
	}
	switch (*oppenentboard & RIGHT_BOARD) {
		case 0x0000010101010000ULL:
			score -= 2;
			break;
		case 0x0000010000010000ULL:
			if((*playerboard & RIGHT_BOARD) == 0x0000000101000000ULL) score += 4;
			break;
		case 0x0000010001010000ULL:
			score += 2;
			break;
		case 0x0000010100010000ULL:
			score += 2;
			break;
		case 0x0001010101010100ULL:
			score -= 25;
			break;
		default:
			break;
	}
	//上
	switch (*playerboard & UP_BOARD) {
		case 0x3c00000000000000ULL:
			score += 2;
			break;
		case 0x2400000000000000ULL:
			if((*oppenentboard & UP_BOARD) == 0x1800000000000000ULL) score -= 4;
			break;
		case 0x2c00000000000000ULL:
			score -= 2;
			break;
		case 0x1c00000000000000ULL:
			score -= 2;
			break;
		case 0x7e00000000000000ULL:
			score += 25;
			break;
		default:
			break;
	}
	switch (*oppenentboard & UP_BOARD) {
		case 0x3c00000000000000ULL:
			score -= 2;
			break;
		case 0x2400000000000000ULL:
			if((*playerboard & UP_BOARD) == 0x1800000000000000ULL) score += 4;
			break;
		case 0x2c00000000000000ULL:
			score += 2;
			break;
		case 0x1c00000000000000ULL:
			score += 2;
			break;
		case 0x7e00000000000000ULL:
			score -= 25;
			break;
		default:
			break;
	}
	//下
	switch (*playerboard & DOWN_BOARD) {
		case 0x000000000000003cULL:
			score += 2;
			break;
		case 0x0000000000000024ULL:
			if((*oppenentboard & DOWN_BOARD) == 0x0000000000000018ULL) score += 4;
			break;
		case 0x000000000000002cULL:
			score -= 2;
			break;
		case 0x000000000000001cULL:
			score -= 2;
			break;
		case 0x000000000000007eULL:
			score += 25;
			break;
		default:
			break;
	}
	switch (*oppenentboard & DOWN_BOARD) {
		case 0x000000000000003cULL:
			score -= 2;
			break;
		case 0x0000000000000024ULL:
			if((*playerboard & DOWN_BOARD) == 0x0000000000000018ULL) score -= 4;
			break;
		case 0x000000000000002cULL:
			score += 2;
			break;
		case 0x000000000000001cULL:
			score += 2;
			break;
		case 0x000000000000007eULL:
			score -= 25;
			break;
		default:
			break;
	}
	
	return score;
}

int score_putable(uint64_t *playerboard, uint64_t *oppenentboard) {
	return __builtin_popcountll(makelegalBoard(playerboard, oppenentboard)) - __builtin_popcountll(makelegalBoard(oppenentboard, playerboard));
}

int score_fixedstone(uint64_t *playerboard, uint64_t *oppenentboard) {
	int fixedstone = 0;
	
	//上
	if((*playerboard | *oppenentboard) & UP_BOARD) {
		for (uint64_t i = 0x8000000000000000ULL; i>=0x0100000000000000ULL; i>>=1) {
			if(*playerboard & i) fixedstone++;
			else fixedstone--;
		}
	} else {
		//左上左方向
		if(*playerboard & 0xfe00000000000000ULL) fixedstone+=7;
		else if(*playerboard & 0xfc00000000000000ULL) fixedstone+=6;
		else if(*playerboard & 0xf800000000000000ULL) fixedstone+=5;
		else if(*playerboard & 0xf000000000000000ULL) fixedstone+=4;
		else if(*playerboard & 0xe000000000000000ULL) fixedstone+=3;
		else if(*playerboard & 0xc000000000000000ULL) fixedstone+=2;
		else if(*playerboard & 0x8000000000000000ULL) fixedstone+=1;
		
		if(*oppenentboard & 0xfe00000000000000ULL) fixedstone-=7;
		else if(*oppenentboard & 0xfc00000000000000ULL) fixedstone-=6;
		else if(*oppenentboard & 0xf800000000000000ULL) fixedstone-=5;
		else if(*oppenentboard & 0xf000000000000000ULL) fixedstone-=4;
		else if(*oppenentboard & 0xe000000000000000ULL) fixedstone-=3;
		else if(*oppenentboard & 0xc000000000000000ULL) fixedstone-=2;
		else if(*oppenentboard & 0x8000000000000000ULL) fixedstone-=1;
		//右上左方向
		if(*playerboard & 0x7f00000000000000ULL) fixedstone+=7;
		else if(*playerboard & 0x3f00000000000000ULL) fixedstone+=6;
		else if(*playerboard & 0x1f00000000000000ULL) fixedstone+=5;
		else if(*playerboard & 0x0f00000000000000ULL) fixedstone+=4;
		else if(*playerboard & 0x0700000000000000ULL) fixedstone+=3;
		else if(*playerboard & 0x0300000000000000ULL) fixedstone+=2;
		else if(*playerboard & 0x0100000000000000ULL) fixedstone+=1;
		
		if(*oppenentboard & 0x7f00000000000000ULL) fixedstone-=7;
		else if(*oppenentboard & 0x3f00000000000000ULL) fixedstone-=6;
		else if(*oppenentboard & 0x1f00000000000000ULL) fixedstone-=5;
		else if(*oppenentboard & 0x0f00000000000000ULL) fixedstone-=4;
		else if(*oppenentboard & 0x0700000000000000ULL) fixedstone-=3;
		else if(*oppenentboard & 0x0300000000000000ULL) fixedstone-=2;
		else if(*oppenentboard & 0x0100000000000000ULL) fixedstone-=1;
	}
	//左
	if((*playerboard | *oppenentboard) & LEFT_BOARD) {
		for (uint64_t i = 0x8000000000000000ULL; i>=0x0000000000000080ULL; i>>=8) {
			if(*playerboard & i) fixedstone++;
			else fixedstone--;
		}
	} else {
		//左上下方向
		if(*playerboard & 0x8080808080808000ULL) fixedstone+=7;
		else if(*playerboard & 0x8080808080800000ULL) fixedstone+=6;
		else if(*playerboard & 0x8080808080000000ULL) fixedstone+=5;
		else if(*playerboard & 0x8080808000000000ULL) fixedstone+=4;
		else if(*playerboard & 0x8080800000000000ULL) fixedstone+=3;
		else if(*playerboard & 0x8080008000000000ULL) fixedstone+=2;
		
		if(*oppenentboard & 0x8080808080808000ULL) fixedstone-=7;
		else if(*oppenentboard & 0x8080808080800000ULL) fixedstone-=6;
		else if(*oppenentboard & 0x8080808080000000ULL) fixedstone-=5;
		else if(*oppenentboard & 0x8080808000000000ULL) fixedstone-=4;
		else if(*oppenentboard & 0x8080800000000000ULL) fixedstone-=3;
		else if(*oppenentboard & 0x8080008000000000ULL) fixedstone-=2;
		
		//左下上方向
		if(*playerboard & 0x0080808080808080ULL) fixedstone+=7;
		else if(*playerboard & 0x0000808080808080ULL) fixedstone+=6;
		else if(*playerboard & 0x0000008080808080ULL) fixedstone+=5;
		else if(*playerboard & 0x0000000080808080ULL) fixedstone+=4;
		else if(*playerboard & 0x0000008000808080ULL) fixedstone+=3;
		else if(*playerboard & 0x0000008000008080ULL) fixedstone+=2;
		else if(*playerboard & 0x0000008000000080ULL) fixedstone+=1;
		
		if(*oppenentboard & 0x0080808080808080ULL) fixedstone-=7;
		else if(*oppenentboard & 0x0000808080808080ULL) fixedstone-=6;
		else if(*oppenentboard & 0x0000008080808080ULL) fixedstone-=5;
		else if(*oppenentboard & 0x0000000080808080ULL) fixedstone-=4;
		else if(*oppenentboard & 0x0000008000808080ULL) fixedstone-=3;
		else if(*oppenentboard & 0x0000008000008080ULL) fixedstone-=2;
		else if(*oppenentboard & 0x0000008000000080ULL) fixedstone-=1;
	}
	//右
	if((*playerboard | *oppenentboard) & RIGHT_BOARD) {
		for (uint64_t i = 0x0100000000000000ULL; i>=0x0000000000000001ULL; i>>=8) {
			if(*playerboard & i) fixedstone++;
			else fixedstone--;
		}
	} else {
		//右上下方向
		if(*playerboard & 0x0101010101010100ULL) fixedstone+=7;
		else if(*playerboard & 0x0101010101010000ULL) fixedstone+=6;
		else if(*playerboard & 0x0101010101000000ULL) fixedstone+=5;
		else if(*playerboard & 0x0101010100000000ULL) fixedstone+=4;
		else if(*playerboard & 0x0101010000000000ULL) fixedstone+=3;
		else if(*playerboard & 0x0101000000000000ULL) fixedstone+=2;
		
		if(*oppenentboard & 0x0101010101010100ULL) fixedstone-=7;
		else if(*oppenentboard & 0x0101010101010000ULL) fixedstone-=6;
		else if(*oppenentboard & 0x0101010101000000ULL) fixedstone-=5;
		else if(*oppenentboard & 0x0101010100000000ULL) fixedstone-=4;
		else if(*oppenentboard & 0x0101010000000000ULL) fixedstone-=3;
		else if(*oppenentboard & 0x0101000000000000ULL) fixedstone-=2;
		//右下上方向
		if(*playerboard & 0x001010101010101ULL) fixedstone+=7;
		else if(*playerboard & 0x000010101010101ULL) fixedstone+=6;
		else if(*playerboard & 0x000000101010101ULL) fixedstone+=5;
		else if(*playerboard & 0x000000001010101ULL) fixedstone+=4;
		else if(*playerboard & 0x000000000010101ULL) fixedstone+=3;
		else if(*playerboard & 0x000000000000101ULL) fixedstone+=2;
		else if(*playerboard & 0x000000000000001ULL) fixedstone+=1;
		
		if(*oppenentboard & 0x001010101010101ULL) fixedstone-=7;
		else if(*oppenentboard & 0x000010101010101ULL) fixedstone-=6;
		else if(*oppenentboard & 0x000000101010101ULL) fixedstone-=5;
		else if(*oppenentboard & 0x000000001010101ULL) fixedstone-=4;
		else if(*oppenentboard & 0x000000000010101ULL) fixedstone-=3;
		else if(*oppenentboard & 0x000000000000101ULL) fixedstone-=2;
		else if(*oppenentboard & 0x000000000000001ULL) fixedstone-=1;
	}
	//下
	if((*playerboard | *oppenentboard) & DOWN_BOARD) {
		for (uint64_t i = 0x0000000000000080ULL; i>=0x0000000000000001ULL; i>>=1) {
			if(*playerboard & i) fixedstone++;
			else fixedstone--;
		}
	} else {
		//左下右方向
		if(*playerboard & 0x00000000000000feULL) fixedstone+=7;
		else if(*playerboard & 0x00000000000000fcULL) fixedstone+=6;
		else if(*playerboard & 0x00000000000000f8ULL) fixedstone+=5;
		else if(*playerboard & 0x00000000000000f0ULL) fixedstone+=4;
		else if(*playerboard & 0x00000000000000e0ULL) fixedstone+=3;
		else if(*playerboard & 0x00000000000000c0ULL) fixedstone+=2;
		
		//右下左方向
		if(*playerboard & 0x000000000000007fULL) fixedstone+=7;
		else if(*playerboard & 0x000000000000003fULL) fixedstone+=6;
		else if(*playerboard & 0x000000000000001fULL) fixedstone+=5;
		else if(*playerboard & 0x000000000000000fULL) fixedstone+=4;
		else if(*playerboard & 0x0000000000000007ULL) fixedstone+=3;
		else if(*playerboard & 0x0000000000000003ULL) fixedstone+=2;
		
	}
	return fixedstone;
}

int countscore(uint64_t *playerboard, uint64_t *oppenentboard, int *afterIndex) {
	if(!(*playerboard)) return -32766;
	if(*afterIndex >= 60) return (__builtin_popcountll(*playerboard)-__builtin_popcountll(*oppenentboard));
	if(*afterIndex >= 44) return ((score_stone(playerboard, oppenentboard)<<1)+(score_fixedstone(playerboard, oppenentboard)*55));
	return ((score_stone(playerboard, oppenentboard)*3)+(score_fixedstone(playerboard, oppenentboard)*55)+(score_putable(playerboard, oppenentboard)<<1));
}
