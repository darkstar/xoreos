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

/** @file common/foxpro.h
 *  A database in FoxPro format.
 */

#ifndef COMMON_FOXPRO_H
#define COMMON_FOXPRO_H

#include <vector>
#include <list>

#include <boost/date_time/gregorian/gregorian_types.hpp>

#include "common/types.h"
#include "common/ustring.h"

namespace Common {

class SeekableReadStream;
class WriteStream;

/** A database in FoxPro 2.0 format. */
class FoxPro {
public:
	/** A field type. */
	enum Type {
		kTypeDouble   = 0x42, // 'B'
		kTypeString   = 0x43, // 'C'
		kTypeDate     = 0x44, // 'D'
		kTypeFloat    = 0x46, // 'F'
		kTypeGeneral  = 0x47, // 'G'
		kTypeInteger  = 0x49, // 'I'
		kTypeBool     = 0x4C, // 'L'
		kTypeMemo     = 0x4D, // 'M'
		kTypeNumber   = 0x4E, // 'N'
		kTypePicture  = 0x50, // 'P'
		kTypeDateTime = 0x54, // 'T'
		kTypeCurrency = 0x59  // 'Y'
	};

	/** A field description. */
	struct Field {
		UString name;

		Type   type;
		uint32 offset;
		uint8  size;
		uint8  decimals;

		byte flags;

		uint32 autoIncNext;
		uint8  autoIncStep;
	};

	/** A record. */
	struct Record {
		bool deleted; ///< Has this record been deleted?
		std::vector<const byte *> fields; ///< Raw field data.
	};

	FoxPro();
	~FoxPro();

	void load(SeekableReadStream *dbf, SeekableReadStream *cdx = 0,
	          SeekableReadStream *fpt = 0);
	void save(WriteStream *dbf, WriteStream *cdx = 0, WriteStream *fpt = 0) const;

	boost::gregorian::date getLastUpdate() const;

	bool hasIndex() const;
	bool hasMemo () const;

	uint32 getFieldCount () const;
	uint32 getRecordCount() const;

	const std::vector<Field>  &getFields () const;
	const std::vector<Record> &getRecords() const;

	UString getString(const Record &record, uint32 field) const;
	int32   getInt   (const Record &record, uint32 field) const;
	bool    getBool  (const Record &record, uint32 field) const;
	double  getDouble(const Record &record, uint32 field) const;

	boost::gregorian::date getDate(const Record &record, uint32 field) const;

	SeekableReadStream *getMemo(const Record &record, uint32 field) const;


	void deleteRecord(uint32 record);

	uint32 addFieldString(const UString &name, uint8 size);
	uint32 addFieldNumber(const UString &name, uint8 size, uint8 decimals);
	uint32 addFieldInt   (const UString &name);
	uint32 addFieldBool  (const UString &name);
	uint32 addFieldDate  (const UString &name);
	uint32 addFieldMemo  (const UString &name);

	uint32 addRecord();


	void setString(uint32 record, uint32 field, const UString &value);
	void setInt   (uint32 record, uint32 field, int32 value);
	void setBool  (uint32 record, uint32 field, bool value);
	void setDouble(uint32 record, uint32 field, double value);

	void setDate(uint32 record, uint32 field, const boost::gregorian::date &value);

	void setMemo(uint32 record, uint32 field, SeekableReadStream *value = 0);

private:
	boost::gregorian::date _lastUpdate;

	bool _hasIndex;
	bool _hasMemo;

	std::vector<Field>  _fields;
	std::vector<Record> _records;

	std::list<byte *> _pool;

	uint16 _memoBlockSize;
	std::vector<byte *> _memos;

	// Loading helpers
	void loadHeader (SeekableReadStream &dbf, uint32 &recordSize, uint32 &recordCount,
	                 uint32 &firstRecordPos);
	void loadFields (SeekableReadStream &dbf, uint32 recordSize);
	void loadRecords(SeekableReadStream &dbf, uint32 recordSize, uint32 recordCount);
	void loadMemos  (SeekableReadStream &fpt);

	// Saving helpers
	void saveHeader (WriteStream &dbf) const;
	void saveFields (WriteStream &dbf) const;
	void saveRecords(WriteStream &dbf) const;
	void saveMemos  (WriteStream &fpt) const;

	void addField(uint8 size);
	void updateUpdate();

	static bool getInt(const byte *data, uint32 size, int32 &i);
};

} // End of namespace Common

#endif // COMMON_FOXPRO_H
