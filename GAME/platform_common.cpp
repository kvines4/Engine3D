struct Button_State {
	bool is_down;
	bool changed;
};

enum {
	BUTTON_ENTER,
	BUTTON_SPACE,
	BUTTON_CONTROL,
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_W,
	BUTTON_A,
	BUTTON_S,
	BUTTON_D,

	BUTTON_COUNT, // should be the last item
};

struct Input {
	Button_State buttons[BUTTON_COUNT];
};