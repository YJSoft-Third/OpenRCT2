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

#if defined(__APPLE__) && defined(__MACH__)

@import AppKit;
#include <mach-o/dyld.h>
#include "platform.h"
#include "../util/util.h"

bool platform_check_steam_overlay_attached() {
	STUB();
	return false;
}

void platform_get_exe_path(utf8 *outPath)
{
	char exePath[MAX_PATH];
	uint32_t size = MAX_PATH;
	int result = _NSGetExecutablePath(exePath, &size);
	if (result != 0) {
		log_fatal("failed to get path");
	}
	exePath[MAX_PATH - 1] = '\0';
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

/**
 * Default directory fallback is:
 *   - (command line argument)
 *   - ~/Library/Application Support/OpenRCT2
 */
void platform_posix_sub_user_data_path(char *buffer, const char *homedir, const char *separator) {
	if (homedir == NULL)
	{
		log_fatal("Couldn't find user data directory");
		exit(-1);
		return;
	}
	
	strncat(buffer, homedir, MAX_PATH - 1);
	strncat(buffer, separator, MAX_PATH - strnlen(buffer, MAX_PATH) - 1);
	strncat(buffer, "Library", MAX_PATH - strnlen(buffer, MAX_PATH) - 1);
	strncat(buffer, separator, MAX_PATH - strnlen(buffer, MAX_PATH) - 1);
	strncat(buffer, "Application Support", MAX_PATH - strnlen(buffer, MAX_PATH) - 1);
	strncat(buffer, separator, MAX_PATH - strnlen(buffer, MAX_PATH) - 1);
	strncat(buffer, "OpenRCT2", MAX_PATH - strnlen(buffer, MAX_PATH) - 1);
	strncat(buffer, separator, MAX_PATH - strnlen(buffer, MAX_PATH) - 1);
}

void platform_show_messagebox(char *message)
{
	NSAlert *alert = [[NSAlert alloc] init];
	[alert addButtonWithTitle:@"OK"];
	[alert setMessageText:[NSString stringWithUTF8String:message]];
	[alert setAlertStyle:NSWarningAlertStyle];
	[alert runModal];
	[alert release];
}

utf8 *platform_open_directory_browser(utf8 *title)
{
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	panel.canChooseFiles = false;
	panel.canChooseDirectories = true;
	panel.allowsMultipleSelection = false;
	utf8 *url = NULL;
	if ([panel runModal] == NSFileHandlingPanelOKButton)
	{
		NSString *selectedPath = [[[panel URLs] firstObject] path];
		const char *path = [selectedPath UTF8String];
		url = (utf8*)malloc(strlen(path) + 1);
		strcpy(url,path);
	}
	return url;
}

int platform_open_common_file_dialog(int type, utf8 *title, utf8 *filename, utf8 *filterPattern, utf8 *filterName)
{
	NSArray *extensions = [
		[
			[NSString stringWithUTF8String:filterPattern]
			stringByReplacingOccurrencesOfString:@"*." withString:@""
		]
		componentsSeparatedByString:@";"
	];
	NSString *filePath = [NSString stringWithUTF8String:filename];
	NSString *directory = [filePath stringByDeletingLastPathComponent];
	NSString *basename = [filePath lastPathComponent];
	if (type == 0)
	{
		NSSavePanel *panel = [NSSavePanel savePanel];
		panel.title = [NSString stringWithUTF8String:title];
		panel.nameFieldStringValue = [NSString stringWithFormat:@"%@.%@",basename,[extensions firstObject]];
		panel.allowedFileTypes = extensions;
		panel.directoryURL = [NSURL fileURLWithPath:directory];
		if ([panel runModal] == NSFileHandlingPanelCancelButton){
			return 0;
		} else {
			strcpy(filename,[[[panel URL] path] UTF8String]);
			return 1;
		}
	}
	else if (type == 1)
	{
		NSOpenPanel *panel = [NSOpenPanel openPanel];
		panel.title = [NSString stringWithUTF8String:title];
		panel.allowedFileTypes = extensions;
		panel.canChooseDirectories = false;
		panel.canChooseFiles = true;
		panel.allowsMultipleSelection = false;
		panel.directoryURL = [NSURL fileURLWithPath:filePath];
		if ([panel runModal] == NSFileHandlingPanelCancelButton){
			return 0;
		} else {
			strcpy(filename,[[[[panel URLs] firstObject] path] UTF8String]);
			return 1;
		}
	} else {
		return 0;
	}
}

#endif