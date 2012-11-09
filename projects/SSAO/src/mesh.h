#ifndef _mesh_h_
#define _mesh_h_

#include <string>
#include "util.h"
#include <fstream>
using namespace std;

class Mesh {
private:
	struct Vertex {
		vec4 position;
		vec2 texcoords;
		vec3 normal;
	};
public:
	Mesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void LoadConvertedObj(string fpath);
	void Draw(); 
private:

	int _vertexCount;
	int _indexCount;
	ID3D11Buffer* _vb;
	ID3D11Buffer* _ib;



	ID3D11Device* _device;
	ID3D11DeviceContext* _deviceContext;

};

#endif