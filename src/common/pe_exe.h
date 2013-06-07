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

/** @file common/pe_exe.h
 *  Portable executable parsing.
 */

#ifndef COMMON_PE_EXE_H
#define COMMON_PE_EXE_H

#include <map>
#include <vector>
#include "common/types.h"
#include "common/ustring.h"

namespace Common {

class SeekableReadStream;
class UString;

class PEResourceID {
public:
	PEResourceID();
	PEResourceID(UString x);
	PEResourceID(uint32 x);

	PEResourceID &operator=(UString string);
	PEResourceID &operator=(uint32 x);

	bool operator==(const UString &x) const;
	bool operator==(const uint32 &x) const;
	bool operator==(const PEResourceID &x) const;

	bool operator<(const PEResourceID &x) const;

	UString getString() const;
	uint32 getID() const;
	UString toString() const;

private:
	/** An ID Type. */
	enum IDType {
		kIDTypeNull,      ///< No type set
		kIDTypeNumerical, ///< A numerical ID.
		kIDTypeString     ///< A string ID.
	} _idType;

	UString _name;        ///< The resource's string ID.
	uint32 _id;           ///< The resource's numerical ID.
};

/** The default Windows PE resources. */
enum PEResourceType {
	kPECursor = 0x1,
	kPEBitmap = 0x2,
	kPEIcon = 0x3,
	kPEMenu = 0x4,
	kPEDialog = 0x5,
	kPEString = 0x6,
	kPEFontDir = 0x7,
	kPEFont = 0x8,
	kPEAccelerator = 0x9,
	kPERCData = 0xA,
	kPEMessageTable = 0xB,
	kPEGroupCursor = 0xC,
	kPEGroupIcon = 0xE,
	kPEVersion = 0x10,
	kPEDlgInclude = 0x11,
	kPEPlugPlay = 0x13,
	kPEVXD = 0x14,
	kPEAniCursor = 0x15,
	kPEAniIcon = 0x16,
	kPEHTML = 0x17,
	kPEManifest = 0x18
};

/**
 * A class able to load resources from a Windows Portable Executable, such
 * as cursors, bitmaps, and sounds.
 */
class PEResources {
public:
	PEResources();
	~PEResources();

	/** Clear all information. */
	void clear();

	/** Load from a stream. */
	bool loadFromEXE(SeekableReadStream *stream);

	/** Return a list of resource types. */
	const std::vector<PEResourceID> getTypeList() const;

	/** Return a list of names for a given type. */
	const std::vector<PEResourceID> getNameList(const PEResourceID &type) const;

	/** Return a list of languages for a given type and name. */
	const std::vector<PEResourceID> getLangList(const PEResourceID &type, const PEResourceID &name) const;

	/** Return a stream to the specified resource, taking the first language found (or 0 if non-existent). */
	SeekableReadStream *getResource(const PEResourceID &type, const PEResourceID &name);

	/** Return a stream to the specified resource (or 0 if non-existent). */
	SeekableReadStream *getResource(const PEResourceID &type, const PEResourceID &name, const PEResourceID &lang);

private:
	struct Section {
		uint32 virtualAddress;
		uint32 size;
		uint32 offset;
	};

	std::map<UString, Section> _sections;

	SeekableReadStream *_exe;

	void parseResourceLevel(Section &section, uint32 offset, int level);
	PEResourceID _curType, _curName, _curLang;

	struct Resource {
		uint32 offset;
		uint32 size;
	};

	typedef std::map<PEResourceID, Resource> LangMap;
	typedef std::map<PEResourceID,  LangMap> NameMap;
	typedef std::map<PEResourceID,  NameMap> TypeMap;

	TypeMap _resources;
};

} // End of namespace Common

#endif
