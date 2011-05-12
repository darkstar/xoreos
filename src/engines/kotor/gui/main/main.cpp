/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/kotor/gui/main/main.cpp
 *  The KotOR main menu.
 */

#include "common/util.h"

#include "events/events.h"

#include "engines/aurora/widget.h"

#include "engines/kotor/module.h"

#include "engines/kotor/gui/main/main.h"

namespace Engines {

namespace KotOR {

MainMenu::MainMenu(Module &module) : _module(&module) {
	load("mainmenu16x12");

	addBackground("back");
}

MainMenu::~MainMenu() {
}

void MainMenu::initWidget(Widget &widget) {
	// BioWare logo, the original game doesn't display it.
	if (widget.getTag() == "LBL_BW") {
		widget.setInvisible(true);
		return;
	}

	// LucasArts logo, the original game doesn't display it.
	if (widget.getTag() == "LBL_LUCAS") {
		widget.setInvisible(true);
		return;
	}

	// Warp button? O_o
	if (widget.getTag() == "BTN_WARP") {
		widget.setInvisible(true);
		return;
	}

	// New downloadable content is available, bluhbluh.
	if (widget.getTag() == "LBL_NEWCONTENT") {
		widget.setInvisible(true);
		return;
	}
}

void MainMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "BTN_EXIT") {
		EventMan.requestQuit();

		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "BTN_NEWGAME") {
		if (_module->load("lev_m40aa"))
			_returnCode = 2;
		return;
	}
}

} // End of namespace KotOR

} // End of namespace Engines