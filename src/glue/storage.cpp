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


#include <cassert>
#include "core/model/model.h"
#include "core/channels/channel.h"
#include "core/channels/sampleChannel.h"
#include "core/channels/midiChannel.h"
#include "core/mixer.h"
#include "core/wave.h"
#include "core/mixerHandler.h"
#include "core/recorderHandler.h"
#include "core/pluginManager.h"
#include "core/pluginHost.h"
#include "core/plugin.h"
#include "core/conf.h"
#include "core/patch.h"
#include "core/init.h"
#include "core/waveManager.h"
#include "core/clock.h"
#include "core/wave.h"
#include "utils/gui.h"
#include "utils/log.h"
#include "utils/string.h"
#include "utils/fs.h"
#include "gui/elems/basics/progress.h"
#include "gui/elems/mainWindow/keyboard/column.h"
#include "gui/elems/mainWindow/keyboard/keyboard.h"
#include "gui/dialogs/mainWindow.h"
#include "gui/dialogs/warnings.h"
#include "gui/dialogs/browser/browserSave.h"
#include "gui/dialogs/browser/browserLoad.h"
#include "main.h"
#include "channel.h"
#include "storage.h"


extern giada::v::gdMainWindow* G_MainWin;


namespace giada {
namespace c {
namespace storage
{
namespace
{
std::string makeWavePath_(const std::string& base, const m::Wave& w, int k)
{
	return base + G_SLASH + w.getBasename(/*ext=*/false) + "-" + std::to_string(k) + "." +  w.getExtension();
} 


bool isWavePathUnique_(const m::Wave& skip, const std::string& path)
{
	m::model::WavesLock l(m::model::waves);

	for (const m::Wave* w : m::model::waves)
		if (w->id != skip.id && w->getPath() == path)
			return false;
	return true;
}

std::string makeUniqueWavePath_(const std::string& base, const m::Wave& w)
{
	std::string path = base + G_SLASH + w.getBasename(/*ext=*/true);
	if (isWavePathUnique_(w, path))
		return path;

	int k = 0;
	path = makeWavePath_(base, w, k);
	while (!isWavePathUnique_(w, path))
		path = makeWavePath_(base, w, k++);
	
	return path;
}


/* -------------------------------------------------------------------------- */


bool savePatch_(const std::string& path, const std::string& name, bool isProject)
{
	if (!m::patch::write(name, path, isProject))
		return false;
	u::gui::updateMainWinLabel(name);
	m::conf::patchPath = isProject ? u::fs::getUpDir(u::fs::getUpDir(path)) : u::fs::dirname(path);
	m::patch::name     = name;
	u::log::print("[savePatch] patch saved as %s\n", path.c_str());
	return true;
}


/* -------------------------------------------------------------------------- */


void saveWavesToProject_(const std::string& base)
{
	for (size_t i = 0; i < m::model::waves.size(); i++) {
		m::model::onSwap(m::model::waves, m::model::getId(m::model::waves, i), [&](m::Wave& w)
		{
			w.setPath(makeUniqueWavePath_(base, w));
			m::waveManager::save(w, w.getPath()); // TODO - error checking	
		});
	}
}
} // {anonymous}


/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */


void savePatch(void* data)
{
	v::gdBrowserSave* browser = (v::gdBrowserSave*) data;
	std::string name          = u::fs::stripExt(browser->getName());
	std::string fullPath      = browser->getCurrentPath() + G_SLASH + name + ".gptc";

	if (name == "") {
		v::gdAlert("Please choose a file name.");
		return;
	}

	if (u::fs::fileExists(fullPath))
		if (!v::gdConfirmWin("Warning", "File exists: overwrite?"))
			return;

	if (savePatch_(fullPath, name, /*isProject=*/false))
		browser->do_callback();
	else
		v::gdAlert("Unable to save the patch!");
}


/* -------------------------------------------------------------------------- */


void loadPatch(void* data)
{
	v::gdBrowserLoad* browser = (v::gdBrowserLoad*) data;
	std::string fullPath      = browser->getSelectedItem();
	bool isProject            = u::fs::isProject(browser->getSelectedItem());

	browser->showStatusBar();

	u::log::print("[glue] loading %s...\n", fullPath.c_str());

	std::string fileToLoad = fullPath;  // patch file to read from
	std::string basePath   = "";        // base path, in case of reading from a project
	if (isProject) {
		fileToLoad = fullPath + G_SLASH + u::fs::stripExt(u::fs::basename(fullPath)) + ".gptc";
		basePath   = fullPath + G_SLASH;
	}

	/* Verify that the patch file is valid first. */

	int ver = m::patch::verify(fileToLoad);	
	if (ver != G_PATCH_OK) {
		if (ver == G_PATCH_UNREADABLE)
			v::gdAlert("This patch is unreadable.");
		else
		if (ver == G_PATCH_INVALID)
			v::gdAlert("This patch is not valid.");
		else
		if (ver == G_PATCH_UNSUPPORTED)
			v::gdAlert("This patch format is no longer supported.");
		browser->hideStatusBar();
		return;
	}

	/* Then reset the system and read the patch. */

	m::init::reset();

	if (m::patch::read(fileToLoad, basePath) != G_PATCH_OK) {
		v::gdAlert("This patch is unreadable.");
		m::mixer::enable();
		return;
	}

	/* Prepare Mixer and Recorder. The latter has to recompute the actions 
	positions if the current samplerate != patch samplerate. */

	m::mixer::allocVirtualInput(m::clock::getFramesInLoop());
	m::mh::updateSoloCount();
	m::recorderHandler::updateSamplerate(m::conf::samplerate, m::patch::samplerate);

	/* Save patchPath by taking the last dir of the broswer, in order to reuse
	it the next time. */

	m::conf::patchPath = u::fs::dirname(fullPath);

	/* Mixer is ready to go back online. */

	m::mixer::enable();

	/* Update Main Window's title. */

	u::gui::updateMainWinLabel(m::patch::name);

	u::log::print("[glue] patch loaded successfully\n");

#ifdef WITH_VST

	if (m::pluginManager::hasMissingPlugins())
		v::gdAlert("Some plugins were not loaded successfully.\nCheck the plugin browser to know more.");

#endif

	browser->do_callback();
}


/* -------------------------------------------------------------------------- */


void saveProject(void* data)
{
	v::gdBrowserSave* browser = (v::gdBrowserSave*) data;
	std::string name            = u::fs::stripExt(browser->getName());
	std::string folderPath      = browser->getCurrentPath();
	std::string fullPath        = folderPath + G_SLASH + name + ".gprj";
	std::string gptcPath        = fullPath + G_SLASH + name + ".gptc";

	if (name == "") {
		v::gdAlert("Please choose a project name.");
		return;
	}

	if (u::fs::isProject(fullPath) && !v::gdConfirmWin("Warning", "Project exists: overwrite?"))
		return;

	if (!u::fs::dirExists(fullPath) && !u::fs::mkdir(fullPath)) {
		u::log::print("[saveProject] Unable to make project directory!\n");
		return;
	}

	u::log::print("[saveProject] Project dir created: %s\n", fullPath.c_str());

	saveWavesToProject_(fullPath);

	if (savePatch_(gptcPath, name, /*isProject=*/true))
		browser->do_callback();
	else
		v::gdAlert("Unable to save the project!");

}


/* -------------------------------------------------------------------------- */


void loadSample(void* data)
{
	v::gdBrowserLoad* browser  = (v::gdBrowserLoad*) data;
	std::string       fullPath = browser->getSelectedItem();

	if (fullPath.empty())
		return;

	int res = c::channel::loadChannel(browser->getChannelId(), fullPath);

	if (res == G_RES_OK) {
		m::conf::samplePath = u::fs::dirname(fullPath);
		browser->do_callback();
		G_MainWin->delSubWindow(WID_SAMPLE_EDITOR); // if editor is open
	}
}


/* -------------------------------------------------------------------------- */


void saveSample(void* data)
{
	v::gdBrowserSave* browser = (v::gdBrowserSave*) data;
	std::string name          = browser->getName();
	std::string folderPath    = browser->getCurrentPath();

	if (name == "") {
		v::gdAlert("Please choose a file name.");
		return;
	}

	std::string filePath = folderPath + G_SLASH + u::fs::stripExt(name) + ".wav";

	if (u::fs::fileExists(filePath) && !v::gdConfirmWin("Warning", "File exists: overwrite?"))
		return;

	ID waveId;
	m::model::onGet(m::model::channels, browser->getChannelId(), [&](m::Channel& c)
	{
		waveId = static_cast<m::SampleChannel&>(c).waveId;
	});

	size_t waveIndex = m::model::getIndex(m::model::waves, waveId);

	std::unique_ptr<m::Wave> wave = m::model::waves.clone(waveIndex);

	if (!m::waveManager::save(*wave.get(), filePath)) {
		v::gdAlert("Unable to save this sample!");
		return;
	}
	
	u::log::print("[saveSample] sample saved to %s\n", filePath.c_str());
	
	/* Update last used path in conf, so that it can be reused next time. */

	m::conf::samplePath = u::fs::dirname(filePath);

	/* Update logical and edited states in Wave. */

	wave->setLogical(false);
	wave->setEdited(false);

	m::model::waves.swap(std::move(wave), waveIndex);

	/* Finally close the browser. */

	browser->do_callback();
}
}}} // giada::c::storage::
