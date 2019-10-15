#include "Mesh.h"

// Utils
#include "Utils/FBX_Importer.h"

bool Mesh::LoadFBX(const char* fileName, const char* mainMeshName) {
	std::map<int, int> _indexed_vertices;
	std::map<int, std::map<int, float>> _indexed_joint_weights;

	FBX_LoadResult fbxLoadResult = FBX_Importer::Load(
		this,
		fileName, mainMeshName,
		this->gVertices, this->gIndices,
		this->gVertexCount, this->gIndexCount,
		this->gSkeleton.gJoints, this->gSkeleton.gJointCount,
		this->gAnimations, this->gAnimationCount,
		_indexed_vertices, _indexed_joint_weights
	);

	if (!fbxLoadResult.success){
		return false;
	}

	// Setup Skeleton
	this->gSkeleton.Setup();

	// Fill joint ID's and weights on all vertices.
	int maxJointIndices[MAX_JOINT_PER_VERTEX];
	double maxJointWeights[MAX_JOINT_PER_VERTEX];
	for (unsigned int v = 0; v < this->gVertexCount; v++) {
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

		// Fill joint information on vertices.
		for (int j = 0; j < MAX_JOINT_PER_VERTEX; j++) {
			this->gVertices[v].jointIDs[j] = maxJointIndices[j];
			this->gVertices[v].jointWeights[j] = (float) maxJointWeights[j];
		}
	}

	return true;
}

void Mesh::createBoxGeometry(Vector3 gSize) {
	// 3D Cube vertices
	Vertex _vertices[] = {
		// Front (Normal -Z)
		{{ -1.0f, -1.0f, -1.0f }, {0,0}, {0, 0, -1}},
		{{ 1.0f, -1.0f, -1.0f }, {0,1}, {0, 0, -1}},
		{{ -1.0f, 1.0f, -1.0f }, {1,0}, {0, 0, -1}},
		{{ 1.0f, 1.0f, -1.0f }, {1,1}, {0, 0, -1}},

		// Back (Normal +Z)
		{{ 1.0f, -1.0f, 1.0f }, {0,0}, {0, 0, 1}},
		{{ -1.0f, -1.0f, 1.0f }, {0,1}, {0, 0, 1}},
		{{ 1.0f, 1.0f, 1.0f }, {1,0}, {0, 0, 1}},
		{{ -1.0f, 1.0f, 1.0f }, {1,1}, {0, 0, 1}},

		// Right (Normal +X)
		{{ 1.0f, -1.0f, -1.0f }, {0,0}, {1, 0, 0}},
		{{ 1.0f, -1.0f, 1.0f }, {0,1}, {1, 0, 0}},
		{{ 1.0f, 1.0f, -1.0f }, {1,0}, {1, 0, 0}},
		{{ 1.0f, 1.0f, 1.0f }, {1,1}, {1, 0, 0}},

		// Left (Normal -X)
		{{ -1.0f, -1.0f, 1.0f }, {0,0}, {-1, 0, 0}},
		{{ -1.0f, -1.0f, -1.0f }, {0,1}, {-1, 0, 0}},
		{{ -1.0f, 1.0f, 1.0f }, {1,0}, {-1, 0, 0}},
		{{ -1.0f, 1.0f, -1.0f }, {1,1}, {-1, 0, 0}},

		// Top (Normal +Y)
		{{ -1.0f, 1.0f, -1.0f }, {0,0}, {0, 1, 0}},
		{{ 1.0f, 1.0f, -1.0f }, {0,1}, {0, 1, 0}},
		{{ -1.0f, 1.0f, 1.0f }, {1,0}, {0, 1, 0}},
		{{ 1.0f, 1.0f, 1.0f }, {1,1}, {0, 1, 0}},

		// Bottom (Normal -Y)
		{{ -1.0f, -1.0f, 1.0f }, {0,0}, {0, -1, 0}},
		{{ 1.0f, -1.0f, 1.0f }, {0,1}, {0, -1, 0}},
		{{ -1.0f, -1.0f, -1.0f }, {1,0}, {0, -1, 0}},
		{{ 1.0f, -1.0f, -1.0f }, {1,1}, {0, -1, 0}},
	};
	this->gVertexCount = (UINT)std::size(_vertices);
	
	// Apply scaling.
	for (unsigned int v = 0; v < this->gVertexCount; v++) {
		_vertices[v].position = _vertices[v].position * gSize;
	}

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

// Calculates 
Vector3 Mesh::calculateBoundingBox(){
	if (this->gVertices == NULL  || this->gVertexCount <= 0) {
		return Vector3(0, 0, 0);
	}

	// Center of the mesh.
	Vector3 centerMesh;
	for (unsigned int v = 0; v < this->gVertexCount; v++) {
		centerMesh = centerMesh + this->gVertices[v].position;
	}
	centerMesh = centerMesh / (float) this->gVertexCount;

	// Most far vertex positions on both side of all axes.
	Vector3 maxDist = Vector3(0, 0, 0);
	Vector3 minDist = Vector3(0, 0, 0);
	for (unsigned int v = 0; v < this->gVertexCount; v++) {
		// Max
		if (this->gVertices[v].position.x > maxDist.x) {
			maxDist.x = this->gVertices[v].position.x;
		}
		if (this->gVertices[v].position.y > maxDist.y) {
			maxDist.y = this->gVertices[v].position.y;
		}
		if (this->gVertices[v].position.z > maxDist.z) {
			maxDist.z = this->gVertices[v].position.z;
		}

		// Min
		if (this->gVertices[v].position.x < minDist.x) {
			minDist.x = this->gVertices[v].position.x;
		}
		if (this->gVertices[v].position.y < minDist.y) {
			minDist.y = this->gVertices[v].position.y;
		}
		if (this->gVertices[v].position.z < minDist.z) {
			minDist.z = this->gVertices[v].position.z;
		}
	}

	return (maxDist - minDist);
}