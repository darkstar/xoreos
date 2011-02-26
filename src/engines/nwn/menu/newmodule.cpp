/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/newmodule.cpp
 *  The new module menu.
 */

#include "common/configman.h"
#include "common/filepath.h"
#include "common/filelist.h"

#include "engines/nwn/menu/newmodule.h"

#include "engines/aurora/util.h"

namespace Engines {

namespace NWN {

NewModuleMenu::NewModuleMenu() {
	load("pre_loadmod");
}

NewModuleMenu::~NewModuleMenu() {
}

void NewModuleMenu::show() {
	initModuleList();
}

void NewModuleMenu::initModuleList() {
	WidgetEditBox &moduleList = *getEditBox("ModuleListBox", true);

	moduleList.clear();
	moduleList.setMode(WidgetEditBox::kModeSelectable);

	Common::UString moduleDir = ConfigMan.getString("NWN_extraModuleDir");
	if (moduleDir.empty())
		return;

	Common::FileList moduleDirList;

	moduleDirList.addDirectory(moduleDir);

	std::list<Common::UString> modules;
	moduleDirList.getFileNames(modules);

	modules.sort(Common::UString::iless());

	for (std::list<Common::UString>::const_iterator m = modules.begin(); m != modules.end(); ++m)
		moduleList.addLine(Common::FilePath::getStem(*m));

	GUI::show();
}

void NewModuleMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

}

} // End of namespace NWN

} // End of namespace Engines