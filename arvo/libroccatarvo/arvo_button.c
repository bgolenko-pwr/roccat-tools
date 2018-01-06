/*
 * This file is part of roccat-tools.
 *
 * roccat-tools is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * roccat-tools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with roccat-tools. If not, see <http://www.gnu.org/licenses/>.
 */

#include <gaminggear/hid_uid.h>
#include "arvo.h"

static void arvo_button_finalize(ArvoButton *button, guint profile_number, guint button_number) {
	button->report_id = ARVO_REPORT_ID_BUTTON;
	button->profile = profile_number;
	button->button = button_number;
}

static guint8 const table_hid_usage_id_to_arvo_button_key[256] = {
	/* 0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f */
	0x00, 0x00, 0x00, 0x00, 0x12, 0x61, 0x18, 0x14, 0x10, 0x31, 0x40, 0x42, 0x80, 0x32, 0x33, 0x34, /* 0 */
	0x52, 0x62, 0x81, 0x71, 0x90, 0x97, 0x13, 0x21, 0x9f, 0x51, 0x98, 0x1a, 0x22, 0x1b, 0x16, 0x15, /* 1 */
	0x1f, 0xa0, 0x17, 0x72, 0x82, 0x84, 0x8b, 0x8a, 0x5a, 0x93, 0x2a, 0x11, 0xa8, 0x7b, 0x73, 0x5b, /* 2 */
	0x23, 0x3a, 0x00, 0x3b, 0x4b, 0x96, 0x53, 0x54, 0x6b, 0x76, 0x9e, 0x70, 0x20, 0x9a, 0x8f, 0x43, /* 3 */
	0x24, 0x64, 0x7a, 0x88, 0x4a, 0x6a, 0x8e, 0x99, 0x91, 0x0f, 0x75, 0x77, 0x78, 0x85, 0x86, 0x67, /* 4 */
	0x65, 0x68, 0x45, 0x58, 0x57, 0x56, 0x66, 0xa6, 0x35, 0x41, 0x92, 0x36, 0x2b, 0x74, 0x30, 0xaf, /* 5 */
	0x83, 0x6f, 0x47, 0x1e, 0x00, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 6 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 7 */
	0x00, 0x00, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 8 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 9 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* a */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* b */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* c */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* d */
	0x7d, 0x39, 0x4c, 0xa1, 0x5d, 0x29, 0x6c, 0xa3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* e */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  /* f */
};

static guint8 arvo_hid_to_arvo_button_key(guint8 usage_id) {
	return table_hid_usage_id_to_arvo_button_key[usage_id];
}

static ArvoButton *button_with_hid_sequence(guint count, guint8 *usage_ids) {
	ArvoButton *button;
	ArvoButtonKey *key;
	gint i;

	g_assert(count <= ARVO_BUTTON_KEYSTROKES_NUM / 2);

	button = g_malloc0(sizeof(ArvoButton));

	if (count == 0)
		return button;

	key = &button->keys[0];
	i = 0;
	while (i < (gint)count) {
		key->key = arvo_hid_to_arvo_button_key(usage_ids[i]);
		key->action = ARVO_BUTTON_KEY_ACTION_PRESS;
		++key;
		++i;
	}
	--i;
	while (i >= 0) {
		key->key = arvo_hid_to_arvo_button_key(usage_ids[i]);
		key->action = ARVO_BUTTON_KEY_ACTION_RELEASE;
		++key;
		--i;
	}
	return button;
}

ArvoButton *arvo_rkp_button_info_to_button(ArvoRkpButtonInfo *info) {
	guint8 array[ARVO_BUTTON_KEYSTROKES_NUM / 2];
	guint count = 0;

	if (info->type == ARVO_RKP_BUTTON_INFO_TYPE_SHORTCUT) {
		if (info->shortcut_shift == ARVO_RKP_BUTTON_INFO_SHORTCUT_STATE_ON) {
			array[count] = HID_UID_KB_LEFT_SHIFT;
			++count;
		}
		if (info->shortcut_ctrl == ARVO_RKP_BUTTON_INFO_SHORTCUT_STATE_ON) {
			array[count] = HID_UID_KB_LEFT_CONTROL;
			++count;
		}
		if (info->shortcut_win == ARVO_RKP_BUTTON_INFO_SHORTCUT_STATE_ON) {
			array[count] = HID_UID_KB_LEFT_GUI;
			++count;
		}
		if (info->shortcut_alt == ARVO_RKP_BUTTON_INFO_SHORTCUT_STATE_ON) {
			array[count] = HID_UID_KB_LEFT_ALT;
			++count;
		}
		array[count] = info->shortcut_key;
		++count;
	}

	return button_with_hid_sequence(count, array);
}

gboolean arvo_button_write(RoccatDevice *arvo, ArvoButton *button,
		guint profile_number, guint button_number, GError **error) {

	g_assert(profile_number >= 1 && profile_number <= ARVO_PROFILE_NUM);
	g_assert(button_number >= 1 && button_number <= ARVO_BUTTON_NUM);

	arvo_button_finalize(button, profile_number, button_number);
	return arvo_device_write(arvo, (gchar const *)button, sizeof(ArvoButton), error);
}