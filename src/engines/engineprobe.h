/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
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
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/engineprobe.h
 *  A probe that checks if an engine can handle game data found in a specific directory and
 *  creates an instance of that engine.
 */

#ifndef ENGINES_ENGINEPROBE_H
#define ENGINES_ENGINEPROBE_H

#include "aurora/types.h"

namespace Common {
	class UString;
	class SeekableReadStream;
	class FileList;
}

namespace Engines {

class Engine;

/** A probe able to detect one specific game. */
class EngineProbe {
public:
	virtual ~EngineProbe() {}

	/** Get the GameID that the probe is able to detect. */
	virtual Aurora::GameID getGameID() const = 0;

	/** Return a string of the full game name. */
	virtual const Common::UString &getGameName() const = 0;

	/** Check for the game in that directory, containing these files. */
	virtual bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const = 0;

	/** Check for the game in that file. */
	virtual bool probe(Common::SeekableReadStream &stream) const = 0;

	/** Create the respective engine for the GameID. */
	virtual Engine *createEngine() const = 0;

	/** Get the GameID that the probe is able to detect. */
	virtual Aurora::Platform getPlatform() const = 0;
};

} // End of namespace Engines

#endif // ENGINES_ENGINEPROBE_H
