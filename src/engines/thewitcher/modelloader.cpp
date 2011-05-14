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

/** @file engines/thewitcher/modelloader.cpp
 *  The Witcher model loader.
 */

#include "common/error.h"
#include "common/stream.h"

#include "aurora/types.h"
#include "aurora/resman.h"

#include "graphics/aurora/model_witcher.h"

#include "engines/thewitcher/modelloader.h"

namespace Engines {

namespace TheWitcher {

Graphics::Aurora::Model *TheWitcherModelLoader::load(const Common::UString &resref,
		Graphics::Aurora::ModelType type, const Common::UString &texture) {

	Graphics::Aurora::Model *model = 0;
	try {
		model = new Graphics::Aurora::Model_Witcher(resref);
	} catch (...) {
		delete model;
		throw;
	}

	return model;
}

} // End of namespace TheWitcher

} // End of namespace Engines
