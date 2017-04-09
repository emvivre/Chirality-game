/**
 *  Copyright: (c) 2014 François Lozes <emvivre@urdn.com.ua>
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

/*
  ===========================================================================

  Copyright (C) 2014 Emvivre

  This file is part of UTIL.

  UTIL is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  UTIL is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with UTIL.  If not, see <http://www.gnu.org/licenses/>.

  ===========================================================================
*/

#ifndef _PLY_BINARY_READER_H_
#define _PLY_BINARY_READER_H_

#include <QVector3D>
#include <vector>
#include <string>

namespace PlyBinaryReader
{
	class UnknowFormat {};
	class UnknowFacesMetadata {};
	class NotTriangularFace {};
	class FaceMissing {};
	class ColorMissing {};
	class CoordMissing {};	
    void read(std::string data, std::vector<QVector3D>& coord, std::vector<int>& triangles, std::vector<QVector3D>& colors, std::vector<QVector3D>& normals);
}

#endif /* !_PLY_BINARY_READER_H_ */
