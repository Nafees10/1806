#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/input.h>

// delay to add in synthesized keys
#define DELAY_SYNTH_US (10 * 1000)

// delay between double tap keys
#define DELAY_INPUT_US 100000

#define REPEAT_DELAY_MS 300

#define UP 0
#define DOWN 1

const struct input_event syn = {.type = EV_SYN, .code = SYN_REPORT, .value = UP};

int event_read(struct input_event *event) {
	return fread(event, sizeof(struct input_event), 1, stdin) == 1;
}

void event_write(const struct input_event *event) {
	if (fwrite(event, sizeof(struct input_event), 1, stdout) != 1)
		exit(EXIT_FAILURE);
}

void key_act(__u16 code, __s32 value){
	struct input_event ev = {.type = EV_KEY, .code = code, .value = value};
	event_write(&ev);
}

int space_map(struct input_event *ev, size_t *toMap, size_t *pressed){
	size_t bit = 0;
	int code = ev->code;
	switch (ev->code){
		case KEY_CAPSLOCK:
			ev->code = KEY_ESC;
			break;

		case KEY_H:
			code = KEY_LEFT;
			bit = (size_t)1 << 0;
			break;
		case KEY_J:
			code = KEY_DOWN;
			bit = (size_t)1 << 1;
			break;
		case KEY_K:
			code = KEY_UP;
			bit = (size_t)1 << 2;
			break;
		case KEY_L:
			code = KEY_RIGHT;
			bit = (size_t)1 << 3;
			break;

		case KEY_O:
			code = KEY_HOME;
			bit = (size_t)1 << 4;
			break;
		case KEY_P:
			code = KEY_END;
			bit = (size_t)1 << 5;
			break;
		case KEY_LEFTBRACE:
			code = KEY_PAGEUP;
			bit = (size_t)1 << 6;
			break;
		case KEY_RIGHTBRACE:
			code = KEY_PAGEDOWN;
			bit = (size_t)1 << 7;
			break;

		case KEY_M:
			code = KEY_MUTE;
			bit = (size_t)1 << 8;
			break;
		case KEY_COMMA:
			code = KEY_VOLUMEDOWN;
			bit = (size_t)1 << 9;
			break;
		case KEY_DOT:
			code = KEY_VOLUMEUP;
			bit = (size_t)1 << 10;
			break;

		case KEY_1:
			code = KEY_F1;
			bit = (size_t)1 << 11;
			break;
		case KEY_2:
			code = KEY_F2;
			bit = (size_t)1 << 12;
			break;
		case KEY_3:
			code = KEY_F3;
			bit = (size_t)1 << 13;
			break;
		case KEY_4:
			code = KEY_F4;
			bit = (size_t)1 << 14;
			break;
		case KEY_5:
			code = KEY_F5;
			bit = (size_t)1 << 15;
			break;
		case KEY_6:
			code = KEY_F6;
			bit = (size_t)1 << 16;
			break;
		case KEY_7:
			code = KEY_F7;
			bit = (size_t)1 << 17;
			break;
		case KEY_8:
			code = KEY_F8;
			bit = (size_t)1 << 18;
			break;
		case KEY_9:
			code = KEY_F9;
			bit = (size_t)1 << 19;
			break;
		case KEY_0:
			code = KEY_F10;
			bit = (size_t)1 << 20;
			break;
		case KEY_MINUS:
			code = KEY_F11;
			bit = (size_t)1 << 21;
			break;
		case KEY_EQUAL:
			code = KEY_F12;
			bit = (size_t)1 << 22;
			break;

		case KEY_V:
			code = KEY_0;
			bit = (size_t)1 << 23;
			break;
		case KEY_Z:
			code = KEY_1;
			bit = (size_t)1 << 24;
			break;
		case KEY_X:
			code = KEY_2;
			bit = (size_t)1 << 25;
			break;
		case KEY_C:
			code = KEY_3;
			bit = (size_t)1 << 26;
			break;
		case KEY_A:
			code = KEY_4;
			bit = (size_t)1 << 27;
			break;
		case KEY_S:
			code = KEY_5;
			bit = (size_t)1 << 28;
			break;
		case KEY_D:
			code = KEY_6;
			bit = (size_t)1 << 29;
			break;
		case KEY_Q:
			code = KEY_7;
			bit = (size_t)1 << 30;
			break;
		case KEY_W:
			code = KEY_8;
			bit = (size_t)1 << 31;
			break;
		case KEY_E:
			code = KEY_9;
			bit = (size_t)1 << 32;
			break;
	}
	if (!bit // not mapped
			|| (bit & *toMap) == 0 // not in toMap
			|| (!ev->value && (bit & *pressed) == 0)) // released, but never pressed under mapping
		return 0;
	*pressed = (*pressed & ~bit);
	if (ev->value)
		*pressed |= bit;
	ev->code = code;
	return 1;
}

void space_layer(struct timeval time, size_t *pressed){
	const unsigned long long start_ms = time.tv_sec * 1000 + time.tv_usec / 1000;
	struct input_event ev;
	int used = 0;
	size_t toMap = ~(size_t)0;
	while (event_read(&ev)){
		if (ev.type != EV_KEY){
			event_write(&ev);
			continue;
		}

		if (used == 0 && (ev.time.tv_sec * 1000 + ev.time.tv_usec / 1000)
				- start_ms >= REPEAT_DELAY_MS){
			key_act(KEY_SPACE, DOWN);
			event_write(&syn);
			usleep(DELAY_SYNTH_US);
			event_write(&ev);
			while (event_read(&ev)){
				event_write(&ev);
				if (ev.type == EV_KEY && ev.code == KEY_SPACE && ev.value == 0)
					return;
			}
			return;
		}

		if (ev.code == KEY_SPACE){
			if (ev.value != UP)
				continue;
			if (used == 0){
				key_act(KEY_SPACE, DOWN);
				event_write(&syn);
				usleep(DELAY_SYNTH_US);
				event_write(&ev);
			}
			return;
		}
		space_map(&ev, &toMap, pressed);
		event_write(&ev);
		used |= *pressed;
	}
}

int main(void) {
	setbuf(stdin, NULL), setbuf(stdout, NULL);
	struct input_event ev;
	size_t space_pressed = 0;
	while (event_read(&ev)) {
		if (ev.type != EV_KEY){
			event_write(&ev);
			continue;
		}

		if (ev.type == DOWN && ev.code == KEY_SPACE){
			space_layer(ev.time, &space_pressed);
			continue;
		}

		if (space_pressed && space_map(&ev, &space_pressed, &space_pressed)){
			event_write(&ev);
			continue;
		}

		switch (ev.code){
		case KEY_CAPSLOCK:
			ev.code = KEY_ESC;
			break;

		default:
			event_write(&ev);
		}
		event_write(&ev);

	}
	fprintf(stderr, "stdin got EOF. Bye Bye\n");
}
