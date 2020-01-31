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


#include "core/channels/midiChannel.h"
#include "core/model/model.h"
#include "utils/gui.h"
#include "gui/elems/midiIO/midiLearnerChannel.h"
#include "gui/elems/basics/button.h"
#include "gui/elems/basics/check.h"
#include "gui/elems/basics/choice.h"
#include "midiOutputMidiCh.h"


namespace giada {
namespace v 
{
gdMidiOutputMidiCh::gdMidiOutputMidiCh(ID channelId)
: gdMidiOutputBase(300, 168, channelId)
{
	m::model::ChannelsLock l(m::model::channels);
	m::MidiChannel& c = static_cast<m::MidiChannel&>(m::model::get(m::model::channels, m_channelId));
	
	setTitle(m_channelId + 1);
	begin();

	m_enableOut   = new geCheck(x()+8, y()+8, 150, 20, "Enable MIDI output");
	m_chanListOut = new geChoice(w()-108, y()+8, 100, 20);

	m_enableLightning = new geCheck(x()+8, m_chanListOut->y()+m_chanListOut->h()+8, 120, 20, "Enable MIDI lightning output");
	m_learners.push_back(new geMidiLearnerChannel(x()+8, m_enableLightning->y()+m_enableLightning->h()+8,  
		w()-16, "playing", G_MIDI_OUT_L_PLAYING, c.midiOutLplaying, m_channelId));
	m_learners.push_back(new geMidiLearnerChannel(x()+8, m_enableLightning->y()+m_enableLightning->h()+32, 
		w()-16, "mute", G_MIDI_OUT_L_MUTE, c.midiOutLmute, m_channelId));
	m_learners.push_back(new geMidiLearnerChannel(x()+8, m_enableLightning->y()+m_enableLightning->h()+56, 
		w()-16, "solo", G_MIDI_OUT_L_SOLO, c.midiOutLsolo, m_channelId));

	m_close = new geButton(w()-88, m_enableLightning->y()+m_enableLightning->h()+84, 80, 20, "Close");

	end();

	m_chanListOut->add("Channel 1");
	m_chanListOut->add("Channel 2");
	m_chanListOut->add("Channel 3");
	m_chanListOut->add("Channel 4");
	m_chanListOut->add("Channel 5");
	m_chanListOut->add("Channel 6");
	m_chanListOut->add("Channel 7");
	m_chanListOut->add("Channel 8");
	m_chanListOut->add("Channel 9");
	m_chanListOut->add("Channel 10");
	m_chanListOut->add("Channel 11");
	m_chanListOut->add("Channel 12");
	m_chanListOut->add("Channel 13");
	m_chanListOut->add("Channel 14");
	m_chanListOut->add("Channel 15");
	m_chanListOut->add("Channel 16");
	m_chanListOut->value(0);
		
	if (c.midiOut)
		m_enableOut->value(1);
	else
		m_chanListOut->deactivate();

	m_enableLightning->value(c.midiOutL);
	for (geMidiLearnerBase* l : m_learners)
		c.midiOutL ? l->activate() : l->deactivate();

	m_chanListOut->value(c.midiOutChan);
	m_chanListOut->callback(cb_setChannel, (void*)this);

	m_enableOut->callback(cb_enableOut, (void*)this);
	m_enableLightning->callback(cb_enableLightning, (void*)this);
	m_close->callback(cb_close, (void*)this);

	set_modal();
	u::gui::setFavicon(this);
	show();
}


/* -------------------------------------------------------------------------- */


void gdMidiOutputMidiCh::cb_enableOut (Fl_Widget *w, void *p) { ((gdMidiOutputMidiCh*)p)->cb_enableOut(); }
void gdMidiOutputMidiCh::cb_setChannel(Fl_Widget *w, void *p) { ((gdMidiOutputMidiCh*)p)->cb_setChannel(); }


/* -------------------------------------------------------------------------- */


void gdMidiOutputMidiCh::cb_enableOut()
{
	m::model::onSwap(m::model::channels, m_channelId, [&](m::Channel& c)
	{
		static_cast<m::MidiChannel&>(c).midiOut     = m_enableOut->value();
		static_cast<m::MidiChannel&>(c).midiOutChan = m_chanListOut->value();
	});

	m_enableOut->value() ? m_chanListOut->activate() : m_chanListOut->deactivate();
}


/* -------------------------------------------------------------------------- */


void gdMidiOutputMidiCh::cb_setChannel()
{
	m::model::onSwap(m::model::channels, m_channelId, [&](m::Channel& c)
	{
		static_cast<m::MidiChannel&>(c).midiOutChan = m_chanListOut->value();
	});
}
}} // giada::v::
