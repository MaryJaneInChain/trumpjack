#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <libintl.h>
#include <locale.h>
#include <gtk/gtk.h>
#include <gio/gio.h>

#include "libjacktrump.h"

typedef struct {
	char *game_title;
	TRUMP_GAME *game;
	GdkPixbuf **cards_pixbuf;

	GtkWindow *main_window;

	GtkWidget *upside_grid;
	GtkWidget *downside_grid;

	GtkWidget *main_box;
	GtkWidget *upside_box;
	GtkWidget *downside_box;
	GtkWidget *cpu_card_pool_box;
	GtkWidget *cpu_status_box;
	GtkWidget *label_box;
	GtkWidget *player_card_pool_box;
	GtkWidget *player_button_box;

	GtkWidget *cpu_status_box_frame;
	GtkWidget *player_button_box_frame;

	GtkWidget *hit_button;
	GtkWidget *stay_button;

	GtkWidget *cpu_status_label;
	GtkWidget *cpu_points_label;
	GtkWidget *player_points_label;
} GAME;

const char *identifier = "com.github.maryjaneinchain.blackjack";

const char *CPU_SPEAKING_START = "So let`s start...";
const char *CPU_SPEAKING_THINKING = "Just give me a second...";
const char *CPU_SPEAKING_HIT = "Hit.";
const char *CPU_SPEAKING_STAY = "Gonna stay.";
const char *CPU_SPEAKING_OUTOF21 = "Oh shit, I`m fucked!";
const char *CPU_SPEAKING_WIN = "I win!";
const char *CPU_SPEAKING_LOSE = "Well done my friend...";
const char *CPU_SPEAKING_TIE = "How about one more?";

const char *DIALOG_LABEL_WIN = "Congratulations, you win!";
const char *DIALOG_LABEL_TIE = "You tied!";
const char *DIALOG_LABEL_LOSE = "Whoops, you lose!";

GdkPixbuf **cards_pixbuf;
GtkApplication *app;

static GAME *main_game;
static GtkWindow *main_window;

int main(int argc, char **argv);

GAME *init_game(char* title);

static void activate(GtkApplication *app, gpointer data);

void create_main_window(GtkApplication *app, GAME *game);
void create_main_window_header_bar(GtkWindow *main_window, GAME *game);
void create_main_window_menu_button(GtkHeaderBar *header_bar, GAME *game);
void create_main_window_menu(GtkMenuButton *menu_button, GAME *game);

static void activate_new(GSimpleAction *action, GVariant *parameter, gpointer data);
static void activate_about(GSimpleAction *action, GVariant *parameter, gpointer data);
static void activate_quit(GSimpleAction *action, GVariant *parameter, gpointer data);

static gboolean draw_background(GtkWidget *widget, cairo_t *cr, gpointer data);
static void change_size(GtkWidget *widget, GdkRectangle *allocation, gpointer user_data);

static GtkWidget* initAbout();
static void activate(GtkApplication *app, gpointer user_data);

void restart_game(GAME *game);
static void update_player_card_pool(GAME *game);
static void update_cpu_card_pool(GAME *game, bool is_result);
void clean_card_pool(GtkBox *box);
static void clean_card_pool_callback(GtkWidget *widget, gpointer data);
void enable_player_moves(GAME *game);
void disable_player_moves(GAME *game);
static void player_hit(GtkButton *button, gpointer data);
static void player_stay(GtkButton *button, gpointer data);
gboolean cpu_turn(GAME *game);
void game_over(GAME *game);
void show_game_over_dialog(GAME *game, int game_result);

static TRUMP_GAME* new_game(GtkBox* main_box, bool is_player_first);

#endif
