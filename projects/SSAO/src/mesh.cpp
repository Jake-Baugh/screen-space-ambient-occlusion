#include "mesh.h"
using namespace std;

Mesh::Mesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
: _device(device), _deviceContext(deviceContext) 
{}

void Mesh::LoadConvertedObj(string fpath) {
	ifstream fin;
	fin.open(fpath, ios::binary);
	if(fin.fail()) return;

	fin.seekg(0, ios::end);
	int filelen = (int)fin.tellg();
	fin.seekg(0, ios::beg);

	_vertexCount = filelen / sizeof(Vertex);
	_indexCount = _vertexCount;

	Vertex* verts = new Vertex[_vertexCount];

	fin.read((char*)verts, filelen);
	fin.close();

	unsigned int* indices = new unsigned int[_indexCount];
	for(int i = 0; i < _indexCount; ++i) indices[i] = i;


	HRESULT hret;
	D3D11_BUFFER_DESC vbd;
	memset(&vbd, 0, sizeof(D3D11_BUFFER_DESC));
	vbd.ByteWidth = sizeof(Vertex) * _vertexCount;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vsrd;
	memset(&vsrd, 0, sizeof(D3D11_SUBRESOURCE_DATA));
	vsrd.pSysMem = verts;

	D3D11_BUFFER_DESC ibd;
	memset(&ibd, 0, sizeof(D3D11_BUFFER_DESC));
	ibd.ByteWidth = sizeof(unsigned int) * _indexCount;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA isrd;
	memset(&isrd, 0, sizeof(D3D11_SUBRESOURCE_DATA));
	isrd.pSysMem = indices;

	hret = _device->CreateBuffer(&vbd, &vsrd, &_vb);
	if(FAILED(hret)) {}

	hret = _device->CreateBuffer(&ibd, &isrd, &_ib);
	if(FAILED(hret)) {}

	delete [] verts; verts = 0;
	delete [] indices; indices = 0;
}

void Mesh::Draw() {
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	_deviceContext->IASetVertexBuffers(0, 1, &_vb, &stride, &offset);
	_deviceContext->IASetIndexBuffer(_ib, DXGI_FORMAT_R32_UINT, 0);

	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_deviceContext->DrawIndexed(_indexCount, 0, 0);
	
}