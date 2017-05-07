#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <libintl.h>
#include <locale.h>
#include <gtk/gtk.h>
#include <gio/gio.h>

#include "main.h"
#include "libjacktrump.h"
#include "card.h"

#define _(STRING) gettext(STRING)

const GActionEntry app_entries[] = {
	{"new", activate_new, NULL, NULL, NULL},
	{"about", activate_about, NULL, NULL, NULL},
	{"quit", activate_quit, NULL, NULL, NULL}
};

int main(int argc, char **argv){
	srand((int)time(0));
	
	int status;

	app = gtk_application_new(identifier, G_APPLICATION_FLAGS_NONE);
	g_action_map_add_action_entries(G_ACTION_MAP(app), app_entries, G_N_ELEMENTS (app_entries), app);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	return status;
}

GAME *init_game(char* title){
	GAME *game = malloc(sizeof(GAME));

	game->game_title = title;
	game->cards_pixbuf = init_card();

	return game;
}

static void activate(GtkApplication *app, gpointer data){
	GAME* game = init_game("Trump Jack");

	create_main_window(app, game);
	main_game = game;

	update_player_card_pool(game);
	update_cpu_card_pool(game, false);
}

void restart_game(GAME* game){
	game->game = trump_jack_create_game(true);

	gtk_label_set_text(game->cpu_status_label, CPU_SPEAKING_START);
	enable_player_moves(game);

	update_player_card_pool(game);
	update_cpu_card_pool(game, false);
}

void create_main_window(GtkApplication *app, GAME *game){
	game->main_window = GTK_WINDOW(gtk_application_window_new(app));

	main_window = game->main_window;
	
	game->game = trump_jack_create_game(true);

	game->main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_container_set_border_width(game->main_box, 20);

	game->upside_grid = gtk_grid_new();
	gtk_grid_set_column_homogeneous(game->upside_grid, true);
	game->downside_grid = gtk_grid_new();
	gtk_grid_set_column_homogeneous(game->downside_grid, true);

	game->cpu_card_pool_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	game->label_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	game->player_card_pool_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

	game->cpu_status_box_frame = gtk_frame_new("CPU Status");
	gtk_frame_set_label_align(game->cpu_status_box_frame, 0.5, 0.5);

	game->cpu_status_label = gtk_label_new(CPU_SPEAKING_START);
	gtk_label_set_line_wrap(game->cpu_status_label, true);

	game->cpu_status_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_set_border_width(game->cpu_status_box, 10);
	gtk_container_add(game->cpu_status_box, game->cpu_status_label);
	gtk_container_add(game->cpu_status_box_frame, game->cpu_status_box);
	
	game->player_button_box_frame = gtk_frame_new("Player Moves");
	gtk_frame_set_label_align(game->player_button_box_frame, 0.5, 0.5);

	game->hit_button = gtk_button_new_with_label("Hit");
	game->stay_button = gtk_button_new_with_label("Stay");

	g_signal_connect(game->hit_button, "clicked", G_CALLBACK(player_hit), game);
	g_signal_connect(game->stay_button, "clicked", G_CALLBACK(player_stay), game);
	
	game->player_button_box = gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
	gtk_container_set_border_width(game->player_button_box, 10);
	gtk_button_box_set_layout(game->player_button_box, GTK_BUTTONBOX_SPREAD);

	gtk_container_add(game->player_button_box, game->hit_button);
	gtk_container_add(game->player_button_box, game->stay_button);
	gtk_container_add(game->player_button_box_frame, game->player_button_box);

	game->cpu_points_label = gtk_label_new("");
	game->player_points_label = gtk_label_new("");

	gtk_grid_attach(game->upside_grid, game->cpu_status_box_frame, 6, 0, 1, 1);
	gtk_grid_attach(game->downside_grid, game->player_button_box_frame, 6, 0, 1, 1);

	gtk_box_pack_start(game->label_box, game->cpu_points_label, true, true, 2);
	gtk_box_pack_end(game->label_box, game->player_points_label, true, true, 2);

	gtk_grid_attach(game->upside_grid, game->cpu_card_pool_box, 0, 0, 6, 1);
	gtk_grid_attach(game->downside_grid, game->player_card_pool_box, 0, 0, 6, 1);

	gtk_box_pack_start(game->main_box, game->upside_grid, true, true, 10);
	gtk_box_pack_start(game->main_box, game->label_box, true, true, 5);
	gtk_box_pack_start(game->main_box, game->downside_grid, true, true, 10);
	gtk_container_add(game->main_window, game->main_box);

	gtk_window_set_title(game->main_window, game->game_title);
	gtk_widget_set_size_request(GTK_WIDGET(game->main_window), 700, 400);
	gtk_window_set_position(game->main_window, GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(game->main_window, 5);

	create_main_window_header_bar(game->main_window, game);

	//g_signal_connect(grid, "draw", G_CALLBACK (draw_background), NULL);
	//g_signal_connect(main_box, "draw", G_CALLBACK (draw_background), NULL);

	gtk_widget_show_all(GTK_WIDGET(game->main_window));
}

void create_main_window_header_bar(GtkWindow* main_window, GAME *game){
	GtkWidget *header_bar;
	GtkMenuButton *menu_button;

	header_bar = gtk_header_bar_new();
	gtk_header_bar_set_title(GTK_HEADER_BAR(header_bar), game->game_title);
	gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header_bar), TRUE);
	
	create_main_window_menu_button(GTK_HEADER_BAR(header_bar), game);

	gtk_window_set_titlebar(main_window, header_bar);
}

void create_main_window_menu_button(GtkHeaderBar* header_bar, GAME *game){
	GtkWidget *button_image;
	GtkWidget *button;

	button_image = gtk_image_new_from_icon_name("start-here", 1);
	button = gtk_menu_button_new();
	gtk_button_set_image(GTK_BUTTON(button), button_image);
	gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);

	create_main_window_menu(button, game);

	gtk_header_bar_pack_start(header_bar, GTK_WIDGET(button));
}

void create_main_window_menu(GtkMenuButton *menu_button, GAME *game){
	GMenu *main_menu;
	GMenu *sub_top_menu;
	GMenu *sub_middle_menu;
	GMenu *sub_bottom_menu;
	GtkWidget *menu_popover;
	GMenuItem *new_game_button;
	GMenuItem *about_button;
	GSimpleAction *new;
	GSimpleAction *about;

	main_menu = g_menu_new();
	sub_top_menu = g_menu_new();
	sub_middle_menu = g_menu_new();
	sub_bottom_menu = g_menu_new();
	new_game_button = g_menu_item_new("New Game", NULL);
	about_button = g_menu_item_new("About", NULL);

	new = g_simple_action_new("new", NULL);
	g_signal_connect(new, "activate", G_CALLBACK(activate_new), NULL);
	g_menu_item_set_action_and_target_value(new_game_button, new, NULL);

	about = g_simple_action_new("about", NULL);
	g_signal_connect(about, "activate", G_CALLBACK(activate_about), NULL);
	g_menu_item_set_action_and_target_value(about_button, about, NULL);

	g_menu_append(sub_top_menu, "_New Game", "app.new");
	g_menu_append(sub_middle_menu, "_Setting", "app.setting");
	g_menu_append(sub_bottom_menu, "_About", "app.about");
	g_menu_append(sub_bottom_menu, "_Quit", "app.quit");
	g_menu_append_section(main_menu, NULL, sub_top_menu);
	g_menu_append_section(main_menu, NULL, sub_middle_menu);
	g_menu_append_section(main_menu, NULL, sub_bottom_menu);

	menu_popover = gtk_popover_new_from_model(GTK_WIDGET(menu_button), main_menu);

	gtk_menu_button_set_use_popover(menu_button, TRUE);
	gtk_menu_button_set_popover(menu_button, menu_popover);
}

static void activate_new(GSimpleAction *action, GVariant *parameter, gpointer data){
	GAME *game = (GAME*)data;

	restart_game(main_game);
}

static void activate_about(GSimpleAction *action, GVariant *parameter, gpointer data){
	GtkWindow *window = main_window;

	const gchar *authors[] = {"Zhang Chi", NULL};
	const gchar *documenters[] = {NULL};

	gtk_show_about_dialog(window,
					"program-name", "Trump Jack",
					"version", "0.0.1",
					"copyright", "(C) 2017 Zhang Chi",
					"license-type", GTK_LICENSE_GPL_3_0,
					"website", "https://github.com/MaryJaneInChain",
					"comments", "Totally bullshit.",
					"authors", authors,
					"documenters", NULL,
					"logo-icon-name", "start-here",
					"title", "About Trump Jack",
					NULL);

	return;
}

static void activate_quit(GSimpleAction *action, GVariant *parameter, gpointer data){
	GApplication *app = data;

	g_application_quit(app);
}

static void update_player_card_pool(GAME *game){
	clean_card_pool(game->player_card_pool_box);

	int index = 0;
	unsigned short *card_pool = game->game->player_card;
	unsigned short card_point = card_pool[index];
	int total_points = 0;
	while(card_point > 0){
		total_points += card_point;
		GtkWidget* card;
		card = gtk_image_new_from_pixbuf(game->cards_pixbuf[card_point-1]);
		gtk_box_pack_start(game->player_card_pool_box, card, FALSE, FALSE, 5);
		card_point = card_pool[++index];
		gtk_widget_show(card);
	}

	char points_string[16];
	if(total_points < 22){
		sprintf(points_string, "Player: %2d / 21", total_points);
	}else{
		sprintf(points_string, "<b>Player: %2d / 21</b>", total_points);
	}
	gtk_label_set_markup(game->player_points_label, points_string);
}

static void update_cpu_card_pool(GAME *game, bool is_result){
	clean_card_pool(game->cpu_card_pool_box);

	int index = 0;

	if(!is_result){
		GtkWidget* hidden_card = gtk_image_new_from_pixbuf(game->cards_pixbuf[11]);
		gtk_box_pack_start(game->cpu_card_pool_box, hidden_card, false, false, 5);
		gtk_widget_show(hidden_card);
		index = 1;
	}

	unsigned short *card_pool = game->game->cpu_card;
	unsigned short card_point = card_pool[index];
	int total_points = 0;
	while(card_point > 0){
		total_points += card_point;
		GtkWidget* card;
		card = gtk_image_new_from_pixbuf(game->cards_pixbuf[card_point-1]);
		gtk_box_pack_start(game->cpu_card_pool_box, card, false, false, 5);
		card_point = card_pool[++index];
		gtk_widget_show(card);
	}

	char points_string[17];
	if(is_result){
		if(total_points < 22){
			sprintf(points_string, "CPU: %2d / 21", total_points);
		}else{
			sprintf(points_string, "<b>CPU: %2d / 21</b>", total_points);
			gtk_label_set_text(GTK_LABEL(game->cpu_status_label), CPU_SPEAKING_OUTOF21);
		}
	}else{
		if(total_points < 21){
			sprintf(points_string, "CPU: %2d + ? / 21", total_points);
		}else{
			sprintf(points_string, "<b>CPU: %2d + ? / 21</b>", total_points);
			gtk_label_set_text(GTK_LABEL(game->cpu_status_label), CPU_SPEAKING_OUTOF21);
		}
	}

	gtk_label_set_markup(game->cpu_points_label, points_string);
}

void clean_card_pool(GtkBox *box){
	gtk_container_foreach(GTK_CONTAINER(box), clean_card_pool_callback, NULL);
}

static clean_card_pool_callback(GtkWidget *widget, gpointer data){
	gtk_widget_destroy(widget);
}

void enable_player_moves(GAME *game){
	if(trump_jack_get_player_total_points(game->game) <= 21){
		gtk_widget_set_sensitive(game->hit_button, true);
	}
	gtk_widget_set_sensitive(game->stay_button, true);
}

void disable_player_moves(GAME *game){
	gtk_widget_set_sensitive(game->hit_button, false);
	gtk_widget_set_sensitive(game->stay_button, false);
}

static void player_hit(GtkButton *button, gpointer data){
	GAME *game = (GAME*)data;

	trump_jack_player_round(game->game, HIT, NULL);
	gtk_label_set_text(GTK_LABEL(game->cpu_status_label), CPU_SPEAKING_THINKING);
	update_player_card_pool(game);
	disable_player_moves(game);

	g_timeout_add(1000, cpu_turn, game);
}

static void player_stay(GtkButton *button, gpointer data){
	GAME *game = (GAME*)data;

	trump_jack_player_round(game->game, STAY, NULL);
	gtk_label_set_text(GTK_LABEL(game->cpu_status_label), CPU_SPEAKING_THINKING);
	update_player_card_pool(game);
	disable_player_moves(game);

	g_timeout_add(1000, cpu_turn, game);
}

gboolean cpu_turn(GAME *game){
	int action = trump_jack_cpu_round(game->game);
	switch(action){
		case STAY:
			gtk_label_set_text(GTK_LABEL(game->cpu_status_label), CPU_SPEAKING_STAY);
			break;
		case HIT:
			gtk_label_set_text(GTK_LABEL(game->cpu_status_label), CPU_SPEAKING_HIT);
			break;
		case END:
			game_over(game);
			return false;
			break;
	}
	update_cpu_card_pool(game, false);
	enable_player_moves(game);

	//g_print("player_stay: %d\ncpu stay: %d\n", game->game->is_player_stay, game->game->is_cpu_stay);

	return false;
}

void game_over(GAME *game){
	update_cpu_card_pool(game, true);
	disable_player_moves(game);

	int result = trump_jack_check_result(game->game);

	switch(result){
		case TRUMP_JACK_RESULT_LOSE:
			gtk_label_set_text(GTK_LABEL(game->cpu_status_label), CPU_SPEAKING_WIN);
			break;
		case TRUMP_JACK_RESULT_TIED:
			gtk_label_set_text(GTK_LABEL(game->cpu_status_label), CPU_SPEAKING_TIE);
			break;
		case TRUMP_JACK_RESULT_WIN:
			gtk_label_set_text(GTK_LABEL(game->cpu_status_label), CPU_SPEAKING_LOSE);
			break;
	}
	//g_print("Result: %d", result);
	show_game_over_dialog(game, result);
}

void show_game_over_dialog(GAME *game, int game_result){
	GtkWidget *dialog;

	GtkWidget *new_game_button;
	GtkWidget *cancel_button;

	GtkWidget *new_game_button_image;
	GtkWidget *cancel_button_image;
	new_game_button_image = gtk_image_new_from_icon_name("document-new", 1);
	cancel_button_image = gtk_image_new_from_icon_name("window-close", 1);

	new_game_button = gtk_button_new_with_label("New Game");
	gtk_button_set_image(GTK_BUTTON(new_game_button), new_game_button_image);
	cancel_button = gtk_button_new_with_label("Cancel");
	gtk_button_set_image(GTK_BUTTON(cancel_button), cancel_button_image);

	char *result_message; 
	switch(game_result){
		case TRUMP_JACK_RESULT_LOSE:
			result_message = DIALOG_LABEL_LOSE;
			break;
		case TRUMP_JACK_RESULT_TIED:
			result_message = DIALOG_LABEL_TIE;
			break;
		case TRUMP_JACK_RESULT_WIN:
			result_message = DIALOG_LABEL_WIN;
			break;
	}

	dialog = gtk_message_dialog_new(game->main_window, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_NONE, result_message, NULL);
	gtk_dialog_add_action_widget(dialog, new_game_button, GTK_RESPONSE_OK);
	gtk_dialog_add_action_widget(dialog, cancel_button, GTK_RESPONSE_CANCEL);

	gtk_widget_show(new_game_button);
	gtk_widget_show(cancel_button);

	int result = gtk_dialog_run(GTK_DIALOG(dialog));

	if(result == GTK_RESPONSE_OK){
		restart_game(game);
	}

	gtk_widget_destroy(dialog);
}

static gboolean draw_background(GtkWidget *widget, cairo_t *cr, gpointer data){
	cairo_surface_t *surface;
	cairo_pattern_t *pattern;
	cairo_matrix_t matrix;

	surface = cairo_image_surface_create_from_png("./bg.png");
	
	int width = gtk_widget_get_allocated_width(widget);
	int height = gtk_widget_get_allocated_height(widget);

	pattern = cairo_pattern_create_for_surface(surface);
	cairo_pattern_set_extend(pattern, CAIRO_EXTEND_REPEAT);

	cairo_matrix_init_scale(&matrix, 1.0, 1.0);
	cairo_pattern_set_matrix(pattern, &matrix);

	cairo_set_source(cr, pattern);
	cairo_rectangle(cr, 0, 0, width, height);

	cairo_fill(cr);

	cairo_pattern_destroy(pattern);
	cairo_surface_destroy(surface);

	return false;
}

static void change_size(GtkWidget *widget, GdkRectangle *allocation, gpointer user_data){
}

static gboolean configure_event_background(GtkWidget *widget, GdkEventConfigure *event, gpointer data){
	if(data){
		cairo_surface_destroy(data);
	}

	data = gdk_window_create_similar_surface(gtk_widget_get_window(widget), CAIRO_CONTENT_COLOR, gtk_widget_get_allocated_width(widget), gtk_widget_get_allocated_height(widget));

	cairo_t *cr;
	cr = cairo_create(data);
	cairo_set_source_rgb(cr, 0.1, 0.3, 0.2);
	cairo_paint(cr);

	cairo_destroy(cr);

	return true;
}
