#ifndef LIBJACKTRUMP_H
#define LIBJACKTRUMP_H

#define CARD_POOL_SIZE 11
#define PLAYER_CARD_POOL_SIZE 10

#include <stdbool.h>

//const int trump_jack_card_pool_size = CARD_POOL_SIZE;
//const int trump_jack_player_card_pool_size = PLAYER_CARD_POOL_SIZE;

enum trump_jack_game_status {CONTINUE, CHECK};
enum trump_jack_player_action {STAY, HIT, END};
enum trump_jack_game_result {TRUMP_JACK_RESULT_LOSE, TRUMP_JACK_RESULT_TIED, TRUMP_JACK_RESULT_WIN};

typedef struct {
	bool card_pool[CARD_POOL_SIZE];
	unsigned short player_card[PLAYER_CARD_POOL_SIZE];
	unsigned short cpu_card[PLAYER_CARD_POOL_SIZE];
	bool is_player_first;
	bool is_player_turn;
	enum trump_jack_game_status status;
	bool is_player_stay;
	bool is_cpu_stay;
} TRUMP_GAME;

TRUMP_GAME* trump_jack_create_game(bool player_first);
void trump_jack_destroy_game(TRUMP_GAME *game);
int trump_jack_player_round(TRUMP_GAME *game, int player_action, void *trump);
int trump_jack_cpu_round(TRUMP_GAME *game);
int trump_jack_player_hit(TRUMP_GAME *game);
void trump_jack_player_stay(TRUMP_GAME *game);
int trump_jack_cpu_hit(TRUMP_GAME *game);
void trump_jack_cpu_stay(TRUMP_GAME *game);

int trump_jack_get_player_total_points(TRUMP_GAME *game);
int trump_jack_get_cpu_total_points(TRUMP_GAME *game);
int trump_jack_get_masked_player_total_points(TRUMP_GAME *game);

int trump_jack_check_result(TRUMP_GAME *game);

#endif
