#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "libjacktrump.h"

TRUMP_GAME* trump_jack_create_game(bool player_first){
	TRUMP_GAME* new_game = malloc(sizeof(TRUMP_GAME));

	for(int i=0;i<CARD_POOL_SIZE;i++){
		new_game->card_pool[i] = true;
	}
	for(int i=0;i<PLAYER_CARD_POOL_SIZE;i++){
		new_game->player_card[i] = 0;
		new_game->cpu_card[i] = 0;
	}

	trump_jack_player_hit(new_game);
	trump_jack_cpu_hit(new_game);

	new_game->is_player_first = player_first;
	new_game->is_player_turn = player_first;
	new_game->status = CONTINUE;

	new_game->is_player_stay = false;
	new_game->is_cpu_stay = false;

	return new_game;
}

void trump_jack_destroy_game(TRUMP_GAME* game){
	free(game);
}

int trump_jack_player_round(TRUMP_GAME* game, int player_action, void* trump){
	if(!game->is_player_first){
		if(game->is_player_stay && game->is_cpu_stay){
			return END;
		}
	}

	switch(player_action){
		case STAY:
			trump_jack_player_stay(game);
			break;
		case HIT:
			trump_jack_player_hit(game);
			return STAY;
			break;
		default:break;
	}
}

int trump_jack_cpu_round(TRUMP_GAME* game){
	int player_point = trump_jack_get_masked_player_total_points(game);
	int cpu_point = trump_jack_get_cpu_total_points(game);

	if(player_point > 18 || cpu_point > 18 || (cpu_point - player_point) > 5){
		trump_jack_cpu_stay(game);

		if(game->is_player_first){
			if(game->is_player_stay && game->is_cpu_stay){
				return END;
			}
		}

		return STAY;
	}else{
		trump_jack_cpu_hit(game);
		return HIT;
	}
}

int trump_jack_player_hit(TRUMP_GAME *game){
	game->is_player_stay = false;

	int random_index = rand() % CARD_POOL_SIZE;
	while(!game->card_pool[random_index]){
		random_index = rand() % CARD_POOL_SIZE;
	}
	game->card_pool[random_index] = false;

	int card_index = 0;
	while(game->player_card[card_index]){
		card_index++;
	};
	random_index++;
	game->player_card[card_index] = random_index;

	return random_index;
}

void trump_jack_player_stay(TRUMP_GAME *game){
	game->is_player_stay = true;
}

int trump_jack_cpu_hit(TRUMP_GAME *game){
	game->is_cpu_stay = false;

	int random_index = rand() % CARD_POOL_SIZE;
	while(!game->card_pool[random_index]){
		random_index = rand() % CARD_POOL_SIZE;
	}
	game->card_pool[random_index] = false;

	int card_index = 0;
	while(game->cpu_card[card_index]){
		card_index++;
	};
	random_index++;
	game->cpu_card[card_index] = random_index;

	return random_index;
}

void trump_jack_cpu_stay(TRUMP_GAME *game){
	game->is_cpu_stay = true;
}

int trump_jack_get_player_total_points(TRUMP_GAME *game){
	int total_points = 0;
	for(int i=0;i<PLAYER_CARD_POOL_SIZE;i++){
		total_points += game->player_card[i];
	}

	return total_points;
}

int trump_jack_get_cpu_total_points(TRUMP_GAME *game){
	int total_points = 0;
	for(int i=0;i<PLAYER_CARD_POOL_SIZE;i++){
		total_points += game->cpu_card[i];
	}

	return total_points;
}

int trump_jack_get_masked_player_total_points(TRUMP_GAME *game){
	int total_points = 0;
	for(int i=1;i<PLAYER_CARD_POOL_SIZE;i++){
		total_points += game->player_card[i];
	}

	return total_points;
}

int trump_jack_check_result(TRUMP_GAME *game){
	int player_point = trump_jack_get_player_total_points(game);
	int cpu_point = trump_jack_get_cpu_total_points(game);

	if(player_point > 21)player_point = -1;
	if(cpu_point > 21)cpu_point = -1;

	if(player_point < cpu_point){
		return TRUMP_JACK_RESULT_LOSE;
	}else if(player_point > cpu_point){
		return TRUMP_JACK_RESULT_WIN;
	}else{
		return TRUMP_JACK_RESULT_TIED;
	}
}
