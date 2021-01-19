#ifndef MYAI_INCLUDED
#define MYAI_INCLUDED 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

#define RED 0
#define BLACK 1
#define CHESS_COVER -1
#define CHESS_EMPTY -2
#define COMMAND_NUM 18
#define EXACT 0
#define LOWER_BOUND 1
#define UPPER_BOUND 2
#define KILLCANNON 0
#define KILLKING 1
#define KILLGUARDS 2
#define KILLALL 3

#define INF 99999999
using namespace std;

struct entry
{
	unsigned long long int password;
	double m;
	int depth;
	int exact;
	int PV;
};

class MyAI  
{
	const char* commands_name[COMMAND_NUM] = {
		"protocol_version",
		"name",
		"version",
		"known_command",
		"list_commands",
		"quit",
		"boardsize",
		"reset_board",
		"num_repetition",
		"num_moves_to_draw",
		"move",
		"flip",
		"genmove",
		"game_over",
		"ready",
		"time_settings",
		"time_left",
  	"showboard"
	};
public:

	struct node
	{
		int board[32];
		int rbc_pieces[3][32];
		int rbc_cnt[3];

		int has_king;
		int PV;
		int value;

		unsigned long long int hash_red;
		unsigned long long int hash_black;
	};

	MyAI(void);
	~MyAI(void);

	// commands
	bool protocol_version(const char* data[], char* response);// 0
	bool name(const char* data[], char* response);// 1
	bool version(const char* data[], char* response);// 2
	bool known_command(const char* data[], char* response);// 3
	bool list_commands(const char* data[], char* response);// 4
	bool quit(const char* data[], char* response);// 5
	bool boardsize(const char* data[], char* response);// 6
	bool reset_board(const char* data[], char* response);// 7
	bool num_repetition(const char* data[], char* response);// 8
	bool num_moves_to_draw(const char* data[], char* response);// 9
	bool move(const char* data[], char* response);// 10
	bool flip(const char* data[], char* response);// 11
	bool genmove(const char* data[], char* response);// 12
	bool game_over(const char* data[], char* response);// 13
	bool ready(const char* data[], char* response);// 14
	bool time_settings(const char* data[], char* response);// 15
	bool time_left(const char* data[], char* response);// 16
	bool showboard(const char* data[], char* response);// 17

private:
	int Color = 0;
	int Red_Time, Black_Time;
	int Board[32];
	int CoverChess[14];
	int Red_Chess_Num, Black_Chess_Num;
	//int node;
	int moves_to_draw;
	int repetition;

	// Utils
	int GetFin(char c);
	int ConvertChessNo(int input);

	// Board
	void initBoardState();
	void generateMove(char move[6]);
	void MakeMove(int* board, int* red_chess_num, int* black_chess_num, int* cover_chess, const int move, const int chess);
	void MakeMove(int* board, int* red_chess_num, int* black_chess_num, int* cover_chess, const char move[6]);
	void MakeMove(const node* board_node, node* new_node, int move, const int chess);
	bool Referee(const int* board, const int Startoint, const int EndPoint, const int color);
	int Expand(const int* board, const int color, int *Result);
	int Expand(const node* board_node, const int color, vector<int> *Result);
	double Evaluate(const int* board);
	double Evaluate(const node* board_node);
	double Nega_max(const int* board, int* move, const int red_chess_num, const int black_chess_num, const int* cover_chess, const int color, const int depth, const int remain_depth);

	// Display
	void Pirnf_Chess(int chess_no,char *Result);
	void Pirnf_Chessboard();


	//Final
	void test();
	int Piece_Moves(const int* board, const int from_location_no, std::vector<int> *EatMoves, std::vector<int> *WalkMoves, const int* values);
	int CannonMoves(const int* board, const int position, vector<int> *EatMoves, vector<int> *WalkMoves, vector<int> *firepositions, const int* values);
	double MiniF4(node* board_node, const int* cover_chess, double alpha, double beta, int depth);
	double MiniG4(node* board_node, const int* cover_chess, double alpha, double beta, int depth);
	double NegaScout(node* board_node, double alpha, double beta, int depth);
	double Star1_F3(node* board_node, const int move, const int* cover_chess, double alpha, double beta, const int depth);
	int openingHeuristic(const int* board, const int cover_count);
	void prepareHash();
	int hash_result(const int color,const node* board_node, double* m, double* alpha, double* beta, int* PV, const int depth);
	void intoEndGame(const node* root);
	double EvaluateEndGame(const node* board_node);
	const bool mine[2][14] = {{1,1,1,1,1,1,1,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,1,1,1,1,1,1,1}};
	const int position_value[2][32] = {{1,2,2,1,
																		2,3,3,2,
																		2,3,3,2,
																		3,4,4,3,
																		3,4,4,3,
																		2,3,3,2,
																		2,3,3,2,
																		1,2,2,1},
																	 {4,3,3,4,
																		3,2,2,3,
																		3,2,2,3,
																		2,1,1,2,
																		2,1,1,2,
																		3,2,2,3,
																		3,2,2,3,
																		4,3,3,4}};
	const int cannon_index[14] = {0,1,0,0,0,0,0,0,1,0,0,0,0,0};
	const int pawn_index[14] = {1,0,0,0,0,0,0,1,0,0,0,0,0,0};
	const int capture_chart[14][14] = {{0,0,0,0,0,0,0,1,0,0,0,0,0,1},
																		{0,0,0,0,0,0,0,1,1,1,1,1,1,1},
																		{0,0,0,0,0,0,0,1,1,1,0,0,0,0},
																		{0,0,0,0,0,0,0,1,1,1,1,0,0,0},
																		{0,0,0,0,0,0,0,1,1,1,1,1,0,0},
																		{0,0,0,0,0,0,0,1,1,1,1,1,1,0},
																		{0,0,0,0,0,0,0,0,1,1,1,1,1,1},
																		{1,0,0,0,0,0,1,0,0,0,0,0,0,0},
																		{1,1,1,1,1,1,1,0,0,0,0,0,0,0},
																		{1,1,1,0,0,0,0,0,0,0,0,0,0,0},
																		{1,1,1,1,0,0,0,0,0,0,0,0,0,0},
																		{1,1,1,1,1,0,0,0,0,0,0,0,0,0},
																		{1,1,1,1,1,1,0,0,0,0,0,0,0,0},
																		{0,1,1,1,1,1,1,0,0,0,0,0,0,0}};
	const int move_dir_count[32] = {2, 3, 3, 2,
																 3, 4, 4, 3,
																 3, 4, 4, 3,
																 3, 4, 4, 3,
																 3, 4, 4, 3,
																 3, 4, 4, 3,
																 3, 4, 4, 3,
																 2, 3, 3, 2};
	const int move_destinations[32][4] = {{1, 4, -1, -1},
																				{2, 5, 0, -1},
																				{3, 6, 1, -1},
																				{7, 2, -1, -1},
																				{0, 5, 8, -1},
																				{1, 6, 9, 4},
																				{2, 7, 10, 5},
																				{3, 11, 6, -1},
																				{4, 9, 12, -1},
																				{5, 10, 13, 8},
																				{6, 11, 14, 9},
																				{7, 15, 10, -1},
																				{8, 13, 16, -1},
																				{9, 14, 17, 12},
																				{10, 15, 18, 13},
																				{11, 19, 14, -1},
																				{12, 17, 20, -1},
																				{13, 18, 21, 16},
																				{14, 19, 22, 17},
																				{15, 23, 18, -1},
																				{16, 21, 24, -1},
																				{17, 22, 25, 20},
																				{18, 23, 26, 21},
																				{19, 27, 22, -1},
																				{20, 25, 28, -1},
																				{21, 26, 29, 24},
																				{22, 27, 30, 25},
																				{23, 31, 26, -1},
																				{24, 29, -1, -1},
																				{25, 30, 28, -1},
																				{26, 31, 29, -1},
																				{27, 30, -1, -1}};
	const int flip_ordering[32] = {4,1,1,4,
																 0,3,3,0,
																 2,3,3,2,
																 2,3,3,2,
																 2,3,3,2,
																 2,3,3,2,
																 0,3,3,0,
																 4,1,1,4};
	const int importance[14] = {6,1,5,4,3,2,0,13,8,12,11,10,9,7};
	const int second_hand[32] = {8,9,10,11,
															 12,13,14,15,
															 16,17,18,19,
															 20,21,22,23,
															 8,9,10,11,
				 											 12,13,14,15,
				 											 16,17,18,19,
					 										 20,21,22,23};
	const bool big_pieces[14] = {0,1,0,0,1,1,1,0,1,0,0,1,1,1};
	const int endgame_values[14] = {4,50,5,5,20,50,70,4,50,5,5,20,50,70};
	const bool state_targets[4][14] = {{0,1,0,0,0,0,0,0,1,0,0,0,0,0},
																		 {0,0,0,0,0,0,1,0,0,0,0,0,0,1},
																		 {0,0,0,0,0,1,0,0,0,0,0,0,1,0},
																		 {1,1,1,1,1,1,1,1,1,1,1,1,1,1}};
	const int ori_values[14] = {30,85,35,40,50,80,165,30,85,35,40,50,80,165};
	const int corner_position[32] = {5,4,7,6,
																	 1,0,3,2,
																	 5,4,7,6,
																	 9,8,11,10,
																	 21,20,23,22,
						 											 25,24,27,26,
						 											 29,28,31,30,
							 										 25,24,27,26};
	unsigned long long int hash_red_to_move[15][32]; //14 + cover
	unsigned long long int hash_black_to_move[15][32]; //14 + cover

	};
#endif

