#define is_down(b)  input->buttons[b].is_down
#define pressed(b)  (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)

Engine3D engine;

internal void
game_init() {
	engine.game_init();
}

internal void
simulate_game(Input* input, double delta_time) {

	//clear_screen(COLOR_BLACK);
	
	engine.game_update(input, delta_time);
}