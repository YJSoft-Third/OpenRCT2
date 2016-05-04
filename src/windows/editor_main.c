#include "../addresses.h"
#include "../interface/viewport.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../world/footpath.h"

static void window_editor_main_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_editor_main_events = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_editor_main_paint,// 0x0066FC97, //window_editor_main_paint,
	NULL,
};

static rct_widget window_editor_main_widgets[] = {
	{ WWT_VIEWPORT, 0, 0, 0xFFFF, 0, 0xFFFF, 0xFFFFFFFE, 0xFFFF },
	{ WIDGETS_END },
};


/**
* Creates the main editor window that holds the main viewport.
*  rct2: 0x0066EF38
*/
void window_editor_main_open()
{
	rct_window* window;

	window_editor_main_widgets[0].right = gScreenWidth;
	window_editor_main_widgets[0].bottom = gScreenHeight;
	window = window_create(0, 0, window_editor_main_widgets[0].right, window_editor_main_widgets[0].bottom,
		&window_editor_main_events, WC_MAIN_WINDOW, WF_STICK_TO_BACK);
	window->widgets = window_editor_main_widgets;

	viewport_create(window, window->x, window->y, window->width, window->height, 0, 0x0FFF, 0x0FFF, 0, 0x1, -1);
	window->viewport->flags |= 0x0400;

	gCurrentRotation = 0;
	RCT2_GLOBAL(0x009E32B0, uint8) = 0;
	RCT2_GLOBAL(0x009E32B2, uint8) = 0;
	RCT2_GLOBAL(0x009E32B3, uint8) = 0;
	gFootpathSelectedType = 0;

	window_top_toolbar_open();
	window_editor_bottom_toolbar_open();
}

/**
*
* rct2: 0x0066FC97
* This function immediately jumps to 0x00685BE1
*/
static void window_editor_main_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	viewport_render(dpi, w->viewport, dpi->x, dpi->y, dpi->x + dpi->width, dpi->y + dpi->height);
}
