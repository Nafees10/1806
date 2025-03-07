#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/input.h>
#include <time.h>

// delay to add in synthesized keys
#define DELAY_SYNTH_US (20 * 1000)

// delay between double tap keys
#define DELAY_INPUT_US 100000

#define REPEAT_DELAY_MS 300

#define UP 0
#define DOWN 1

const struct input_event syn = {.type = EV_SYN, .code = SYN_REPORT, .value = UP};

typedef int (*layer_map)(struct input_event* ev, size_t toMap, size_t *pressed);

static inline long long time_ms(){
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ts.tv_sec * 1000LL) + (ts.tv_nsec / 1000000LL);
}

int global_map(struct input_event *ev){
	switch (ev->code){
		case KEY_CAPSLOCK:
			ev->code = KEY_ESC;
			return 1;
		case KEY_ESC:
			ev->code = KEY_GRAVE;
			return 1;
		case KEY_SYSRQ:
			ev->code = KEY_COMPOSE;
			return 1;
	}
	return 0;
}

void event_write(const struct input_event *event) {
	if (fwrite(event, sizeof(struct input_event), 1, stdout) != 1)
		exit(EXIT_FAILURE);
}

int event_read(struct input_event *event) {
	while (1){
		const int ret = fread(event, sizeof(struct input_event), 1, stdin) == 1;
		if (!ret)
			return ret;
		if (event->type != EV_KEY){
			event_write(event);
			continue;
		}
		global_map(event);
		return ret;
	}
}

void key_act(__u16 code, __s32 value){
	struct input_event ev = {.type = EV_KEY, .code = code, .value = value};
	event_write(&ev);
}

int ralt_map(struct input_event *ev, size_t toMap, size_t *state){
	size_t bit = 0;
	__u16 code = ev->code;
	switch (ev->code){
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
			|| (bit & toMap) == 0 // not in toMap
			|| (!ev->value && (bit & *state) == 0)) // released, but never pressed under mapping
		return 0;
	*state = (*state & ~bit);
	if (ev->value)
		*state |= bit;
	ev->code = code;
	return 1;
}

#define BIT_ALT       (1 << 0)
#define BIT_RET       (1 << 1)
#define BIT_RET_USED  (1 << 2)

int main(void) {
	setbuf(stdin, NULL), setbuf(stdout, NULL);
	struct input_event ev;
	size_t ralt_state = 0;
	int state = 0;
	while (event_read(&ev)) {
		if (ev.value == UP &&
				(ralt_state && ralt_map(&ev, ralt_state, &ralt_state))
				){
			event_write(&ev);
			continue;
		}

		if (ev.code == KEY_ENTER){
			if (ev.value == UP){
				if ((state & BIT_RET_USED) == 0){
					key_act(KEY_ENTER, DOWN);
					event_write(&syn);
					usleep(DELAY_SYNTH_US);
					key_act(KEY_ENTER, UP);
				} else {
					key_act(KEY_RIGHTCTRL, UP);
				}
				state &= ~(BIT_RET_USED | BIT_RET);
			} else {
				state |= BIT_RET;
			}
			continue;
		}

		if (ev.code == KEY_RIGHTALT){
			if (ev.value){
				state |= BIT_ALT;
			} else {
				state &= ~BIT_ALT;
			}
			continue;
		}

		if (state & BIT_ALT)
			ralt_map(&ev, ~(size_t)0, &ralt_state);

		if (ev.value && (state & BIT_RET) && (state & BIT_RET_USED) == 0){
			state |= BIT_RET_USED;
			key_act(KEY_RIGHTCTRL, DOWN);
			event_write(&ev);
			event_write(&syn);
			continue;
		}
		event_write(&ev);
	}
	fprintf(stderr, "stdin got EOF. Bye Bye\n");
}
