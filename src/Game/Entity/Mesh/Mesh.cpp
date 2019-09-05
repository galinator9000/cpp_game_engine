#include "Mesh.h"

// Utils
#include "Utils/FBX_Importer.h"

bool Mesh::LoadFBX(const char* fileName) {
	std::vector<Vertex>* _vertices = new std::vector<Vertex>();
	std::vector<unsigned int>* _indices = new std::vector<unsigned int>();
	std::vector<Joint*>* _joints = new std::vector<Joint*>();
	std::vector<Animation*>* _animations = new std::vector<Animation*>();

	std::map<int, int> _indexed_vertices;
	std::map<int, std::map<int, float>> _indexed_joint_weights;

	// Load .FBX file to our vectors.
	if (!FBX_Importer::Load(fileName, _vertices, _indices, _joints, _animations, _indexed_vertices, _indexed_joint_weights)) {
		return false;
	}

	//// Process Animation.
	this->gAnimationCount = (UINT)_animations->size();
	Animation** animations = new Animation*[this->gAnimationCount];
	for (int a = 0; a < _animations->size(); a++) {
		animations[a] = _animations->at(a);
	}
	this->gAnimations = animations;

	//// Process Skeleton
	this->gSkeleton.gJointCount = (UINT) _joints->size();
	Joint** joints = new Joint*[this->gSkeleton.gJointCount];
	for (int j = 0; j < _joints->size(); j++) {
		joints[j] = _joints->at(j);
	}
	this->gSkeleton.gJoints = joints;
	this->gSkeleton.Setup();

	int maxJointIndices[MAX_JOINT_PER_VERTEX];
	double maxJointWeights[MAX_JOINT_PER_VERTEX];

	// Fill joint ID's and weights on all vertices.
	for (int v = 0; v < _vertices->size(); v++) {
		std::vector<int> addedJoints;
		addedJoints.clear();

		// Reset local arrays.
		memset(&maxJointIndices, -1, sizeof(int) * MAX_JOINT_PER_VERTEX);
		memset(&maxJointWeights, 0, sizeof(double) * MAX_JOINT_PER_VERTEX);

		// Sort joints by their weights.
		for (int j = 0; j < MAX_JOINT_PER_VERTEX; j++) {
			std::map<int, float>* jointIndexWeightPair = &(_indexed_joint_weights[_indexed_vertices[v]]);
			for (std::map<int, float>::iterator it = jointIndexWeightPair->begin();  it != jointIndexWeightPair->end(); it++) {
				if (
					it->second > maxJointWeights[j] &&
					!(std::find(addedJoints.begin(), addedJoints.end(), it->first) != addedJoints.end())
				){
					addedJoints.push_back(it->first);
					maxJointIndices[j] = it->first;
					maxJointWeights[j] = it->second;
				}
			}
		}

		// Take sum of the weights.
		double sumWeights = 0.0;
		for (int j = 0; j < MAX_JOINT_PER_VERTEX; j++) {
			sumWeights += maxJointWeights[j];
		}

		// Normalize weights.
		for (int j = 0; j < MAX_JOINT_PER_VERTEX; j++) {
			maxJointWeights[j] *= 1.0 / sumWeights;
		}

		// Fill vertex struct.
		for (int j = 0; j < MAX_JOINT_PER_VERTEX; j++) {
			_vertices->at(v).jointIDs[j] = maxJointIndices[j];
			_vertices->at(v).jointWeights[j] = (float) maxJointWeights[j];
		}
	}

	//// Process Vertices
	this->gVertexCount = (UINT)_vertices->size();
	Vertex* vertices = new Vertex[this->gVertexCount];
	for (int v = 0; v < _vertices->size(); v++) {
		vertices[v] = _vertices->at(v);
	}
	this->gVertices = vertices;

	//// Process Indices
	this->gIndexCount = (UINT)_indices->size();
	unsigned int* indices = new unsigned int[this->gIndexCount];
	for (int i = 0; i < _indices->size(); i++) {
		indices[i] = _indices->at(i);
	}
	this->gIndices = indices;

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

// General
Animation* Mesh::getAnimation(const char* animName) {
	for (unsigned int a = 0; a < this->gAnimationCount; a++) {
		if (!gAnimations[a]->name.compare(animName)) {
			return gAnimations[a];
		}
	}
	return NULL;
}