/* -----------------------------------------------------------------------------
 *
 * Giada - Your Hardcore Loopmachine
 *
 * -----------------------------------------------------------------------------
 *
 * Copyright (C) 2010-2020 Giovanni A. Zuliani | Monocasual
 *
 * This file is part of Giada - Your Hardcore Loopmachine.
 *
 * Giada - Your Hardcore Loopmachine is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * Giada - Your Hardcore Loopmachine is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Giada - Your Hardcore Loopmachine. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * -------------------------------------------------------------------------- */


#include "core/const.h"
#include "core/conf.h"
#include "gui/elems/basics/choice.h"
#include "tabMisc.h"


namespace giada {
namespace v
{
geTabMisc::geTabMisc(int X, int Y, int W, int H)
: Fl_Group(X, Y, W, H, "Misc")
{
	begin();
	debugMsg = new geChoice(x()+w()-230, y()+9, 230, 20, "Debug messages");
	end();

	debugMsg->add("(disabled)");
	debugMsg->add("To standard output");
	debugMsg->add("To file");

	labelsize(G_GUI_FONT_SIZE_BASE);
	selection_color(G_COLOR_GREY_4);

	switch (m::conf::conf.logMode) {
		case LOG_MODE_MUTE:
			debugMsg->value(0);
			break;
		case LOG_MODE_STDOUT:
			debugMsg->value(1);
			break;
		case LOG_MODE_FILE:
			debugMsg->value(2);
			break;
	}
}


/* -------------------------------------------------------------------------- */


void geTabMisc::save()
{
	switch(debugMsg->value()) {
		case 0:
			m::conf::conf.logMode = LOG_MODE_MUTE;
			break;
		case 1:
			m::conf::conf.logMode = LOG_MODE_STDOUT;
			break;
		case 2:
			m::conf::conf.logMode = LOG_MODE_FILE;
			break;
	}
}
}} // giada::v::