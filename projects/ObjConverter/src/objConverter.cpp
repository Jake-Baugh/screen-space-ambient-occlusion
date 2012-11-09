#include "objConverter.h"

ObjConverter::ObjConverter(string outputDir) 
	: _outputDir(outputDir) {

}

void ObjConverter::ConvertObj(string objfpath, bool right_handed) {
	int faceCount = 0;
	vec4 v(0.f, 0.f, 0.f, 1.f);
	vec2 vt(0.f, 0.f);
	vec3 vn(0.f, 0.f, 0.f);
	ifstream fin;
	fin.open(objfpath);
	if(fin.fail()) { return; }

	string key;
	while(!fin.eof()) {
		fin >> key;

		if(key == "v") {
			fin >> v.x >> v.y >> v.z;
			_v.push_back(v);
		}
		else if(key == "vt") {
			fin >> vt.x >> vt.y;
			_vt.push_back(vt);
		}
		else if(key == "vn") {
			fin >> vn.x >> vn.y >> vn.z;
			_vn.push_back(vn);
		}
		else if(key == "f") {
			char c;
			int v_index = -1;
			int vt_index = -1;
			int vn_index = -1;
			faceCount++;

			for(int i = 0; i < 3; ++i) {
				fin >> v_index;
				if(v_index < 0)			v_index = _v.size() + v_index;
				else					v_index--;
				if(!_vt.empty()) {
					fin >> c >> vt_index;
					if(vt_index < 0)	vt_index = _vt.size() + vt_index;
					else					vt_index--;
				}
				else { fin >> c; }
				if(!_vn.empty()) {
					fin >> c >> vn_index;
					if(vn_index < 0)	vn_index = _vn.size() + vn_index;
					else					vn_index--;
				}
				Vertex vert; 
				memset(&vert, 0, sizeof(Vertex));
				if(v_index != -1)	vert.position	= _v[v_index];
				if(vt_index != -1)	vert.texcoords	= _vt[vt_index];
				if(vn_index != -1)	vert.normal		= _vn[vn_index];
				_vertices.push_back(vert);
				_indices.push_back(_vertices.size() - 1);
			}
		}
		else {
			char buff[512];
			fin.getline(buff, 512);
		}
	}
	fin.close();
	Convert(objfpath, faceCount);
}

void ObjConverter::Convert(string objfpath, int faceCount) {
	string fname;
	int i = objfpath.find_last_of("/");
	if(i != string::npos) {
		int j = objfpath.find_first_of(".");
		fname = objfpath.substr(i+1, j-1);
	}

	ofstream fout;
	fout.open(_outputDir+"/"+fname+".converted", ios::binary);
	if(fout.fail()) return;
	int size = sizeof(Vertex) * _vertices.size();
	fout.write((char*)&_vertices[0], size);
	fout.close();

}