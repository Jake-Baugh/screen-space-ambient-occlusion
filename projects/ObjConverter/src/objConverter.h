#ifndef _objconverter_h_
#define _objconverter_h_

#include <string>
#include <fstream>
#include <vector>
#include "util.h"
using namespace std;

struct Vertex {
	vec4 position;
	vec2 texcoords;
	vec3 normal;
};

class ObjConverter {
public:
	ObjConverter(string output_dir);

	void ConvertObj(string objfpath, bool right_handed);
private:
	void Convert(string objfpath, int faceCount);
private:
	string _outputDir;

	vector<vec4> _v;
	vector<vec2> _vt;
	vector<vec3> _vn;
	vector<Vertex> _vertices;
	vector<unsigned int> _indices;
	vector<unsigned int> _vInd;
	vector<unsigned int> _vtInd;
	vector<unsigned int> _vnInd;
	
};

#endif