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

/** @file graphics/images/winiconimage.cpp
 *  Decoding Windows icon and cursor files (.ICO and .CUR).
 */

#include "common/util.h"
#include "common/stream.h"
#include "common/error.h"
#include "common/strutil.h"

#include "graphics/images/winiconimage.h"

namespace Graphics {

WinIconImage::WinIconImage(Common::SeekableReadStream &cur) : _hotspotX(0), _hotspotY(0) {
	_compressed = false;
	_hasAlpha   = true;
	_dataType   = kPixelDataType8;

	load(cur);
}

WinIconImage::~WinIconImage() {
}

void WinIconImage::load(Common::SeekableReadStream &cur) {
	try {

		readHeader(cur);
		readData  (cur);

		if (cur.err())
			throw Common::Exception(Common::kReadError);

	} catch (Common::Exception &e) {
		e.add("Failed reading CUR file");
		throw;
	}
}

void WinIconImage::readHeader(Common::SeekableReadStream &cur) {
	if (!cur.seek(0))
		throw Common::Exception(Common::kSeekError);

	if (cur.readUint16LE() != 0)
		throw Common::Exception("Reserved bytes != 0");

	_iconType = cur.readUint16LE();
	if (_iconType != 1 && _iconType != 2)
		throw Common::Exception("CUR/ICO resource is not a cursor/icon");

	_imageCount = cur.readUint16LE();

	if (!_imageCount)
		throw Common::Exception("No image count");
}

void WinIconImage::readData(Common::SeekableReadStream &cur) {
	// For now, use the last image (which should be the highest quality)
	// TODO: Detection of which image to use
	cur.skip((_imageCount - 1) * 16);

	uint width  = cur.readByte();
	uint height = cur.readByte();
	/* byte colorDepth = */ cur.readByte();

	// Not terrible to handle, but whatever :P
	if (width & 3)
		throw Common::Exception("Non-divisible-by-4 images not handled");

	if (width == 0)
		width = 256;
	if (height == 0)
		height = 256;

	if (cur.readByte() != 0)
		throw Common::Exception("Reserved byte != 0");

	if (_iconType == 2) {
		_hotspotX = cur.readUint16LE();
		_hotspotY = cur.readUint16LE();
	} else {
		// Icons have a 'planes' field and a 'bits per pixel' pixel
		// field, but both are unused.
		cur.readUint16LE();
		cur.readUint16LE();
	}

	/* uint32 size = */ cur.readUint32LE();
	uint32 offset = cur.readUint32LE();

	// Welcome to the cursor data. Let's ride through the Win bitmap header v3
	cur.seek(offset);

	/* uint32 headerSize = */ cur.readUint32LE();
	/* uint32 bitmapWidth = */ cur.readUint32LE();
	/* uint32 bitmapHeight = */ cur.readUint32LE();
	/* uint16 planes = */ cur.readUint16LE();
	uint16 bitsPerPixel = cur.readUint16LE();
	/* uint32 compression = */ cur.readUint32LE();
	/* uint32 imageSize = */ cur.readUint32LE();
	cur.skip(16); // Skip the rest

	// We're only using 8bpp/24bpp for now. If more is required, DrMcCoy will
	// volunteer to add it.
	if (bitsPerPixel != 8 && bitsPerPixel != 24)
		throw Common::Exception("Unhandled bpp %d", bitsPerPixel);

	const int pitch = width * (bitsPerPixel / 8);

	// Now we're at the palette. Read it in for 8bpp
	byte palette[256 * 4];

	if (bitsPerPixel == 8)
		cur.read(palette, 256 * 4);

	// The XOR map
	byte *xorMap = new byte[pitch * height];
	cur.read(xorMap, pitch * height);

	// The AND map
	const uint32 andWidth = (width + 7) / 8;
	byte *andMap = new byte[andWidth * height];
	cur.read(andMap, andWidth * height);

	_format    = kPixelFormatBGRA;
	_formatRaw = kPixelFormatRGBA8;

	_mipMaps.push_back(new MipMap);

	_mipMaps[0]->width  = width;
	_mipMaps[0]->height = height;
	_mipMaps[0]->size   = width * height * 4;
	_mipMaps[0]->data   = new byte[_mipMaps[0]->size];

	const byte *xorSrc = xorMap;
	      byte *dst    = _mipMaps[0]->data;

	for (uint32 y = 0; y < height; y++) {
		const byte *andSrc = andMap + andWidth * y;

		for (uint32 x = 0; x < width; x++) {
			if (bitsPerPixel == 8) {
				const byte pixel = *xorSrc++;

				*dst++ = palette[pixel * 4];
				*dst++ = palette[pixel * 4 + 1];
				*dst++ = palette[pixel * 4 + 2];
			} else {
				*dst++ = *xorSrc++;
				*dst++ = *xorSrc++;
				*dst++ = *xorSrc++;
			}

			*dst++ = (andSrc[x / 8] & (1 << (7 - x % 8))) ? 0 : 0xff;
		}
	}

	delete[] xorMap;
	delete[] andMap;
}

int WinIconImage::getHotspotX() const {
	return _hotspotX;
}

int WinIconImage::getHotspotY() const {
	return _hotspotY;
}

} // End of namespace Graphics
