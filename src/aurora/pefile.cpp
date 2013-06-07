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

/** @file aurora/pefile.cpp
 *  A portable executable archive.
 */

#include "common/error.h"
#include "common/ustring.h"
#include "common/file.h"
#include "common/pe_exe.h"
#include "common/filepath.h"

#include "aurora/pefile.h"
#include "aurora/util.h"

namespace Aurora {

PEFile::PEFile(const Common::UString &fileName, const std::vector<Common::UString> &remap) :
	_peFile(0) {

	Common::File *file = new Common::File();
	if (!file->open(fileName)) {
		delete file;
		throw Common::Exception("Could not open exe");
	}

	_peFile = new Common::PEResources();

	if (!_peFile->loadFromEXE(file)) {
		delete file;
		delete _peFile;
		throw Common::Exception("Could not parse exe");
	}

	load(remap);
}

PEFile::~PEFile() {
	delete _peFile;
}

void PEFile::clear() {
	_resources.clear();
}

const Archive::ResourceList &PEFile::getResources() const {
	return _resources;
}

Common::SeekableReadStream *PEFile::getResource(uint32 index) const {
	// Convert from the PE cursor group/cursor format to the standalone
	// cursor format.

	Common::MemoryWriteStreamDynamic out;
	Common::SeekableReadStream *cursorGroup = _peFile->getResource(Common::kPEGroupCursor, index);

	if (!cursorGroup)
		return 0;

	// Cursor Group Header
	out.writeUint16LE(cursorGroup->readUint16LE());
	out.writeUint16LE(cursorGroup->readUint16LE());
	uint16 cursorCount = cursorGroup->readUint16LE();
	out.writeUint16LE(cursorCount);

	std::vector<Common::SeekableReadStream *> cursorStreams;
	cursorStreams.resize(cursorCount);

	uint32 startOffset = 6 + cursorCount * 16;

	for (uint16 i = 0; i < cursorCount; i++) {
		out.writeByte(cursorGroup->readUint16LE());     // width
		out.writeByte(cursorGroup->readUint16LE() / 2); // height
		cursorGroup->readUint16LE();                    // planes
		out.writeByte(cursorGroup->readUint16LE());     // bits per pixel
		out.writeByte(0);                               // reserved

		cursorGroup->readUint32LE();                    // data size
		uint16 id = cursorGroup->readUint16LE();

		Common::SeekableReadStream *cursor = _peFile->getResource(Common::kPECursor, id);
		if (!cursor) {
			warning("Could not get cursor resource %d", id);
			return 0;
		}

		out.writeUint16LE(cursor->readUint16LE());      // hotspot X
		out.writeUint16LE(cursor->readUint16LE());      // hotspot Y
		out.writeUint32LE(cursor->size() - 4);          // size
		out.writeUint32LE(startOffset);                 // offset
		startOffset += cursor->size() - 4;

		cursorStreams[i] = cursor;
	}

	for (uint32 i = 0; i < cursorStreams.size(); i++) {
		byte *data = new byte[cursorStreams[i]->size() - 4];
		cursorStreams[i]->read(data, cursorStreams[i]->size() - 4);
		out.write(data, cursorStreams[i]->size() - 4);
		delete[] data;
		delete cursorStreams[i];
	}

	return new Common::MemoryReadStream(out.getData(), out.size());
}

void PEFile::load(const std::vector<Common::UString> &remap) {
	std::vector<Common::PEResourceID> cursorList = _peFile->getNameList(Common::kPEGroupCursor);

	for (std::vector<Common::PEResourceID>::const_iterator it = cursorList.begin(); it != cursorList.end(); ++it) {
		Resource res;

		if (it->getID() == 0xFFFFFFFF)
			throw Common::Exception("Found non-integer cursor group");

		uint32 id = it->getID() - 1;
		if (id >= remap.size())
			res.name = Common::UString::sprintf("cursor%d", id);
		else
			res.name = remap[id];

		res.type  = kFileTypeCUR;
		res.index = id + 1;

		_resources.push_back(res);
	}
}

} // End of namespace Aurora
