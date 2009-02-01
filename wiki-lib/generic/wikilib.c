#include <wikilib.h>
#include <input.h>
#include <msg.h>
#include <malloc.h>
#include <file-io.h>

int wikilib_init (void)
{
	return 0;
}

int wikilib_run(void)
{
	void *a;
	int fd, i;

	a = malloc(512);
	msg(MSG_INFO, " a = %p", a);

	
	msg(MSG_INFO, " 1. run", a);
	fd = wl_open("/kernel", WL_O_RDONLY);
	wl_read(fd, a, 512);
	wl_close(fd);

	for (i = 0; i < 10; i++) {
		msg(MSG_INFO, " 2. run", a);
		fd = wl_open("/kernel", WL_O_RDONLY);
		wl_read(fd, a, 512);
		wl_read(fd, a, 512);
		wl_read(fd, a, 512);
		wl_close(fd);
	}

//	dump_cache_stats();

	for (;;) {
		struct wl_input_event ev;
		wl_input_wait(&ev);

		switch (ev.type) {
		case WL_INPUT_EV_TYPE_KEYBOARD:
			msg(MSG_INFO, "%s() got key: %d", __func__, ev.key_event.keycode);
			break;
		case WL_INPUT_EV_TYPE_TOUCH:
			msg(MSG_INFO, "%s() touch event @%d,%d val %d\n", __func__,
				ev.touch_event.x,
				ev.touch_event.y,
				ev.touch_event.value);
			break;
		}
	}

	/* never reached */
	return 0;
}

