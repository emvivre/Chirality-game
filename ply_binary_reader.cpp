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

#include "ply_binary_reader.h"
#include <fstream>
#include <sstream>
#include "regex.hh"

static std::vector<QVector3D> normal_compute(const std::vector<QVector3D>& coord, const std::vector<int>& triangles)
{
    std::vector< std::pair<int,QVector3D> > normals_map;
	for ( unsigned int i = 0; i < coord.size(); i++ ) {
        normals_map.push_back(std::make_pair(0, QVector3D(0,0,0)));
	}
	for ( unsigned i = 0; i < triangles.size(); i+= 3) {
        QVector3D v0 = coord[triangles[i+1]] - coord[triangles[i]];
        v0.normalize();
        QVector3D v1 = coord[triangles[i+2]] - coord[triangles[i]];
        v1.normalize();
        QVector3D n = QVector3D::crossProduct(v0, v1);
		for ( int j = 0; j < 3; j++ ) {
			normals_map[triangles[i+j]].first++;
			normals_map[triangles[i+j]].second += n;
		}
	}
    std::vector<QVector3D> normals;
	for ( auto& e : normals_map ) {
		e.second /= e.first;
        e.second.normalize();
		normals.push_back(e.second);
	}
	return normals;
}

namespace PlyBinaryReader
{	
	struct Data {
        std::vector<QVector3D>* coord;
		std::vector<int>* triangles;
        std::vector<QVector3D>* colors;
        std::vector<QVector3D>* normals;
        Data(std::vector<QVector3D>* coord, std::vector<int>* triangles, std::vector<QVector3D>* colors, std::vector<QVector3D>* normals) : coord(coord), triangles(triangles), colors(colors), normals(normals) {}
	};
    void read(std::istream& f, const Data& d) {
		bool read_triangles = d.triangles != NULL;
		bool read_colors = d.colors != NULL;
		bool read_normals = d.normals != NULL;				
		std::string line;
		std::vector<std::string> r;
		bool is_binary = false;
		int nb_element = 0;
		int nb_face = 0;
		bool describing_faces = false;		
		bool has_coord = false;
		bool has_normal = false;
		bool has_colors = false;
		while(std::getline(f, line)) {
			if ( line == "format binary_little_endian 1.0" ) is_binary = true;
            else if ( line == "property uchar red" || line == "property uchar green" || line == "property uchar blue") has_colors = true;
			else if ( line == "property float x" || line == "property float y" || line == "property float z" ) has_coord = true; 
			else if ( line == "property float nx" || line == "property float ny" || line == "property float nz" ) has_normal = true; 
			else if ( line == "end_header") break;
			else if ( (r = Regex::search("^element vertex (.*)", line)).size() > 0 ) { 
				nb_element = atoi(r[0].c_str());
			}
			else if ( (r = Regex::search("^element face (.*)", line)).size() > 0 ) {
				nb_face = atoi(r[0].c_str());
				describing_faces = true;
			}
			else if ( describing_faces == true && line != "property list uchar int vertex_indices" ) throw UnknowFacesMetadata();
		}
		if ( read_triangles == true && nb_face == 0 ) throw FaceMissing();
		if ( has_coord == false ) throw CoordMissing();
		if ( read_colors == true && has_colors == false ) throw ColorMissing();
		if ( is_binary == false ) throw UnknowFormat();        
        for ( int i = 0; i < nb_element; i++ ) {
			float x, y, z; 
			f.read((char*)&x, sizeof x);
			f.read((char*)&y, sizeof y);
			f.read((char*)&z, sizeof z);
            QVector3D c(x, y, z);
			d.coord->push_back(c);
            if ( has_colors == true ) {
                unsigned char r, g, b;
                f.read((char*)&r, sizeof r);
                f.read((char*)&g, sizeof r);
                f.read((char*)&b, sizeof r);
                QVector3D color(r/255., g/255., b/255.);
                d.colors->push_back(color);
            }
			if ( has_normal == true ) {
				f.read((char*)&x, sizeof x);
				f.read((char*)&y, sizeof y);
				f.read((char*)&z, sizeof z);
                QVector3D n(x,y,z);
				d.normals->push_back(n);
			}
		}
		for ( int i = 0; i < nb_face; i++ ) {
			unsigned char nb_vtx; f.read((char*)&nb_vtx, sizeof nb_vtx);            
			if ( nb_vtx != 3 ) throw NotTriangularFace();
			int v0, v1, v2; 
			f.read((char*)&v0, sizeof v0);
			f.read((char*)&v1, sizeof v1);
			f.read((char*)&v2, sizeof v2);
			d.triangles->push_back(v0);
			d.triangles->push_back(v1);
			d.triangles->push_back(v2);
		}
		if ( read_normals == true && has_normal == false ) {
			*d.normals = normal_compute(*d.coord, *d.triangles);
		}
	}
    void read(std::string data, std::vector<QVector3D>& coord, std::vector<int>& triangles, std::vector<QVector3D>& colors, std::vector<QVector3D>& normals) {
        std::stringstream ss(data);
        read(ss, Data(&coord, &triangles, &colors, &normals));
	}	
}
