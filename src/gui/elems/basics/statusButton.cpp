/* -----------------------------------------------------------------------------
 *
 * Giada - Your Hardcore Loopmachine
 *
 * geStatusButton
 * Simple geButton with a boolean 'status' parameter.
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


#include <FL/fl_draw.H>
#include "core/const.h"
#include "statusButton.h"


geStatusButton::geStatusButton(int x, int y, int w, int h, const char** imgOff,
	const char** imgOn, const char** imgDisabled)
: geButton(x, y, w, h, nullptr, imgOff, imgOn, imgDisabled),
  m_status(false)
{
}


/* -------------------------------------------------------------------------- */


void geStatusButton::draw()
{
	geButton::draw();
	fl_draw_pixmap(m_status ? imgOn : imgOff, x()+1, y()+1, G_COLOR_GREY_4);
}


/* -------------------------------------------------------------------------- */


void geStatusButton::setStatus(bool s)
{
    m_status = s;
    redraw();
}


bool geStatusButton::getStatus() const
{
    return m_status;
}
