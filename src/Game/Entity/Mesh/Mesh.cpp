#include "Mesh.h"

// Utils
#include "Utils/FBX_Importer.h"

bool Mesh::LoadFBX(const char* fileName) {
	std::vector<Vertex>* _vertices = new std::vector<Vertex>();
	std::vector<unsigned int>* _indices = new std::vector<unsigned int>();

	// Load .FBX file to our vectors.
	if (!FBX_Importer::Load(fileName, _vertices, _indices)) {
		return false;
	}

	this->gVertexCount = (UINT)_vertices->size();
	this->gIndexCount = (UINT)_indices->size();

	Vertex* vertices = new Vertex[this->gVertexCount];
	unsigned int* indices = new unsigned int[this->gIndexCount];

	for (int v = 0; v < _vertices->size(); v++) {
		vertices[v] = _vertices->at(v);
	}
	for (int i = 0; i < _indices->size(); i++) {
		indices[i] = _indices->at(i);
	}

	this->gVertices = vertices;
	this->gIndices = indices;

	delete _vertices;
	delete _indices;

	return true;
}