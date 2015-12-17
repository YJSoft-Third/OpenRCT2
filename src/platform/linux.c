/*****************************************************************************
 * Copyright (c) 2014 Ted John
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * This file is part of OpenRCT2.
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#if defined(__linux__)

#include "platform.h"
#include <dlfcn.h>
#include <stdlib.h>
#include "../util/util.h"
#include <gtk/gtk.h>

// See http://syprog.blogspot.ru/2011/12/listing-loaded-shared-objects-in-linux.html
struct lmap {
	void* base_address;
	char* path;
	void* unused;
	struct lmap *next, *prev;
};

struct dummy {
	void* pointers[3];
	struct dummy* ptr;
};

void platform_get_exe_path(utf8 *outPath)
{
	char exePath[MAX_PATH];
	ssize_t bytesRead;
	bytesRead = readlink("/proc/self/exe", exePath, MAX_PATH);
	if (bytesRead == -1) {
		log_fatal("failed to read /proc/self/exe");
	}
	exePath[bytesRead - 1] = '\0';
	char *exeDelimiter = strrchr(exePath, platform_get_path_separator());
	if (exeDelimiter == NULL)
	{
		log_error("should never happen here");
		outPath[0] = '\0';
		return;
	}
	int exeDelimiterIndex = (int)(exeDelimiter - exePath);

	safe_strncpy(outPath, exePath, exeDelimiterIndex + 1);
	outPath[exeDelimiterIndex] = '\0';
}

bool platform_check_steam_overlay_attached() {
	void* processHandle = dlopen(NULL, RTLD_NOW);

	struct dummy* p = (struct dummy*) processHandle;
	p = p->ptr;

	struct lmap* pl = (struct lmap*) p->ptr;

	while (pl != NULL) {
		if (strstr(pl->path, "gameoverlayrenderer.so") != NULL) {
			dlclose(processHandle);
			return true;
		}
		pl = pl->next;
	}
	dlclose(processHandle);

	return false;
}

/**
 * Default directory fallback is:
 *   - (command line argument)
 *   - $XDG_CONFIG_HOME/OpenRCT2
 *   - /home/[uid]/.config/OpenRCT2
 */
void platform_posix_sub_user_data_path(char *buffer, const char *homedir, const char *separator) {
	const char *configdir = getenv("XDG_CONFIG_HOME");
	log_verbose("configdir = '%s'", configdir);
	if (configdir == NULL)
	{
		log_verbose("configdir was null, used getuid, now is = '%s'", homedir);
		if (homedir == NULL)
		{
			log_fatal("Couldn't find user data directory");
			exit(-1);
			return;
		}

		strncat(buffer, homedir, MAX_PATH - 1);
		strncat(buffer, separator, MAX_PATH - strnlen(buffer, MAX_PATH) - 1);
		strncat(buffer, ".config", MAX_PATH - strnlen(buffer, MAX_PATH) - 1);
	}
	else
	{
		strncat(buffer, configdir, MAX_PATH - 1);
	}
	strncat(buffer, separator, MAX_PATH - strnlen(buffer, MAX_PATH) - 1);
	strncat(buffer, "OpenRCT2", MAX_PATH - strnlen(buffer, MAX_PATH) - 1);
	strncat(buffer, separator, MAX_PATH - strnlen(buffer, MAX_PATH) - 1);
}

utf8 *platform_open_directory_browser(utf8 *title)
{
	STUB();
	gint res;
	utf8 *outpath = NULL;
	GtkWidget *widget =
	gtk_file_chooser_dialog_new ("OpenRCT2",
	                             NULL,
	                             GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
								 "_Cancel",
                                      GTK_RESPONSE_CANCEL,
                                      "_Open",
                                      GTK_RESPONSE_ACCEPT,
                                      NULL);
									 gtk_file_chooser_set_local_only (GTK_FILE_CHOOSER(widget), true);
									  res = gtk_dialog_run (GTK_DIALOG (widget));
									  if (res == GTK_RESPONSE_ACCEPT)
  {
    char *filename;
    GtkFileChooser *chooser = GTK_FILE_CHOOSER (widget);
    filename = gtk_file_chooser_get_filename (chooser);
	size_t sz = strlen(filename) + 1;
	outpath = malloc(sz);
    safe_strncpy(outpath, filename, sz);
    g_free (filename);
}
			  gtk_widget_destroy (widget);
	return outpath;
}

void platform_show_messagebox(char *message)
{
	STUB();
	GtkWidget *widget =
gtk_message_dialog_new (NULL,
                        GTK_DIALOG_MODAL,
                        GTK_MESSAGE_WARNING,
                        GTK_BUTTONS_OK,
                        "test");
						gtk_dialog_run (GTK_DIALOG (widget));
gtk_widget_destroy (widget);
	log_verbose(message);
}

/**
 *
 *  rct2: 0x004080EA
 */
int platform_open_common_file_dialog(int type, utf8 *title, utf8 *filename, utf8 *filterPattern, utf8 *filterName)
{
	STUB();
	return 0;
}

#endif
