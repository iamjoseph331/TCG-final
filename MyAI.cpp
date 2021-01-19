#include "float.h"
#include "MyAI.h"

#define DEPTH_LIMIT 7
#define HASH_SIZE 25

int nodecnt = 0;
bool endGame = false;
bool attacking_side = false;
entry transposition_table[1 << HASH_SIZE];

MyAI::MyAI(void){}

MyAI::~MyAI(void){}

bool MyAI::protocol_version(const char* data[], char* response){
  strcpy(response, "1.0.0");
  return 0;
}

bool MyAI::name(const char* data[], char* response){
	strcpy(response, "ProtoAI");
	return 0;
}

bool MyAI::version(const char* data[], char* response){
	strcpy(response, "2.0.1");
	return 0;
}

bool MyAI::known_command(const char* data[], char* response){
  for(int i = 0; i < COMMAND_NUM; i++){
		if(!strcmp(data[0], commands_name[i])){
			strcpy(response, "true");
			return 0;
		}
	}
	strcpy(response, "false");
	return 0;
}

bool MyAI::list_commands(const char* data[], char* response){
  for(int i = 0; i < COMMAND_NUM; i++){
		strcat(response, commands_name[i]);
		if(i < COMMAND_NUM - 1){
			strcat(response, "\n");
		}
	}
	return 0;
}

bool MyAI::quit(const char* data[], char* response){
  fprintf(stderr, "Bye\n"); 
	return 0;
}

bool MyAI::boardsize(const char* data[], char* response){
  fprintf(stderr, "BoardSize: %s x %s\n", data[0], data[1]); 
	return 0;
}

bool MyAI::reset_board(const char* data[], char* response){
	this->Red_Time = -1; // unknown
	this->Black_Time = -1; // unknown
	this->initBoardState();
	return 0;
}

bool MyAI::num_repetition(const char* data[], char* response){
  sscanf(data[1], "%d", &(this->repetition));
  return 0;
}

bool MyAI::num_moves_to_draw(const char* data[], char* response){
  sscanf(data[1], "%d", &(this->moves_to_draw));
  return 0;
}

bool MyAI::move(const char* data[], char* response){
  char move[6];
	sprintf(move, "%s-%s", data[0], data[1]);
	this->MakeMove(this->Board, &this->Red_Chess_Num, &this->Black_Chess_Num, this->CoverChess, move);
	return 0;
}

bool MyAI::flip(const char* data[], char* response){
  char move[6];
	sprintf(move, "%s(%s)", data[0], data[1]);
	this->MakeMove(this->Board, &this->Red_Chess_Num, &this->Black_Chess_Num, this->CoverChess, move);
	return 0;
}

bool MyAI::genmove(const char* data[], char* response){
	// set color
	if(!strcmp(data[0], "red")){
		this->Color = RED;
	}else if(!strcmp(data[0], "black")){
		this->Color = BLACK;
	}else{
		this->Color = 2;
	}
	// genmove
  char move[6];
	this->generateMove(move);
	sprintf(response, "%c%c %c%c", move[0], move[1], move[3], move[4]);
	return 0;
}

bool MyAI::game_over(const char* data[], char* response){
  fprintf(stderr, "Game Result: %s\n", data[0]); 
	return 0;
}

bool MyAI::ready(const char* data[], char* response){
  prepareHash();
  //test();
  return 0;
}

bool MyAI::time_settings(const char* data[], char* response){
  return 0;
}

bool MyAI::time_left(const char* data[], char* response){
  if(!strcmp(data[0], "red")){
		sscanf(data[1], "%d", &(this->Red_Time));
	}else{
		sscanf(data[1], "%d", &(this->Black_Time));
	}
	fprintf(stderr, "Time Left(%s): %s\n", data[0], data[1]); 
	return 0;
}

bool MyAI::showboard(const char* data[], char* response){
  Pirnf_Chessboard();
	return 0;
}


// *********************** AI FUNCTION *********************** //

int MyAI::GetFin(char c)//get Final piece name from ID(16)
{
	static const char skind[]={'-','K','G','M','R','N','C','P','X','k','g','m','r','n','c','p'};
	for(int f=0;f<16;f++)if(c==skind[f])return f;
	return -1;
}

int MyAI::ConvertChessNo(int input)//convert ID(16) to ID(14)
{
	switch(input)
	{
	case 0:
		return CHESS_EMPTY;
		break;
	case 8:
		return CHESS_COVER;
		break;
	case 1:
		return 6;
		break;
	case 2:
		return 5;
		break;
	case 3:
		return 4;
		break;
	case 4:
		return 3;
		break;
	case 5:
		return 2;
		break;
	case 6:
		return 1;
		break;
	case 7:
		return 0;
		break;
	case 9:
		return 13;
		break;
	case 10:
		return 12;
		break;
	case 11:
		return 11;
		break;
	case 12:
		return 10;
		break;
	case 13:
		return 9;
		break;
	case 14:
		return 8;
		break;
	case 15:
		return 7;
		break;
	}
	return -1;
}

void MyAI::test()
{
	node root;
	root.hash_red = 0;
	root.hash_black = 0;
	root.has_king = 0;
	int cover_chess[14] = {0};
	std::vector<int> red, black, cover, result;

	for(int i = 0; i < 32; ++i)
	{
		std::cin >> root.board[i];
		if(root.board[i] == CHESS_EMPTY)
		{
			continue;
		}
		root.hash_red ^= hash_red_to_move[root.board[i] + 1][i];
		root.hash_black ^= hash_black_to_move[root.board[i] + 1][i];

		if(root.board[i] >= 0 && root.board[i] < 7)
		{
			red.push_back(i);
			if(this->Board[i] == 6)
			{
				root.has_king |= 2;
			}
		}
		else if(root.board[i] >= 7 && root.board[i] < 14)
		{
			black.push_back(i);
			if(this->Board[i] == 13)
			{
				root.has_king |= 1;
			}
		}
		else if(root.board[i] == CHESS_COVER)
		{
			cover.push_back(i);
		}
	}
	cout << "MakeMove:\n";
	int c;
	cin >> c;
	this->Color = c;

	copy(red.begin(), red.end(), root.rbc_pieces[0]);
	copy(black.begin(), black.end(), root.rbc_pieces[1]);
	copy(cover.begin(), cover.end(), root.rbc_pieces[2]);
	root.rbc_cnt[0] = red.size();
	root.rbc_cnt[1] = black.size();
	root.rbc_cnt[2] = cover.size();
	
	nodecnt += 1;
	if(root.rbc_cnt[2] <= 0) // Into EndGame Heuristic
	{
		intoEndGame(&root);
	}

	cover_chess[7] = 1;
	cover_chess[12] = 1;	

	double maxVal = MiniF4(&root, cover_chess, -1*INF, INF, 5);
	cout << "BestMove: " << root.PV << endl;
	cout << "Value: " << maxVal << endl;
}

void MyAI::prepareHash()
{
	random_device rd;
	mt19937_64 e2(rd());
	unsigned long long int max = 0;
	max = max - 1;
	uniform_int_distribution<unsigned long long int> dist(1, max);
	for(int i = 0; i < 15; ++i)
	{
		for(int j = 0; j < 32; ++j)
		{
			hash_red_to_move[i][j] = dist(e2);
			hash_black_to_move[i][j] = dist(e2);
			//cout << hash_red_to_move[i][j] << " ";
		}
	}
	for(int i = 0; i < (1 << HASH_SIZE); ++i)
	{
		transposition_table[i].password = 0;
		transposition_table[i].m = 0;
		transposition_table[i].depth = 0;
		transposition_table[i].exact = -1;
		transposition_table[i].PV = 0;	
	}
	return;
}

void MyAI::initBoardState()
{	
	int iPieceCount[14] = {5, 2, 2, 2, 2, 2, 1, 5, 2, 2, 2, 2, 2, 1};
	memcpy(this->CoverChess,iPieceCount,sizeof(int)*14);
	Red_Chess_Num = 16; Black_Chess_Num = 16;

	//convert to ID(14)
	int Index = 0;
	for(int i=0;i<8;i++)
	{
		for(int j=0;j<4;j++)
		{
			this->Board[Index] = CHESS_COVER;
			Index++;
		}
	}
	this->Pirnf_Chessboard();
}

int MyAI::openingHeuristic(const int* board, const int cover_count)
{
	if(cover_count == 32)
	{
		return 2525;
	}
	else
	{
		for(int i = 0; i < 32; ++i)
		{
			if(board[i] != CHESS_COVER)
			{
				return 101 * second_hand[i];
			}
		}
	}
	return 404;
}

void MyAI::intoEndGame(const node* root)
{
	int red_big_pieces_count = 0, red_big_pieces_value = 0;
	int black_big_pieces_count = 0, black_big_pieces_value = 0;
	for(int i = 0; i < root->rbc_cnt[0]; ++i)
	{
		int pos = root->rbc_pieces[0][i];
		if(big_pieces[root->board[pos]])
		{
			red_big_pieces_count += 1;
			red_big_pieces_value += endgame_values[root->board[pos]];
		}
	}
	for(int i = 0; i < root->rbc_cnt[1]; ++i)
	{
		int pos = root->rbc_pieces[1][i];
		if(big_pieces[root->board[pos]])
		{
			black_big_pieces_count += 1;
			black_big_pieces_value += endgame_values[root->board[pos]];
		}
	}

	if(red_big_pieces_count >= black_big_pieces_count && red_big_pieces_value > black_big_pieces_value)
	{
		attacking_side = RED;		
		endGame = true;
	}
	else if(black_big_pieces_count >= red_big_pieces_count && black_big_pieces_value > red_big_pieces_value)
	{
		attacking_side = BLACK;
		endGame = true;
	}
	cout << "EndGame: " << endGame << endl;
	return;
}

void MyAI::generateMove(char move[6])
{
	/* generateMove Call by reference: change src,dst */
	//this->node = 0;
	endGame = false;
	int startPoint = 0;
	int EndPoint = 0;

	node root;
	root.hash_red = 0;
	root.hash_black = 0;
	root.has_king = 0;
	for(int i = 0; i < 3; ++i)root.rbc_cnt[i] = 0;
	for(int i = 0; i < 32; ++i)
	{
		root.board[i] = this->Board[i];
		if(this->Board[i] == CHESS_EMPTY)
		{
			continue;
		}
		root.hash_red ^= hash_red_to_move[this->Board[i] + 1][i];
		root.hash_black ^= hash_black_to_move[this->Board[i] + 1][i];
		if(this->Board[i] >= 0 && this->Board[i] < 7)
		{
			root.rbc_pieces[0][root.rbc_cnt[0]] = i;
			root.rbc_cnt[0] += 1;
			if(this->Board[i] == 6)
			{
				root.has_king |= 2;
			}
		}
		else if(this->Board[i] >= 7 && this->Board[i] < 14)
		{
			root.rbc_pieces[1][root.rbc_cnt[1]] = i;
			root.rbc_cnt[1] += 1;
			if(this->Board[i] == 13)
			{
				root.has_king |= 1;
			}
		}
		else if(this->Board[i] == CHESS_COVER)
		{
			root.rbc_pieces[2][root.rbc_cnt[2]] = i;
			root.rbc_cnt[2] += 1;
		}
	}
	
	nodecnt = 1;
	if(root.rbc_cnt[2] <= 2) // Into EndGame Heuristic
	{
		intoEndGame(&root);
	}

	int cover_count = root.rbc_cnt[2];
	int maxVal = 0, best_move;
	if(cover_count > 30)
	{
		best_move = openingHeuristic(this->Board, cover_count);
	}
	else
	{
		maxVal = MiniF4(&root, this->CoverChess, -1*INF, INF, DEPTH_LIMIT);
		best_move = root.PV;
	}
	
	//double t = Nega_max(this->Board, &best_move, this->Red_Chess_Num, this->Black_Chess_Num, this->CoverChess, this->Color, 0, DEPTH_LIMIT);
	
	startPoint = best_move/100;
	EndPoint   = best_move%100;
	sprintf(move, "%c%c-%c%c",'a'+(startPoint%4),'1'+(7-startPoint/4),'a'+(EndPoint%4),'1'+(7-EndPoint/4));

	char chess_Start[4]="";
	char chess_End[4]="";
	Pirnf_Chess(this->Board[startPoint],chess_Start);
	Pirnf_Chess(this->Board[EndPoint],chess_End);
	printf("My result: \n--------------------------\n");
	printf("Nega max: %d (node: %d)\n", maxVal, nodecnt);
	printf("(%d) -> (%d)\n",startPoint,EndPoint);
	printf("<%s> -> <%s>\n",chess_Start,chess_End);
	printf("move:%s\n",move);
	printf("--------------------------\n");
	this->Pirnf_Chessboard();
}

void MyAI::MakeMove(const node* board_node, node* new_node, int move, const int chess)//beware param chess
{
	int src = move / 100, dst = move % 100;
	int piece = board_node->board[src];
	int color = mine[1][piece];

	for(int i = 0; i < 32; ++i)
	{
		new_node->board[i] = board_node->board[i];
		for(int j = 0; j < 3; ++j)
		{
			new_node->rbc_pieces[j][i] = board_node->rbc_pieces[j][i];
		}
	}
	for(int j = 0; j < 3; ++j)
	{
		new_node->rbc_cnt[j] = board_node->rbc_cnt[j];
	}

	new_node->has_king = board_node->has_king;

	if(src == dst) //flip
	{
		new_node->board[src] = chess;

		new_node->hash_red = board_node->hash_red ^ hash_red_to_move[0][src];
		new_node->hash_red ^= hash_red_to_move[chess + 1][src];
		new_node->hash_black = board_node->hash_black ^ hash_black_to_move[0][src];
		new_node->hash_black ^= hash_black_to_move[chess + 1][src];
		
		int belong = mine[1][chess];
		new_node->rbc_pieces[belong][new_node->rbc_cnt[belong]] = src;
		new_node->rbc_cnt[belong] += 1;

		bool flag = false;
		for(int i = 0; i < new_node->rbc_cnt[2] - 1; ++i)
		{
			if(flag)
			{
				new_node->rbc_pieces[2][i] = new_node->rbc_pieces[2][i + 1]; 
			}
			else if(dst == new_node->rbc_pieces[2][i])
			{
				new_node->rbc_pieces[2][i] = new_node->rbc_pieces[2][i + 1]; 
				flag = true;
			}
		}

		new_node->rbc_cnt[2] -= 1;//cover
	}
	else
	{
		new_node->hash_red = board_node->hash_red;
		new_node->hash_black = board_node->hash_black;

		int enemy_piece = board_node->board[dst];
		if(enemy_piece >= 0)//吃掉 !color 的棋 找出來減掉
		{
			new_node->hash_red ^= hash_red_to_move[enemy_piece + 1][dst];
			new_node->hash_black ^= hash_black_to_move[enemy_piece + 1][dst]; 

			if(enemy_piece % 7 == 6)
			{
				new_node->has_king &= (!color + 1);
			}

			bool flag = false;
			for(int i = 0; i < new_node->rbc_cnt[!color] - 1; ++i)
			{
				if(flag)
				{
					new_node->rbc_pieces[!color][i] = new_node->rbc_pieces[!color][i + 1]; 
				}
				else if(dst == new_node->rbc_pieces[!color][i])
				{
					new_node->rbc_pieces[!color][i] = new_node->rbc_pieces[!color][i + 1]; 
					flag = true;
				}
			}
			new_node->rbc_cnt[!color] -= 1;
		}

		for(int i = 0; i < new_node->rbc_cnt[color]; ++i)
		{
			if(src == new_node->rbc_pieces[color][i])
			{
				new_node->rbc_pieces[color][i] = dst;
				break;
			}
		}
		int myPiece = board_node->board[src];
		new_node->hash_red ^= hash_red_to_move[myPiece + 1][src];//去掉自己(移動)
		new_node->hash_red ^= hash_red_to_move[myPiece + 1][dst];//加上自己(移動)
		new_node->hash_black ^= hash_black_to_move[myPiece + 1][src];//去掉自己(移動)
		new_node->hash_black ^= hash_black_to_move[myPiece + 1][dst];//加上自己(移動)

		new_node->board[dst] = board_node->board[src];
		new_node->board[src] = CHESS_EMPTY;
	}
	return;
}

void MyAI::MakeMove(int* board, int* red_chess_num, int* black_chess_num, int* cover_chess, const char move[6])
{ 
	int src, dst;
	src = ('8'-move[1])*4+(move[0]-'a');
	if(move[2]=='('){ 
		printf("# call flip(): flip(%d,%d) = %d\n",src, src, GetFin(move[3])); 
		board[src] = ConvertChessNo(GetFin(move[3]));
		cover_chess[ConvertChessNo(GetFin(move[3]))]--;
		Pirnf_Chessboard();
	}else { 
		dst = ('8'-move[4])*4+(move[3]-'a');
		printf("# call move(): move : %d-%d \n",src,dst); 
		if(board[dst] != CHESS_EMPTY){
			if(board[dst] / 7 == 0){
				(*red_chess_num)--;
			}else{
				(*black_chess_num)--;
			}
		}
		board[dst] = board[src];
		board[src] = CHESS_EMPTY;
		Pirnf_Chessboard();
	}
	/* init time */
}

int MyAI::CannonMoves(const int* board, const int position, vector<int> *EatMoves, vector<int> *WalkMoves, const int* values)
{
	int value = 0;
	int from = 100 * position;
	int dirs = move_dir_count[position];
	for(int i = 0; i < dirs; ++i)
	{
		int to = move_destinations[position][i];
		if(board[to] == CHESS_EMPTY)
		{
			WalkMoves->push_back(from + to);
		}
	}
	int cannon = board[position];
	
	bool flag = false;
	for(int i = position + 4; i < 32; i += 4)
	{
		if(flag && board[i] >= 0)
		{
			if(capture_chart[cannon][board[i]])
			{
				EatMoves->push_back(from + i);
				value += values[board[i]];
				break;
			}
			else break;
		}
		if(board[i] != CHESS_EMPTY)
		{
			if(flag)
				break;
			flag = true;
		}
	}

	flag = false;
	for(int i = position - 4; i >= 0; i -= 4)
	{
		if(flag && board[i] >= 0)
		{
			if(capture_chart[cannon][board[i]])
			{
				EatMoves->push_back(from + i);
				value += values[board[i]];
				break;
			}
			else break;
		}
		if(board[i] != CHESS_EMPTY)
		{
			if(flag)
				break;
			flag = true;
		}
	}

	flag = false;
	for(int i = position - 1; (i + 4) % 4 != 3; i -= 1)
	{
		if(flag && board[i] >= 0)
		{
			if(capture_chart[cannon][board[i]])
			{
				EatMoves->push_back(from + i);
				value += values[board[i]];
				break;
			}
			else break;
		}
		if(board[i] != CHESS_EMPTY)
		{
			if(flag)
				break;
			flag = true;
		}
	}	

	flag = false;
	for(int i = position + 1; i % 4 != 0; i += 1)
	{
		if(flag && board[i] >= 0)
		{
			if(capture_chart[cannon][board[i]])
			{
				EatMoves->push_back(from + i);
				value += values[board[i]];
				break;
			}
			else break;
		}
		if(board[i] != CHESS_EMPTY)
		{
			if(flag)
				break;
			flag = true;
		}
	}
	return value;
}

int MyAI::Expand(const int* board, const int* pieces, const int p_cnt, const int* covers, const int c_cnt, vector<int> *Result) 
{
	Result->clear();
	vector<int> eats, walks, flips;
	
	for(int i = 0; i < p_cnt; ++i)
	{
		int a = pieces[i];
		if(cannon_index[board[a]])
		{
			CannonMoves(board, a, &eats, &walks, ori_values);
		}
		else
		{
			Piece_Moves(board, a, &eats, &walks, ori_values);
		}
	}
	for(int i = 0; i < c_cnt; ++i)
	{
		flips.push_back(covers[i] * 101);
	}
	int e = eats.size(), w = walks.size(), f = flips.size();
	Result->reserve(e + w + f);

	//bucket sort
	int eat_bucket[14][6] = {0};
	int eb_cnt[14] = {0};
	int walk_bucket[14][4] = {0};
	int wb_cnt[14] = {0};
	int flip_bucket[5][12] = {0};
	int fb_cnt[5] = {0};
	for(auto a : eats)
	{
		int enemy = board[a % 100];
		eat_bucket[enemy][eb_cnt[enemy]] = a;
		eb_cnt[enemy] += 1;
	}
	for(auto a : walks)
	{
		int moved_piece = board[a / 100];
		walk_bucket[moved_piece][wb_cnt[moved_piece]] = a;
		wb_cnt[moved_piece] += 1;
	}
	for(auto a : flips)
	{
		int priority = flip_ordering[a / 100];
		flip_bucket[priority][fb_cnt[priority]] = a;
		fb_cnt[priority] += 1;
	}

	//move ordering
	int altitude = 0;
	for(int i = 0; i < 14; ++i)
	{
		int cnt = eb_cnt[importance[i]];
		if(cnt > 0)
		{
			for(int j = 0; j < cnt; ++j)
			{
				Result->push_back(eat_bucket[importance[i]][j]);	
			}
			altitude += cnt;
		}
	}
	for(int i = 0; i < 14; ++i)
	{
		int cnt = wb_cnt[importance[i]];
		if(cnt > 0)	
		{
			for(int j = 0; j < cnt; ++j)
			{
				Result->push_back(walk_bucket[importance[i]][j]);
			}
			altitude += cnt;
		}
	}
	for(int i = 0; i < 5; ++i)
	{
		int cnt = fb_cnt[i];
		if(cnt)
		{
			for(int j = 0; j < cnt; ++j)
			{
				Result->push_back(flip_bucket[i][j]);
			}
			altitude += cnt;
		}
	}
	//flip

	return e + w;
}

int MyAI::Piece_Moves(const int* board, const int from_location_no, vector<int>* EatMoves, vector<int>* WalkMoves, const int* values)
{
	int value = 0;
	int mv_cnt = move_dir_count[from_location_no];
	int from = from_location_no * 100;
	bool isCannon = cannon_index[ board[from_location_no] ];
	if(isCannon)
	{
		value = CannonMoves(board, from_location_no, EatMoves, WalkMoves, values);
	}
	else
	{
		for(int i = 0; i < mv_cnt; ++i)
		{
			int des = move_destinations[from_location_no][i];
			if(board[des] == CHESS_COVER)
			{
				continue;//can't move to cover
			}
			else if(board[des] == CHESS_EMPTY)
			{
				WalkMoves->push_back(from + des);
			}
			else
			{
				if(capture_chart[board[from_location_no]][board[des]])
				{
					EatMoves->push_back(from + des);
					value += values[board[des]];
				}
			}
		}
	}
	return value;
}
// always use my point of view, so use this->Color
double MyAI::EvaluateEndGame(const node* board_node)
{
	double my_score = 0.0, ene_score = 0.0;
	double suicide_prevent = 2.0;
	int state = KILLCANNON;
	int my_color = this->Color;
	int ene_color = !this->Color;
	int modified_values[14] = {40,500,50,50,200,500,700,40,500,50,50,200,500,700};

	if(my_color == attacking_side)
	{
		//find state
		for(int i = 0; i < 4; ++i)
		{
			bool thisState = false;
			for(int j = 0; j < board_node->rbc_cnt[ene_color]; ++j)
			{
				int ene_piece = board_node->board[ board_node->rbc_pieces[ene_color][j] ];
				if(state_targets[i][ene_piece])
				{
					thisState = true;
					break;
				}
			}
			if(thisState)
			{
				state = i;
			}
		}

		//start Evaluate
		vector<int> tragets;
		int offset = 7 * ene_color;
		if(state == KILLCANNON)
		{
			modified_values[offset + 1] = 1000;//Enemy Cannon value modify
			modified_values[offset + 6] = 800;//Enemy King value modify
			modified_values[offset + 5] = 600;//Enemy Guard value modify
			modified_values[offset + 4] = 300;//Enemy Mammoth value modify
		}
		else if(state == KILLKING)
		{
			modified_values[7 - offset] = 400;
			modified_values[offset + 1] = 600;//Enemy Cannon value modify
			modified_values[offset + 6] = 1000;//Enemy King value modify
			modified_values[offset + 5] = 600;//Enemy Guard value modify
			modified_values[offset + 4] = 300;//Enemy Mammoth value modify
		}
		else if(state == KILLGUARDS)
		{
			modified_values[offset + 1] = 600;//Enemy Cannon value modify
			modified_values[offset + 6] = 800;//Enemy King value modify
			modified_values[offset + 5] = 1000;//Enemy Guard value modify
			modified_values[offset + 4] = 300;//Enemy Mammoth value modify
		}
		else if(state == KILLALL)
		{
			modified_values[offset + 0] = 900;
			modified_values[offset + 1] = 990;
			modified_values[offset + 2] = 950;
			modified_values[offset + 3] = 960;
			modified_values[offset + 4] = 9700;
			modified_values[offset + 5] = 980;
			modified_values[offset + 6] = 1000;
		}
		else
		{
			cout << "Error State!\n";
		}
	}
	else //defending side
	{
		int offset = 7 * my_color;
		for(int i = offset; i < offset + 7; ++i)
		{
			modified_values[i] *= 10;
		}
	}

	vector<int> eat_moves, walk_moves;
	
	for(int i = 0; i < board_node->rbc_cnt[my_color]; ++i)
	{
		int pos = board_node->rbc_pieces[my_color][i];
		my_score += modified_values[board_node->board[pos]]; //子力價值
		my_score += 0.5 * Piece_Moves(board_node->board, pos, &eat_moves, &walk_moves, modified_values);//可以吃到對方的分數

		int my_piece = board_node->board[pos];
		int corner_pos = corner_position[pos];
		int corner_piece = board_node->board[corner_pos];
		if(!cannon_index[my_piece] && corner_piece >= 0 && capture_chart[my_piece][corner_piece])
		{
		 //位置得分
			my_score += 0.8 * modified_values[corner_piece];
		}
	}
	int cnt = board_node->rbc_cnt[ene_color];
	for(int i = 0; i < cnt; ++i)
	{
		eat_moves.clear();
		walk_moves.clear();
		int b = board_node->rbc_pieces[ene_color][i];
		ene_score += 2 * modified_values[board_node->board[b]]; //子力價值
		ene_score += suicide_prevent * Piece_Moves(board_node->board, b, &eat_moves, &walk_moves, modified_values);//可以吃到對方的分數
		ene_score += modified_values[board_node->board[b]] * 0.25 * (walk_moves.size() + eat_moves.size());
	}
	
	return my_score - ene_score;
}

// always use my point of view, so use this->Color
double MyAI::Evaluate(const node* board_node)
{
	if(endGame)
	{
		return EvaluateEndGame(board_node);
	}
	// total score
	double red_score = 0.0, black_score = 0.0;
	double attack_rate = 1.0;
	double piece_rate = 3.0;
	double pawn_bonus = 1.0;
	// static material values
	// cover and empty are both zero

	vector<int> eat_moves, walk_moves;
	
	for(int i = 0; i < board_node->rbc_cnt[0]; ++i)
	{
		int a = board_node->rbc_pieces[0][i];
		int my_piece = board_node->board[a];
		if(pawn_index[my_piece] && (board_node->has_king & 1))
		{
			pawn_bonus = 2.0;
		}
		else 
		{
			pawn_bonus = 1.0;
		}
		red_score += piece_rate * pawn_bonus * ori_values[my_piece]; //子力價值
		red_score += attack_rate * Piece_Moves(board_node->board, a, &eat_moves, &walk_moves, ori_values);//可以吃到對方的分數
		red_score += position_value[cannon_index[my_piece]][a];//位置得分
	}

	for(int i = 0; i < board_node->rbc_cnt[1]; ++i)
	{
		int b = board_node->rbc_pieces[1][i];
		int ene_piece = board_node->board[b];
		if(pawn_index[ene_piece] && (board_node->has_king > 1))
		{
			pawn_bonus = 2.0;
		}
		else 
		{
			pawn_bonus = 1.0;
		}
		black_score += piece_rate * pawn_bonus * ori_values[ene_piece]; //子力價值
		black_score += Piece_Moves(board_node->board, b, &eat_moves, &walk_moves, ori_values);//可以吃到對方的分數
		black_score += position_value[cannon_index[ene_piece]][b];//位置得分
	}
	if(this->Color == RED)
	{
		return red_score - black_score;
	}
	return black_score - red_score;
}

//result of looking up transposition table
//return value: to continue search or not 
int MyAI::hash_result(const int color, const node* board_node, double* m, double* alpha, double* beta, int* PV, const int depth)
{
	unsigned long long int entry_id = 0;
	if(color == RED)
	{
		entry_id = board_node->hash_red & ((1 << HASH_SIZE) - 1);
		if(transposition_table[entry_id].password != board_node->hash_red)//hash miss
		{
			return -1;
		}
	}
	else
	{
		entry_id = board_node->hash_black & ((1 << HASH_SIZE) - 1);
		if(transposition_table[entry_id].password != board_node->hash_black)//hash miss
		{
			return -1;
		}
	}
	// test depth
	int ex = transposition_table[entry_id].exact;
	if(transposition_table[entry_id].depth >= DEPTH_LIMIT - depth)
	{
		*PV = transposition_table[entry_id].PV;
		if(ex == EXACT)
		{
			*m = transposition_table[entry_id].m;
			return 1;
		}
		else if(ex == LOWER_BOUND)
		{
			*alpha = transposition_table[entry_id].m;
		}
		else if(ex == UPPER_BOUND)
		{
			*beta = transposition_table[entry_id].m;
		}
	}
	else
	{
		//參考
		if(ex == EXACT)
		{
			*m = transposition_table[entry_id].m;
			*PV = transposition_table[entry_id].PV;
			return 0;
		}
		return -1;
	}
	return 0;
}

double MyAI::MiniF4(node* board_node, const int* cover_chess, double alpha, double beta, int depth)
{
	bool color = this->Color;
	unsigned long long int entry_id = (color == RED ? board_node->hash_red & ((1 << HASH_SIZE) - 1) : board_node->hash_black & ((1 << HASH_SIZE) - 1));
	unsigned long long int pass = (color == RED ? board_node->hash_red : board_node->hash_black);
	double m = -1 * INF;
	int pv;
	if(hash_result(color, board_node, &m, &alpha, &beta, &pv, depth) == 1)
	{
		board_node->value = m;
		board_node->PV = pv;
		return m;
	}
	transposition_table[entry_id].password = pass;
	transposition_table[entry_id].depth = DEPTH_LIMIT - depth;

	vector<int> successor_positions;
	int ew = Expand(board_node->board, board_node->rbc_pieces[color], board_node->rbc_cnt[color], board_node->rbc_pieces[2], board_node->rbc_cnt[2], &successor_positions);
	int b = successor_positions.size();
	if(b == 0 || depth == 0)
	{
		int val = Evaluate(board_node);
		board_node->value = val;
		return val;
	}

	//begin
	node p1;
	if(ew != 0)
	{
		MakeMove(board_node, &p1, successor_positions[0], -1);
		m = max(m, MiniG4(&p1, cover_chess, alpha, beta, depth - 1));
		board_node->PV = successor_positions[0];
		board_node->value = m;

		if(m >= beta)
		{	
			transposition_table[entry_id].m = m;
			transposition_table[entry_id].exact = LOWER_BOUND;
			transposition_table[entry_id].PV = successor_positions[0];
			return m;
		}
	}
	//loop 過所有選項：先做吃子，走步再翻子
	for(int i = 1; i < ew; ++i)
	{
		node pi;
		MakeMove(board_node, &pi, successor_positions[i], -1);
		double t = MiniG4(&pi, cover_chess, m, m+1, depth - 1);
		if(t > m)//failed high
		{
			board_node->PV = successor_positions[i];
			if(depth < 3 || t >= beta)
			{
				m = t;
			}
			else
			{
				m = MiniG4(&pi, cover_chess, t, beta, depth - 1);
			}
		}
		if(m >= beta)
		{
			board_node->value = m;
			transposition_table[entry_id].m = m;
			transposition_table[entry_id].exact = LOWER_BOUND;
			transposition_table[entry_id].PV = board_node->PV;
			return m;
		}
	}

	//開找翻子棋
	for(int i = ew; i < b; ++i)
	{
		double vsum = 0.0;
		double c = 0;
		for(int j = 0; j < 14; ++j)
		{
			if(cover_chess[j] != 0)
			{
				node pi;
				MakeMove(board_node, &pi, successor_positions[i], j);
				double t = MiniG4(&pi, cover_chess, m, beta, min(2, depth - 1));
				vsum += t * cover_chess[j];
				c += cover_chess[j];
			}
		}
		double chance = vsum / c;
		if(chance > m)
		{
			board_node->PV = successor_positions[i];
			m = chance;
		}
		if(m >= beta)
		{
			transposition_table[entry_id].m = m;
			transposition_table[entry_id].exact = LOWER_BOUND;
			transposition_table[entry_id].PV = board_node->PV;
			board_node->value = m;
			return m;
		}
		if(i > ew + 5)
			break;
	}

	//end
	transposition_table[entry_id].m = m;
	transposition_table[entry_id].PV = board_node->PV;
	if(m > alpha)
	{
		transposition_table[entry_id].exact = EXACT;
	}
	else
	{
		transposition_table[entry_id].exact = UPPER_BOUND;
	}
	board_node->value = m;
	return m;
}
 
double MyAI::MiniG4(node* board_node, const int* cover_chess, double alpha, double beta, int depth)
{
	bool color = !(this->Color);
	unsigned long long int entry_id = (color == RED ? board_node->hash_red & ((1 << HASH_SIZE) - 1) : board_node->hash_black & ((1 << HASH_SIZE) - 1));
	unsigned long long int pass = (color == RED ? board_node->hash_red : board_node->hash_black);
	double m = INF;
	int pv;
	if(hash_result(color, board_node, &m, &alpha, &beta, &pv, depth) == 1)
	{
		board_node->value = m;
		board_node->PV = pv;
		return m;
	}
	transposition_table[entry_id].password = pass;
	transposition_table[entry_id].depth = DEPTH_LIMIT - depth;

	vector<int> successor_positions;
	int ew = Expand(board_node->board, board_node->rbc_pieces[color], board_node->rbc_cnt[color], board_node->rbc_pieces[2], board_node->rbc_cnt[2], &successor_positions);
	int b = successor_positions.size();
	if(b == 0 || depth == 0)
	{
		int val = Evaluate(board_node);
		board_node->value = val;
		return val;
	}

	//begin	
	node p1;
	if(ew != 0)
	{
		MakeMove(board_node, &p1, successor_positions[0], -1);
		m = min(m, MiniF4(&p1, cover_chess, alpha, beta, depth - 1));
		board_node->PV = successor_positions[0];
		board_node->value = m;

		if(m <= alpha)
		{
			transposition_table[entry_id].m = m;
			transposition_table[entry_id].exact = UPPER_BOUND;
			transposition_table[entry_id].PV = successor_positions[0];
			return m;
		}	
	}
	//loop 過所有選項：先做吃子，走步再翻子
	for(int i = 1; i < ew; ++i)
	{
		node pi;
		MakeMove(board_node, &pi, successor_positions[i], -1);
		double t = MiniF4(&pi, cover_chess, m - 1, m, depth - 1);
		if(t < m)//failed low
		{
			board_node->PV = successor_positions[i];
			if(depth < 3 || t <= alpha)
			{
				m = t;
			}
			else
			{
				m = MiniF4(&pi, cover_chess, alpha, t, depth - 1);
			}
		}
		if(m <= alpha)
		{
			board_node->value = m;
			transposition_table[entry_id].m = m;
			transposition_table[entry_id].exact = UPPER_BOUND;
			transposition_table[entry_id].PV = board_node->PV;
			return m;
		}
	}
	//end

	//開找翻子棋
	for(int i = ew; i < b; ++i)
	{
		double vsum = 0.0, c = 0;
		for(int j = 0; j < 14; ++j)
		{
			if(cover_chess[j] != 0)
			{
				node pi;
				MakeMove(board_node, &pi, successor_positions[i], j);
				double t = MiniF4(&pi, cover_chess, alpha, m, min(2, depth - 1));
				vsum += t * cover_chess[j];
				c += cover_chess[j];
			}
		}
		double chance = vsum / c;
		if(chance < m)
		{
			board_node->PV = successor_positions[i];
			m = chance;
		}
		if(m <= alpha)
		{
			transposition_table[entry_id].m = m;
			transposition_table[entry_id].exact = UPPER_BOUND;
			transposition_table[entry_id].PV = board_node->PV;
			board_node->value = m;
			return m;
		}
		if(i > ew + 5)
			break;
	}

	transposition_table[entry_id].m = m;
	transposition_table[entry_id].PV = board_node->PV;
	if(m < beta)
	{
		transposition_table[entry_id].exact = EXACT;
	}
	else
	{
		transposition_table[entry_id].exact = LOWER_BOUND;
	}
	board_node->value = m;
	return m;
}

double MyAI::NegaScout(node* board_node, double alpha, double beta, int depth)
{
	bool color = (depth&1) ^ (!(this->Color));
	vector<int> successor_positions;
	Expand(board_node->board, board_node->rbc_pieces[color], board_node->rbc_cnt[color], board_node->rbc_pieces[2], board_node->rbc_cnt[2], &successor_positions);
	int b = successor_positions.size();
	if(b == 0 || depth == 0)
	{
		return Evaluate(board_node);
	}
	//begin
	double m = -1 * INF;
	double n = beta;
	for(int i = 0; i < b; ++i)
	{
		node pi;
		MakeMove(board_node, &pi, successor_positions[i], -1);
		double t = -1 * NegaScout(&pi, -1 * n, -1 * max(alpha, m), depth - 1);
		if(t > m)
		{
			if(n == beta || depth < 3 || t >= beta)
			{
				m = t;
			}
			else
			{
				m = -1 * NegaScout(&pi, -1 * beta, -1 * t, depth - 1);
			}
		}
		if(m >= beta)return m;
		n = max(alpha, m) + 1;
	}

	/*
	for(int i = ew; i < b; ++i)
	{
		node pi;
		double t = Star1_F3(board_node, successor_positions[i], cover_chess, max(alpha, m), beta);
		if(t > m)
		{
			m = t;
		}
		if(m >= beta)
		{
			board_node->value = m;
			return m;//chance beta cutoff
		}
	}*/
	return m;
}

double MyAI::Star1_F3(node* board_node, const int move, const int* cover_chess, double alpha, double beta, const int depth)
{
	double values[15] = {0};
	double A[15], B[15], m[15], M[15];
	double vsum = 0, vmin = INF, vmax = -1*INF;
	double c = 0;
	node ki[15];//讓他變成1~c
	for(int i = 0; i < 14; ++i)
	{
		if(cover_chess[i] != 0)
		{
			MakeMove(board_node, &ki[i+1], move, i);
			values[i] = Evaluate(&ki[i+1]);
			vmax = max(vmax, values[i]);
			vmin = min(vmin, values[i]);
			c += 1;
		}
	}

	A[0] = c * (alpha - vmax) + vmax; B[0] = c * (beta - vmin) + vmin;
	m[0] = vmin; M[0] = vmax;
	vsum = 0.0;
	for(int i = 1; i <= c; ++i)
	{
		double t = MiniG4(&ki[i], cover_chess, max(A[i-1], vmin), min(B[i-1], vmax), depth - 1);
		m[i] = m[i-1] + (t - vmin)/c; 
		M[i] = M[i-1] + (t - vmax)/c;
		if(t >= B[i-1])
		{
			return m[i];//failed high, chance node cutoff I
		}
		if(t <= A[i-1])
		{
			return M[i];//failed low, chance node cut off II
		}
		vsum += t;
		A[i] = A[i-1] + vmax - t;
		B[i] = B[i-1] + vmin - t;
	}
	return vsum / c;
}

//Display chess board
void MyAI::Pirnf_Chessboard()
{	
	char Mes[1024]="";
	char temp[1024];
	char myColor[10]="";
	if(Color == -99)
		strcpy(myColor,"Unknown");
	else if(this->Color == RED)
		strcpy(myColor,"Red");
	else
		strcpy(myColor,"Black");
	sprintf(temp,"------------%s-------------\n",myColor);
	strcat(Mes,temp);
	strcat(Mes,"<8> ");
	for(int i=0;i<32;i++)
	{
		if(i != 0 && i % 4 == 0)
		{
			sprintf(temp,"\n<%d> ",8-(i/4));
			strcat(Mes,temp);
		}
		char chess_name[4]="";
		Pirnf_Chess(this->Board[i],chess_name);
		sprintf(temp,"%5s", chess_name);
		strcat(Mes,temp);
	}
	strcat(Mes,"\n\n     ");
	for(int i=0;i<4;i++)
	{
		sprintf(temp," <%c> ",'a'+i);
		strcat(Mes,temp);
	}
	strcat(Mes,"\n\n");
	printf("%s",Mes);
}

//Print chess
void  MyAI::Pirnf_Chess(int chess_no,char *Result)
{
		// XX -> Empty
		if(chess_no == CHESS_EMPTY)
		{	
			strcat(Result, " - ");
			return;
		}
		//OO -> DarkChess
		else if(chess_no == CHESS_COVER)
		{
			strcat(Result, " X ");
			return;
		}
		/*if(Color == RED )
				strcat(Result, "R");
		else
				strcat(Result, "B");*/
		switch(chess_no)
		{
		case 0:
				strcat(Result, " P ");
				break;
		case 1:
				strcat(Result, " C ");
				break;
		case 2:
				strcat(Result, " N ");
				break;
		case 3:
				strcat(Result, " R ");
				break;
		case 4:
				strcat(Result, " M ");
				break;
		case 5:
				strcat(Result, " G ");
				break;
		case 6:
				strcat(Result, " K ");
				break;
		case 7:
				strcat(Result, " p ");
				break;
		case 8:
				strcat(Result, " c ");
				break;
		case 9:
				strcat(Result, " n ");
				break;
		case 10:
				strcat(Result, " r ");
				break;
		case 11:
				strcat(Result, " m ");
				break;
		case 12:
				strcat(Result, " g ");
				break;
		case 13:
				strcat(Result, " k ");
				break;
	}
}


