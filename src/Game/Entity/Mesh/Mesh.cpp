#include "Mesh.h"

// Utils
#include "Utils/FBX_Importer.h"

void Mesh::Update() {
	if (this->meshDeformer != NULL) {
		this->meshDeformer->Update();
	}
}

bool Mesh::LoadFBX(const char* fileName) {
	std::vector<Vertex>* _vertices = new std::vector<Vertex>();
	std::vector<unsigned int>* _indices = new std::vector<unsigned int>();

	// If any deformer is attached to this mesh,
	// fill it's values.
	std::vector<Joint>* _joints = NULL;
	if (this->meshDeformer != NULL) {
		_joints = new std::vector<Joint>();
	}

	// Load .FBX file to our vectors.
	if (!FBX_Importer::Load(fileName, _vertices, _indices, _joints)) {
		return false;
	}

	this->gVertexCount = (UINT) _vertices->size();
	this->gIndexCount = (UINT) _indices->size();
	if (this->meshDeformer != NULL) {
		this->meshDeformer->gJointCount = (UINT) _joints->size();
	}

	// Vertices
	Vertex* vertices = new Vertex[this->gVertexCount];
	for (int v = 0; v < _vertices->size(); v++) {
		vertices[v] = _vertices->at(v);
	}
	this->gVertices = vertices;

	// Indices
	unsigned int* indices = new unsigned int[this->gIndexCount];
	for (int i = 0; i < _indices->size(); i++) {
		indices[i] = _indices->at(i);
	}
	this->gIndices = indices;

	// Joints
	if (this->meshDeformer != NULL) {
		Joint* joints = new Joint[this->meshDeformer->gJointCount];
		for (int j = 0; j < _joints->size(); j++) {
			joints[j] = _joints->at(j);
		}
		this->meshDeformer->gJoints = joints;

		// Update matrices on joints.
		this->meshDeformer->calculateMatrices();
	}

	delete _vertices;
	delete _indices;
	delete _joints;

	return true;
}

void Mesh::createBoxShape() {
	// 3D Cube vertices
	Vertex _vertices[] = {
		// Front (Normal -Z)
		{{ -1.0f, -1.0f, -1.0f }, {0, 0, -1}, {0,0}},
		{{ 1.0f, -1.0f, -1.0f }, {0, 0, -1}, {0,1}},
		{{ -1.0f, 1.0f, -1.0f }, {0, 0, -1}, {1,0}},
		{{ 1.0f, 1.0f, -1.0f }, {0, 0, -1}, {1,1}},

		// Back (Normal +Z)
		{{ 1.0f, -1.0f, 1.0f }, {0, 0, 1}, {0,0}},
		{{ -1.0f, -1.0f, 1.0f }, {0, 0, 1}, {0,1}},
		{{ 1.0f, 1.0f, 1.0f }, {0, 0, 1}, {1,0}},
		{{ -1.0f, 1.0f, 1.0f }, {0, 0, 1}, {1,1}},

		// Right (Normal +X)
		{{ 1.0f, -1.0f, -1.0f }, {1, 0, 0}, {0,0}},
		{{ 1.0f, -1.0f, 1.0f }, {1, 0, 0}, {0,1}},
		{{ 1.0f, 1.0f, -1.0f }, {1, 0, 0}, {1,0}},
		{{ 1.0f, 1.0f, 1.0f }, {1, 0, 0}, {1,1}},

		// Left (Normal -X)
		{{ -1.0f, -1.0f, 1.0f }, {-1, 0, 0}, {0,0}},
		{{ -1.0f, -1.0f, -1.0f }, {-1, 0, 0}, {0,1}},
		{{ -1.0f, 1.0f, 1.0f }, {-1, 0, 0}, {1,0}},
		{{ -1.0f, 1.0f, -1.0f }, {-1, 0, 0}, {1,1}},

		// Top (Normal +Y)
		{{ -1.0f, 1.0f, -1.0f }, {0, 1, 0}, {0,0}},
		{{ 1.0f, 1.0f, -1.0f }, {0, 1, 0}, {0,1}},
		{{ -1.0f, 1.0f, 1.0f }, {0, 1, 0}, {1,0}},
		{{ 1.0f, 1.0f, 1.0f }, {0, 1, 0}, {1,1}},

		// Bottom (Normal -Y)
		{{ -1.0f, -1.0f, 1.0f }, {0, -1, 0}, {0,0}},
		{{ 1.0f, -1.0f, 1.0f }, {0, -1, 0}, {0,1}},
		{{ -1.0f, -1.0f, -1.0f }, {0, -1, 0}, {1,0}},
		{{ 1.0f, -1.0f, -1.0f }, {0, -1, 0}, {1,1}},
	};
	this->gVertexCount = (UINT)std::size(_vertices);

	Vertex* vertices = new Vertex[this->gVertexCount];
	std::copy(_vertices, _vertices + this->gVertexCount, vertices);
	this->gVertices = vertices;

	// 3D Cube indices
	unsigned int _indices[] = {
		// Front
		0,2,1, 2,3,1,

		// Back
		4,6,5, 6,7,5,

		// Right
		8,10,9, 10,11,9,

		// Left
		12,14,13, 14,15,13,

		// Top
		16,18,17, 18,19,17,

		// Bottom
		20,22,21, 22,23,21
	};
	this->gIndexCount = (UINT)std::size(_indices);

	unsigned int* indices = new unsigned int[this->gIndexCount];
	std::copy(_indices, _indices + this->gIndexCount, indices);
	this->gIndices = indices;
}

bool Mesh::attachMeshDeformer(MeshDeformer* meshDeformer) {
	if (this->meshDeformer == NULL) {
		this->meshDeformer = meshDeformer;
		return true;
	}
	return false;
}