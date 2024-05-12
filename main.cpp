#include "ctime"
#include "tuple"
#include "vector"
#include "cctype"
#include "string"
#include "iostream"
#include "stdlib.h"
#include "math.h"
#include "algorithm"
#define RANDOM_SEED 1
/* the time of determinitic algorithm */
#define RNADOM_ALGORITHM_TIME 50
/* determinitic algorithm of plus x */
#define DETERMINITIC_X 3
/* the time of deterministic algorithm */
#define DETERMINITIC_ALGORITHM_TIME 50
/* the time of strategy0108 algorithm from paper repeated goofspiel: A game of Pure Strategy. */
#define STRATEGY0108_TIME 50
using namespace std;

/* player base class */
class Player{
	/* attributions */
	protected:
		/* record current cards, {<prize card, u out card, opponent out card, u score, opponent score>} */
		vector<tuple<int, int, int, float, float>> current_match_record= {};
		/* current score */
		float current_match_score= 0;
		/* win game count */
		int win_count= 0;
		/* tie game count */
		int tie_count= 0;
		/* total game count */
		int total_count= 0;
		/* flag, card has out of hand */
		vector<bool> card_in_hand_flag= {true, true, true, true, true, true, true, true, true, true, true, true, true};
		
	/* functions */
	public:
		/* construction method */
		Player(){};
		
		/* clear state in one match */
		void clear_current_match_state(){
			this->current_match_record.clear();
			this->current_match_score= 0;
			for(int i= 0; i< 13; i++){this->card_in_hand_flag[i]= true;}
		}
		
		/* show your cards */
		void show_hand_cards(){
			for(int i= 1; i<= 13; i++){
				if(card_in_hand_flag[i- 1]){cout<< i<< " ";}
			}cout<< endl;
		}
		
		/* next cards by strategies without analysis */
		virtual int next_card(int prize_card){return 0;};
		
		/* next cards by strategies with analyze opponent cards */
		virtual int next_card(int prize_card, vector<vector<tuple<int, int, int, float, float>>> history_computer_match_record){return 0;};
		
		/* total count, win count, tie count, lost count */
		tuple<int, int, int, int> get_total_win_tie_lost_count(){
			return make_tuple(this->total_count, this->win_count, this->tie_count, this->total_count- this->win_count- this->tie_count);
		}
		
		/* current match record */
		vector<tuple<int, int, int, float, float>> get_current_match_record(){
			return this->current_match_record;
		}
		
		/* add to current match record */
		void insert_current_round_record(tuple<int, int, int, float, float> round_record){
			this->current_match_record.push_back(round_record);
		}
		
		/* return x is in hand */
		bool is_card_in_hand(int card){
			return this->card_in_hand_flag[card- 1];
		}
		
		/* current score value add x */
		void set_current_match_score_add_x(float x){
			this->current_match_score+= x;
		}
		
		/* get current match score */
		float get_current_match_score(){
			return this->current_match_score;
		}
		
		/* win 1 match */
		void win_count_add1(){
			this->win_count++;
		}
		
		/* tie 1 match */
		void tie_count_add1(){
			this->tie_count++;
		}
		
		/* total count plus 1 */
		void total_count_add1(){
			this->total_count++;
		}
		
		/* clear match state for some rounds */
		void clear_history_match_state(){
			this->win_count= 0;
			this->tie_count= 0;
			this->total_count= 0;
		}		
};

/* computer player */
class ComputerPlayer: public Player{
	
	private:
		/* if you want prize_card, you would out prize_card+ win_gap */
		int win_gap= 3;
		/* check random algorithm */
		vector<int> random_check_time= {50, 100};
		/* opponent is deterministic algorithm? */
		bool is_deterministic_algorithm= true;
		/* opponent is random algorithm */
		bool is_random_algorithm= false;
		/* prize cards has out */
		vector<int> out_prize_cards;
		/* prize cards is out */
		vector<bool> out_prize_card_flags= {false, false, false, false, false, false, false, false, false, false, false, false, false};
		/* cards has out */
		vector<int> out_cards;
		/* opponent cards has out */
		vector<int> out_opponent_cards;
		/* opponent cards has out */
		vector<bool> out_opponent_card_flags= {false, false, false, false, false, false, false, false, false, false, false, false, false};
		/* record opponent gap for judge deterministic alagorithm */
		vector<int> opponent_gap;
		
	public:	
		/* add three strategy plus */
		int next_card(int prize_card, vector<vector<tuple<int, int, int, float, float>>> history_computer_match_record){
			int out_card;
			/* round */
			int round_time= this->current_match_record.size()+ 1;
			/* 
			random player check 
			The opponent's play is random, and for the reward card x, its response will follow a uniform distribution U [1,13]. 
			So we can know that the sample mean it responds to will approach 7 (law of large numbers). 
			*/
			if(count(random_check_time.begin(), random_check_time.end(), history_computer_match_record.size())){
				this->is_random_algorithm= true;
				for(int k= 1; k<= 13; k++){
					int prize_card_random= k;float opponent_card_mean4random_card= 0;
					for(int i= 0; i< history_computer_match_record.size(); i++){
						vector<tuple<int, int, int, float, float>> vec_i= history_computer_match_record[i];
						for(int j= 0; j< 13; j++){
							tuple<int, int, int, float, float> tup_j= vec_i[j];
							if(get<0>(tup_j)== prize_card_random){opponent_card_mean4random_card+= get<2>(tup_j);}
						}
					}
					/* if random, then mean will approach 7 */
					opponent_card_mean4random_card= opponent_card_mean4random_card* 1.0/ history_computer_match_record.size();
					if(opponent_card_mean4random_card< 5|| opponent_card_mean4random_card> 8){this->is_random_algorithm= false;}
				}
			}
			tuple<int, int, int, float, float> round_record;
			/* history data for analysis, */
			if(round_time> 1){
				round_record= this->current_match_record[round_time- 2];
				int prize_card= get<0>(round_record), my_card= get<1>(round_record), opponent_card= get<2>(round_record);
				out_prize_cards.push_back(prize_card);
				out_prize_card_flags[prize_card- 1]= true;
				out_cards.push_back(my_card);
				out_opponent_cards.push_back(opponent_card);
				out_opponent_card_flags[opponent_card- 1]= true;
				/* record opponent positive gap, then we can determine if it is a deterministic algorithm */
				if(opponent_card< prize_card){opponent_gap.push_back(opponent_card+ 13- prize_card);
				}else{opponent_gap.push_back(opponent_card- prize_card);}
				/* adaptive adjust win gap according to opponent last winning gap+ 1 */
				if(prize_card>= 4&& prize_card<= 10&& opponent_card> prize_card){this->win_gap= opponent_card- prize_card+ 1;}
				/* deterministic algorithm check */
				if(round_time>= 4&& is_deterministic_algorithm){
					int x= opponent_gap[0];
					for(int i= 1; i< opponent_gap.size(); i++){
						if(x!= opponent_gap[i]){is_deterministic_algorithm= false;break;}
					}
				}
			}
			/* find min& max cards of you and opponent (note, never out hand) */
			int min_card= 13+ 1, max_card= 0, opponent_min_card= 13+ 1, opponent_max_card= 0, min_prize_card= 13+ 1, max_prize_card= 0;
			for(int i= 0; i< this->card_in_hand_flag.size(); i++){
				if(this->card_in_hand_flag[i]== true){
					if(i+ 1< min_card){min_card= i+ 1;}
					if(i+ 1> max_card){max_card= i+ 1;}
				}
				if(this->out_opponent_card_flags[i]== false){
					if(i+ 1< opponent_min_card){opponent_min_card= i+ 1;}
					if(i+ 1> opponent_max_card){opponent_max_card= i+ 1;}
				}
				if(this->out_prize_card_flags[i]== false){
					if(i+ 1< min_prize_card){min_prize_card= i+ 1;}
					if(i+ 1> max_prize_card){max_prize_card= i+ 1;}
				}
			}
			/* is deterministic algorithm plus x? then we plus x+ 1. */
			if(round_time>= 4&& is_deterministic_algorithm){
				int start= (prize_card+ opponent_gap[0]+ 1- 1)% 13;
				bool flag_end_find= false;				
				for(int i= start; i< 13; i++){
					if(card_in_hand_flag[i]){out_card= i+ 1;flag_end_find= true;break;}
				}
				if(!flag_end_find){
					for(int i= 0; i<= start- 1; i++){
						if(card_in_hand_flag[i]){out_card= i+ 1;break;}
					}
				}
			/* is random algorithm? then we match closest value. */
			}else if(this->is_random_algorithm){
				int i= prize_card, j= prize_card- 1;
				while(true){
					if(i<= 13&& this->card_in_hand_flag[i- 1]){out_card= i;break;}
					if(j>= 1&& this->card_in_hand_flag[j- 1]){out_card= j;break;}
					i++;j--;
				}
			/* 
				small value: {1, 2, 3}; total= 6.
				middle value: {4, 5, 6, 7, 8, 9, 10}; total= 49.
				high value: {11, 12, 13}. total= 36.
				we want get more middle value.
			*/				
			}else{
				/* avoid to learn */
				if(this->current_match_score>= 46.0){
					int x;
					do{
						x= rand()% 13+ 1;
					}while(card_in_hand_flag[x- 1]);
					out_card= x;
				}else if(prize_card<= 3){
					/* smallest prize card, out smallest our's card */
					if(prize_card== 1){
						out_card= min_card;
					}else if(prize_card== 2){
						/* 2, if 2, 3, 4 exist, then sample one from (2, 3, 4), otherwise 1, otherwise min from (5, 6, 7, 8, 9, 10, 11, 12, 13)*/
							bool flag24= this->card_in_hand_flag[1]|| this->card_in_hand_flag[2]|| this->card_in_hand_flag[3];
							while(flag24){
								out_card= rand()% (4- 2+ 1)+ 2;
								if(card_in_hand_flag[out_card- 1]){break;}
							}
							if(flag24== false){out_card= min_card;}
					}else{
						/* 3, sample one from (3, 4, 5), otherwise 1, 2, 6, 7, 8, 9, 10, 11, 12, 13*/
						bool flag35= this->card_in_hand_flag[2]|| this->card_in_hand_flag[3]|| this->card_in_hand_flag[4];
						while(flag35){
							out_card= rand()% (5- 3+ 1)+ 3;
							if(card_in_hand_flag[out_card- 1]){break;}					
						}
						if(flag35== false){out_card= min_card;}
					}
				}else if(prize_card>= 4&& prize_card<= 10){
					/* 4-10, +x operation, x according to opponent last winning gap */
					bool flag_end_find= false;
					int start= min((prize_card+ win_gap- 1)% 13, 13- 1);
					for(int i= start; i< 13; i++){
						if(card_in_hand_flag[i]){out_card= i+ 1;flag_end_find= true;break;}
					}
					if(!flag_end_find){
						for(int i= start- 1; i>= 0; i--){
							if(card_in_hand_flag[i]){out_card= i+ 1;break;}
						}
					}
				}else{
					/* 11, 12, 13 */
					/* in_hand_cards, which cards in our hand */
					vector<int> in_hand_cards;
					for(int i= 0; i< 13; i++){
						if(card_in_hand_flag[i]== true){in_hand_cards.push_back(i+ 1);}
					}
					/* maxest value higher than maxest value of opponent, then take it down otherwise we will abondon */
					if(max_card> opponent_max_card){
						out_card= max_card;
					/* lack of confidence, sample a larger value */
					}else if(opponent_max_card> max_card){
						/* top 25% */
						if(rand()% 2){out_card= in_hand_cards[floor(in_hand_cards.size()/ 4.0)];
						/* top 75% */
						}else{out_card= in_hand_cards[floor(in_hand_cards.size()/ 4.0* 3.0)];}
					/* probility, smallest value, then higher than it */
					/* a little confidence, opponent_max_card== max_card*/
					}else{
						/* out_card= in_hand_cards[floor(in_hand_cards.size()/ 4.0* 3.0)]; */
						if(round_time<= 5){
							if(rand()% 2){out_card= in_hand_cards[floor(in_hand_cards.size()/ 2.0* 1)];
							}else{out_card= min_card;}
						}else{out_card= max_card;}
					}
				}
			}
			/* clear round's state */
			if(round_time== 13){
				for(int i= 0; i< out_prize_card_flags.size(); i++){out_prize_card_flags[i]= false;out_opponent_card_flags[i]= false;}
				this->win_gap= 3;
				out_prize_cards.clear();
				out_cards.clear();
				out_opponent_cards.clear();
			}
			
			/* flag and return */
			this->card_in_hand_flag[out_card- 1]= false;
			return out_card;
		}
		
		/* clear history's state */
		void clear_history_match_state(){
			this->is_deterministic_algorithm= true;
			this->is_random_algorithm= false;
			this->opponent_gap.clear();
			this->win_count= 0;
			this->tie_count= 0;
			this->total_count= 0;
		}
		
};

/* 0108 attempts to predict the opponent��s play for each upcard.
However, recognizing that playing a value greater than the predicted
opponent��s play might fail when playing for high value
cards, 0108 attempts to ��neutralize�� the high value cards and to
focus on winning the seven low value cards by playing value
3. In addition, to limit the opponent��s learning ability, 0108
uses randomization when playing for the cards it does not target
to win. The assumption is that the opponent will mainly play a
card value equal to the exposed spade card value strategy
and will attempt to learn/anticipate 0108��s strategy. */
class Strategy0108Player: public Player{
	
	public:
		/* for card in 4- 10, get it; otherwise, random. */
		int next_card(int prize_card){
			bool flag= true;
			int x;
			if(prize_card>= 4&& prize_card<= 10){x= prize_card+ 3;
			}else{
				while(flag){
					x= rand()% (6- 1+ 1)+ 1;
					if(this->card_in_hand_flag[x- 1]){break;}
				}
			}
			this->card_in_hand_flag[x- 1]= false; 
			return x;
		}
};

/* random strategy player */
class RandomStrategyPlayer: public Player{
	
	public:
		/* implements next_cards function */
		int next_card(int prize_card){
			bool flag= true;
			while(flag){
				int x= rand()% 13+ 1;
				if(this->card_in_hand_flag[x- 1]){
					this->card_in_hand_flag[x- 1]= false;
					return x;
				}
			}
			return 0;
		}
};

/* deterministic strategy player
1. what is deterministic strategy? 
each stage provides a clear card without randomness or probability
2. The deterministic strategy we have chosen?
given x, return x+ 1*/
class DeterministicStrategyPlayer: public Player{
	
	public:
		/* implements next_cards function */
		int next_card(int prize_card){
			int out_card= (prize_card+ DETERMINITIC_X- 1)% 13+ 1;
			this->card_in_hand_flag[out_card- 1]= false;
			return out_card;
		}
};

/* Tool class */
class Tool{
	
	public:
		/* generate unrepeated random integer; range, left to right; num, generation number*/
		vector<int> gen_unrep_randint(int left, int right, int num){
			vector<int> randint_array= {};
			vector<bool> has_gen_flag= {};
			for(int i= 0; i<= right; i++){has_gen_flag.push_back(false);}
			while(num>= 1){
				int x= rand()% (right- left+ 1)+ left;
				if(!has_gen_flag[x]){
					randint_array.push_back(x);
					has_gen_flag[x]= true;
					num--;
				}
			}
			return randint_array;
		}
		
		/* string transpose positive number */
		int string2positive_number(string ipt){
			int x= 0;
			if(ipt.length()== 0|| ipt[0]== '0'){return -1;}
			for(int i= 0; i< ipt.length(); i++){
				if(isdigit(ipt[i])){x= x* 10+ ipt[i]- '0';
				}else{return -1;}
			}
			return x;
		}
};

/* Human player class*/
class HumanPlayer: public Player{
	
	public:
		Tool tool;
		int next_card(int card){
			string ipt;
			int p1_card;
			this->show_hand_cards();
			/* Invalid input judge */
			do{
				cout << "Enter the value of the card to play (1-13): ";
				getline(cin, ipt);
				p1_card= tool.string2positive_number(ipt);
				if(p1_card>= 1&& p1_card<= 13&& this->card_in_hand_flag[p1_card- 1]){
					this->card_in_hand_flag[p1_card- 1]= false;
					break;
				}
				if(p1_card>= 1&& p1_card<= 13){
					cout<< "The card has already been played. Please try again."<< endl;
				}else{
					cout<< "Invalid card. Please try again."<< endl;
				}
			}while(true);
			return p1_card;
		}
};

/* Goofspiel Game class */
class GoofspielGame{
	
	/* Members declared using the protected modifier are not directly accessible outside of the class, 
	but are accessible in derived classes (i.e. classes that inherit from that class). */
	protected:
		Player* player1;
		Player* player2;		
		Tool tool;
		/* match_mode, 0: human vs cp, 1: cp vs random, 2: cp vs deterministic strategy player, 3 cp vs strategy0108 */
		int match_mode;
		vector<int> prize_cards;
		/* data is managed by the platform */
		/* human match record {the format of roundi as {<prize card, p1 out card, p2 out card, p1 score, p2 score>, <prize card, p1 out card, p2 out card, p1 score, p2 score>, ...}} */
		vector<vector<tuple<int, int, int, float, float>>> history_human_match_record= {};
		/* computer match record */
		vector<vector<tuple<int, int, int, float, float>>> history_computer_match_record= {};
		/* flag, prize cards in referee's hand */
		vector<bool> unplayed_prize_card_flags= {true, true, true, true, true, true, true, true, true, true, true, true, true};
			
	public:
		/* construct method */
		GoofspielGame(){}
		
		/* provide match record for analysis */
		vector<vector<tuple<int, int, int, float, float>>> get_computer_match_record(){
			return this->history_computer_match_record;
		}

		/* provide human record for analysis */
		vector<vector<tuple<int, int, int, float, float>>> get_human_match_record(){
			return this->history_human_match_record;
		}
		
		/* receive prize card */
		int get_prize_card(int round){
			unplayed_prize_card_flags[round]= false;
			return prize_cards[round];
		}
		
		/* initize random prize cards */
		void init_prize_cards(){
			prize_cards= tool.gen_unrep_randint(1, 13, 13);
		}
		
		/* ensure match mode and players */ 
		void init_match(Player* p1, Player* p2, int mode){
			player1= p1;
			player2= p2;
			/* ensure battle mode */
			if(mode== 1){this->match_mode= 0;
			}else if(mode== 3){this->match_mode= 1;
			}else if(mode== 4){this->match_mode= 2;
			}else{this->match_mode= 3;}
		}
		
		/* output prize card of referee's hand */
		void show_prize_cards(){
			for(int i= 0; i< this->unplayed_prize_card_flags.size(); i++){
				if(this->unplayed_prize_card_flags[i]){cout<< this->prize_cards[i]<< " ";}
			}cout<<endl;
		}
		
		/* clear computer history result */
		void clear_computer_history_result(){
			history_computer_match_record.clear();
		}
		
		/* start competition */
		void play(){
			int p1_card, p2_card;
			/* human vs cp */
			if(match_mode== 0){
				/* init prize cards*/
				init_prize_cards(); 
				for(int i= 0; i< 13; i++){	
					int prize_card= this->get_prize_card(i);
					cout<< "------------- Round "<< i+ 1<< " -------------\n";
					cout<< "Prize card: " << prize_card << "\n";
					cout << "Your hand: ";
					p1_card= player1->next_card(prize_card);
					p2_card= player2->next_card(prize_card, this->history_computer_match_record);
					cout<< "You played: "<< p1_card<< "\n";
					cout<< "Computer played: "<< p2_card<< "\n";
					/* show scores */
					if(p1_card> p2_card){
						player1->set_current_match_score_add_x(prize_card);
						cout<< "You win the round!\n";
					}else if(p1_card< p2_card){
						player2->set_current_match_score_add_x(prize_card);
						cout<< "Computer wins the round!\n";
					}else{
						player1->set_current_match_score_add_x(prize_card/ 2.0);
						player2->set_current_match_score_add_x(prize_card/ 2.0);
						cout<< "The round is a tie! Both players receive "<< prize_card/ 2.0<< " points.\n";
					}
					/* Show one decimal place for floating point numbers */
					cout<< "	Current scores	\n";
					cout<< "You: "<< player1->get_current_match_score()<< "\n";
					cout<< "Computer: "<< player2->get_current_match_score()<< "\n\n";
					/* save record */
					player1->insert_current_round_record(make_tuple(prize_card, p1_card, p2_card, player1->get_current_match_score(), player2->get_current_match_score()));
					player2->insert_current_round_record(make_tuple(prize_card, p2_card, p1_card, player2->get_current_match_score(), player1->get_current_match_score()));
				}
				//get_total_win_tie_lost_count
				/* game over */
				player1->total_count_add1();
				player2->total_count_add1();
				int player_score_gap= player1->get_current_match_score()- player2->get_current_match_score();
				if(player_score_gap> 0){
					player1->win_count_add1();
				}else if(player_score_gap== 0){
					player1->tie_count_add1();
					player2->tie_count_add1();
				}else{player2->win_count_add1();}
				/* output game result */
				cout<< "------------- Game Over -------------\n";
				cout<< "Final scores:\n";
				cout<< "You: "<< player1->get_current_match_score()<< "\n";
				cout<< "Computer: "<< player2->get_current_match_score()<< "\n";
				if(player_score_gap> 0){
					cout<< "You win the game!\n";
				}else if(player_score_gap< 0) {
					cout<< "Computer wins the game!\n";
				}else{
					cout<< "The game is a tie!\n";
				}
				/* save current match record to sever */
				this->history_human_match_record.push_back(player1->get_current_match_record());
				this->history_computer_match_record.push_back(player2->get_current_match_record());
				/* clear current match state */
				this->player1->clear_current_match_state();
				this->player2->clear_current_match_state();				
			}else{
				/* for recycling */
				int match_time;
				string p1_name= "Computer";
				string p2_name;
				if(match_mode== 1){
					match_time= RNADOM_ALGORITHM_TIME;
					p2_name= "Random strategy";
				}else if(match_mode== 2){
					match_time= DETERMINITIC_ALGORITHM_TIME;
					p2_name= "Deterministic strategy";
				}else{
					match_time= STRATEGY0108_TIME;
					p2_name="0108 strategy";}
				/* start */
				for(int i= 0; i< match_time; i++){
					init_prize_cards();
					cout << "Game " << (i + 1) << ":\n";
					for(int j= 0; j< 13; j++){
						int prize_card= this->get_prize_card(j);
						p1_card= this->player1->next_card(prize_card, this->history_computer_match_record);
						p2_card= this->player2->next_card(prize_card);
						cout<< "  Round "<< j+ 1<< ": Prize card "<< prize_card<< " | "<< p1_name<< " played "<< p1_card<< " | "<< p2_name<< " played "<< p2_card;
						if(p1_card> p2_card){
							player1->set_current_match_score_add_x(prize_card);
							cout<< " | "<< p1_name<< " wins the round!\n";
						}else if(p1_card< p2_card){
							player2->set_current_match_score_add_x(prize_card);
							cout<< " | "<< p2_name<< " wins the round!\n";
						}else{
							player1->set_current_match_score_add_x(prize_card/ 2.0);
							player2->set_current_match_score_add_x(prize_card/ 2.0);
							cout<< " | It's a tie. Both players receive "<< prize_card/ 2.0 << " points\n";
						}
						cout<< "Current scores - "<< p1_name<< ": " <<player1->get_current_match_score() << ", "<< p2_name<< ": "<< player2->get_current_match_score()<< "\n\n";
						/* save round record */
						player1->insert_current_round_record(make_tuple(prize_card, p1_card, p2_card, player1->get_current_match_score(), player2->get_current_match_score()));
					}
					/* update count value */
					int player1_final_score, player2_final_score;
					player1_final_score= player1->get_current_match_score();
					player2_final_score= player2->get_current_match_score();
					cout<< "Result for game "<< i+1<< ": "<< p1_name<< " Player Score: "<< player1_final_score<< ", "<< p2_name<< " Player Score: "<< player2_final_score<< endl<< endl;
					player1->total_count_add1();
					player2->total_count_add1();
					if(player1_final_score> player2_final_score){
						player1->win_count_add1();
					}else if(player1_final_score< player2_final_score){
						player2->win_count_add1();
					}else{
						player1->tie_count_add1();
						player2->tie_count_add1();
					}
					/* record score */					
					this->history_computer_match_record.push_back(player1->get_current_match_record());
					/* clear current match state */
					this->player1->clear_current_match_state();
					this->player2->clear_current_match_state();
				}
				tuple<int, int, int, int> player1_match_condition= player1->get_total_win_tie_lost_count();
				tuple<int, int, int, int> player2_match_condition= player2->get_total_win_tie_lost_count();
				// Print the final summary after all games
				cout<< "After "<< match_time<< " games:"<< endl;
				cout<< p1_name<< " Player Wins: "<< get<1>(player1_match_condition)<< endl;
				cout<< p2_name<< " Player Wins: "<< get<1>(player2_match_condition)<< endl;
				cout<< "Ties: "<< get<2>(player1_match_condition)<< endl;
			}
		}
		
		/* display human game achievements */
		void display_human_game_achievements(){
			int win_count= 0, die_count= 0, lost_count= 0, total_count= 0;
			vector<float> human_scores, computer_scores;
			for(int i= 0; i< this->history_human_match_record.size(); i++){
				vector<tuple<int, int, int, float, float>> vec_i= this->history_human_match_record[i];
				tuple<int, int, int, float, float> current_result_tuple= vec_i[12];
				int gap= get<3>(current_result_tuple)- get<4>(current_result_tuple);
				if(gap> 0){win_count++;
				}else if(gap< 0){lost_count++;
				}else{die_count++;}
				total_count++;
				human_scores.push_back(get<3>(current_result_tuple));
				computer_scores.push_back(get<4>(current_result_tuple));
			}
			cout<< "	Game results	\n";
			cout<< "Total of "<< total_count<< " games played.\n";
			cout<< "Human player won "<< win_count<< " times.\n";
			cout<< "Computer player won "<< lost_count<< " times.\n";
			cout<< "There were "<< die_count<< " ties.\n";
			for(int i= 0; i< human_scores.size(); i++){
				cout<< "Game "<< i+ 1<< ": Human player score "<< human_scores[i]<< ", Computer player score "<< computer_scores[i]<< "\n";
			}
		}
};



int main(){
	
	/* set random seed */
	srand(time(NULL));
	string ipt;
	int choice;
	bool has_play= false;
	/* define entity */
	GoofspielGame game;
	HumanPlayer hp;
	ComputerPlayer cp;
	DeterministicStrategyPlayer dsp;
	RandomStrategyPlayer rsp;
	Strategy0108Player s0108p;
	cout << "========================================\n";
	cout << "          Welcome to Goofspiel!         \n";
	cout << "========================================\n\n";		
	/* start */
	do {
		cout << "\nPlease select an option:\n";
		cout << " 1 - Play a normal game\n";
		cout << " 2 - Display the results of humans vs computers\n";
		cout << " 3 - Test against random strategy\n";
		cout << " 4 - Test against deterministic strategy\n";
		cout << " 5 - Test against strategy0108\n";
		cout << " 6 - Exit\n";
		cout << "Your choice: ";
		
		getline(cin, ipt);
		if(!(ipt.length()== 1&& isdigit(ipt[0]))){choice= 0;
		}else{choice= ipt[0]- '0';}
		
		switch(choice){
			/* human vs computer */
			case 1:{
				while(true){
					/* player entry */
					game.init_match(&hp, &cp, 1);
					/* start competition */
					game.play();
					/* computer player clear current state */
					cp.clear_history_match_state();
					/* setting flag */
					has_play= true;
					/* clear cin buffer */
					string ans;
					do{
						cout<< "Do you want to play again? (y/n): ";
						getline(cin, ans);
						if(ans.length()== 1&& (ans[0]== 'y'|| ans[0]== 'n')){break;}
					}while(true);
					if(ans[0]== 'n'){break;}
				}
				/* clear computer history match record */
				game.clear_computer_history_result();
				break;
			}
			
			/* display_game_achievements huamn history's record */
			case 2:{
				if(has_play== false){
					cout<< "No games have been played yet."<< endl;
				}else{game.display_human_game_achievements();}
				break;
			}
			
			/* computer vs random */
			case 3:{
				game.init_match(&cp, &rsp, 3);
				game.play();
				game.clear_computer_history_result(); 
				cp.clear_current_match_state();
				cp.clear_history_match_state();
				rsp.clear_current_match_state();
				rsp.clear_history_match_state();
				break;
			}
			
			/* computer vs determinitic */
			case 4:{
				game.init_match(&cp, &dsp, 4);
				game.play();
				game.clear_computer_history_result(); 
				cp.clear_history_match_state();
				cp.clear_current_match_state();
				dsp.clear_history_match_state();
				dsp.clear_current_match_state();
				break;
			} 
			
			case 5:{
				// Strategy0108_TIME
				game.init_match(&cp, &s0108p, 5);
				game.play();
				game.clear_computer_history_result(); 
				cp.clear_history_match_state();
				cp.clear_current_match_state();
				s0108p.clear_current_match_state();
				s0108p.clear_history_match_state();
				break;
			}
			
			/* exit */
			case 6:{
				cout << "Exiting the game. goodbye!"<< endl;
				return 0;
			}
			
			/* deal with invalid option */
			default:{
				cout << "\nInvalid option. please enter a valid choice."<< endl;
				break;
			}
		}
	}while(choice!= 6);
}

