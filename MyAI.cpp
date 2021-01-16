#include "float.h"
#include "MyAI.h"

#define DEPTH_LIMIT 5

int nodecnt = 0;

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
	strcpy(response, "0.0.9");
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
  test();
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
	int test_board[32] = {0};
	std::vector<int> red, black, cover, result;
	for(int i = 0; i < 32; ++i)
	{
		std::cin >> test_board[i];
		if(test_board[i] >= 0 && test_board[i] < 7)
		{
			red.push_back(i);
		}
		else if(test_board[i] >= 7 && test_board[i] < 14)
		{
			black.push_back(i);
		}
		else if(test_board[i] == CHESS_COVER)
		{
			cover.push_back(i);
		}
	}
	cout << "MakeMove:\n";
	int c;
	cin >> c;
	this->Color = c;

	node root;

	copy(test_board, test_board+32, root.board);
	copy(red.begin(), red.end(), root.rbc_pieces[0]);
	copy(black.begin(), black.end(), root.rbc_pieces[1]);
	copy(cover.begin(), cover.end(), root.rbc_pieces[2]);
	root.rbc_cnt[0] = red.size();
	root.rbc_cnt[1] = black.size();
	root.rbc_cnt[2] = cover.size();
	root.has_king = 3;
	nodecnt += 1;

	double maxVal = MiniF4(&root, -1*INF, INF, 5);
	cout << "BestMove: " << root.PV << endl;
	cout << "Value: " << maxVal << endl;
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

void MyAI::generateMove(char move[6])
{
	/* generateMove Call by reference: change src,dst */
	//this->node = 0;
	int startPoint = 0;
	int EndPoint = 0;

	node root;
	std::vector<int> red, black, cover, result;
	for(int i = 0; i < 32; ++i)
	{
		if(this->Board[i] >= 0 && this->Board[i] < 7)
		{
			red.push_back(i);
		}
		else if(this->Board[i] >= 7 && this->Board[i] < 14)
		{
			black.push_back(i);
		}
		else if(this->Board[i] == CHESS_COVER)
		{
			cover.push_back(i);
		}
	}
	copy(this->Board, this->Board+32, root.board);
	copy(red.begin(), red.end(), root.rbc_pieces[0]);
	copy(black.begin(), black.end(), root.rbc_pieces[1]);
	copy(cover.begin(), cover.end(), root.rbc_pieces[2]);
	root.rbc_cnt[0] = red.size();
	root.rbc_cnt[1] = black.size();
	root.rbc_cnt[2] = cover.size();
	root.has_king = 3;
	nodecnt = 1;

	int maxVal = MiniF4(0, -1*INF, INF, DEPTH_LIMIT);

	int best_move = root.PV;
	
	//double t = Nega_max(this->Board, &best_move, this->Red_Chess_Num, this->Black_Chess_Num, this->CoverChess, this->Color, 0, DEPTH_LIMIT);
	
	startPoint = best_move/100;
	EndPoint   = best_move%100;
	sprintf(move, "%c%c-%c%c",'a'+(startPoint%4),'1'+(7-startPoint/4),'a'+(EndPoint%4),'1'+(7-EndPoint/4));

	char chess_Start[4]="";
	char chess_End[4]="";
	Pirnf_Chess(Board[startPoint],chess_Start);
	Pirnf_Chess(Board[EndPoint],chess_End);
	printf("My result: \n--------------------------\n");
	printf("Nega max: %d (node: %d)\n", maxVal, nodecnt);
	printf("(%d) -> (%d)\n",startPoint,EndPoint);
	printf("<%s> -> <%s>\n",chess_Start,chess_End);
	printf("move:%s\n",move);
	printf("--------------------------\n");
	this->Pirnf_Chessboard();
}

//Dep
void MyAI::MakeMove(int* board, int* red_chess_num, int* black_chess_num, int* cover_chess, const int move, const int chess)
{
	int src = move/100, dst = move%100;
	if(src == dst){ 
		board[src] = chess;
		cover_chess[chess] -= 1;
	}else { 
		if(board[dst] != CHESS_EMPTY){
			if(board[dst] / 7 == 0){
				(*red_chess_num)--;
			}else{
				(*black_chess_num)--;
			}
		}
		board[dst] = board[src];
		board[src] = CHESS_EMPTY;
	}
}

void MyAI::MakeMove(const node* board_node, node* new_node, int move, const int chess)//beware param chess
{
	int src = move / 100, dst = move % 100;
	int color = mine[1][chess];

	copy(board_node->board, board_node->board + 32, new_node->board);

	copy(board_node->rbc_pieces[color], board_node->rbc_pieces[color] + 16, new_node->rbc_pieces[color]);
	copy(board_node->rbc_pieces[!color], board_node->rbc_pieces[!color] + 16, new_node->rbc_pieces[!color]);
	copy(board_node->rbc_pieces[2], board_node->rbc_pieces[2] + 16, new_node->rbc_pieces[2]);

	copy(board_node->rbc_cnt, board_node->rbc_cnt + 3, new_node->rbc_cnt);
	new_node->has_king = board_node->has_king;

	if(src == dst) //flip
	{
		new_node->board[src] = chess;
		new_node->rbc_pieces[color][new_node->rbc_cnt[color]] = src;
		new_node->rbc_cnt[color] += 1;

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
		if(board_node->board[dst] >= 0)//吃掉 !color 的棋 找出來減掉
		{
			if(board_node->board[dst] % 7 == 6)
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

int MyAI::CannonMoves(const int* board, const int position, vector<int> *EatMoves, vector<int> *WalkMoves)
{
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
				break;
			}
			else break;
		}
		if(board[i] != CHESS_EMPTY)
		{
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
				break;
			}
			else break;
		}
		if(board[i] != CHESS_EMPTY)
		{
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
				break;
			}
			else break;
		}
		if(board[i] != CHESS_EMPTY)
		{
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
				break;
			}
			else break;
		}
		if(board[i] != CHESS_EMPTY)
		{
			flag = true;
		}
	}
	return EatMoves->size() + WalkMoves->size();
}

int MyAI::Expand(const int* board, const int* pieces, const int p_cnt, vector<int> *Result) 
{
	Result->clear();
	vector<int> eats, walks; 
	for(int i = 0; i < p_cnt; ++i)
	{
		int a = pieces[i];
		if(cannon_index[board[a]])
		{
			CannonMoves(board, a, &eats, &walks);
		}
		else
		{
			Piece_Moves(board, a, &eats, &walks);
		}
	}
	Result->resize(eats.size() + walks.size());
	copy(eats.begin(), eats.end(), Result->begin());
	copy(walks.begin(), walks.end(), Result->begin() + eats.size());
	return Result->size();
}

int MyAI::Piece_Moves(const int* board, const int from_location_no, vector<int> *EatMoves, vector<int> *WalkMoves)
{
	static const int values[14] = {20,75,30,40,50,80,165,20,75,30,40,50,80,165};
	int value = 0;
	int mv_cnt = move_dir_count[from_location_no];
	int from = from_location_no * 100;
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
	return value;
}

// always use my point of view, so use this->Color
double MyAI::Evaluate(const node* board_node)
{
	// total score
	double red_score = 0.0, black_score = 0.0;
	double attack_rate = 1.0;
	// static material values
	// cover and empty are both zero
	static const double values[14] = {20,75,30,40,50,80,165,20,75,30,40,50,80,165};

	vector<int> eat_moves, walk_moves;
	
	for(int i = 0; i < board_node->rbc_cnt[0]; ++i)
	{
		int a = board_node->rbc_pieces[0][i];
		red_score += values[board_node->board[a]]; //子力價值
		red_score += attack_rate * Piece_Moves(board_node->board, a, &eat_moves, &walk_moves);//可以吃到對方的分數
		red_score += position_value[cannon_index[board_node->board[a]]][a];//位置得分
	}

	for(int i = 0; i < board_node->rbc_cnt[1]; ++i)
	{
		int b = board_node->rbc_pieces[1][i];
		black_score += values[board_node->board[b]]; //子力價值
		black_score += Piece_Moves(board_node->board, b, &eat_moves, &walk_moves);//可以吃到對方的分數
		black_score += position_value[cannon_index[board_node->board[b]]][b];//位置得分
	}
	if(this->Color == RED)
	{
		return red_score - black_score;
	}
	return black_score - red_score;
}

double MyAI::MiniF4(node* board_node, double alpha, double beta, int depth)
{
	//cout << "MaxNode :" << id << endl;
	bool color = this->Color;
	vector<int> successor_positions;
	Expand(board_node->board, board_node->rbc_pieces[color], board_node->rbc_cnt[color], &successor_positions);
	int b = successor_positions.size();
	if(b == 0 || depth == 0)
	{
		int val = Evaluate(board_node);
		board_node->value = val;
		return val;
	}
	//begin
	double m = -1 * INF;
	node p1;
	MakeMove(board_node, &p1, successor_positions[0], -1);
	m = max(m, MiniG4(&p1, alpha, beta, depth - 1));
	board_node->PV = successor_positions[0];
	board_node->value = m;

	if(m >= beta)
	{	
		//cout << "betacut: " << id << endl;
		return m;
	}
	for(int i = 1; i < b; ++i)
	{
		node pi;
		MakeMove(board_node, &pi, successor_positions[i], -1);
		double t = MiniG4(&pi, m, m+1, depth - 1);
		if(t > m)//failed high
		{
			//cout << "Node: " << id << " value update by move: " << successor_positions[i] << " subtree id: " << pi << endl;
			board_node->PV = successor_positions[i];
			if(depth < 3 || t >= beta)
			{
				m = t;
			}
			else
			{
				m = MiniG4(&pi, t, beta, depth - 1);
			}
		}
		if(m >= beta)
		{
			//cout << "scout(b)cut: " << id << endl;
			board_node->value = m;
			return m;
		}
	}
	//end
	board_node->value = m;
	return m;
}
 
double MyAI::MiniG4(node* board_node, double alpha, double beta, int depth)
{
	//cout << "MinNode :" << id << endl;
	bool color = !(this->Color);
	vector<int> successor_positions;
	Expand(board_node->board, board_node->rbc_pieces[color], board_node->rbc_cnt[color], &successor_positions);
	int b = successor_positions.size();
	if(b == 0 || depth == 0)
	{
		int val = Evaluate(board_node);
		board_node->value = val;
		return val;
	}
	//begin
	double m = INF;
	node p1;
	MakeMove(board_node, &p1, successor_positions[0], -1);
	//cout << "subtree: " << p1 << " by move: " << successor_positions[0] << endl;
	m = min(m, MiniF4(&p1, alpha, beta, depth - 1));
	board_node->PV = successor_positions[0];
	board_node->value = m;
	if(m <= alpha)
	{
		//cout << "alphacut: " << id << endl;
		return m;
	}
	for(int i = 1; i < b; ++i)
	{
		node pi;
		MakeMove(board_node, &pi, successor_positions[i], -1);
		double t = MiniF4(&pi, m - 1, m, depth - 1);
		if(t < m)//failed low
		{
			//cout << "Node: " << id << " value update by move: " << successor_positions[i] << " subtree id: " << pi << endl;
			board_node->PV = successor_positions[i];
			if(depth < 3 || t <= alpha)
			{
				m = t;
			}
			else
			{
				m = MiniF4(&pi, alpha, t, depth - 1);
			}
		}
		if(m <= alpha)
		{
			//cout << "scout(a)cut: " << id << endl;
			board_node->value = m;
			return m;
		}
	}
	//end
	board_node->value = m;
	return m;
}

double MyAI::NegaScout(node* board_node, double alpha, double beta, int depth)
{
	bool color = (depth&1) ^ (!(this->Color));
	vector<int> successor_positions;
	Expand(board_node->board, board_node->rbc_pieces[color], board_node->rbc_cnt[color], &successor_positions);
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
	return m;
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


