#pragma once
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "Structs.h"
#include "Entity/Mesh/MeshDeformer/Animation/Animation.h"

// Utils
#include "fbxsdk.h"

class FBX_Importer {
public:
	static bool Load(
		const char* fileName, const char* mainMeshName,
		Vertex*& _vertices, unsigned int*& _indices,
		unsigned int& _vertices_count, unsigned int& _indices_count,
		Joint**& _joints, unsigned int& _jointCount,
		Animation**& _animations, unsigned int& _animationCount,
		std::map<int, int>& _indexed_vertices,
		std::map<int, std::map<int, float>>& _indexed_joint_weights
	);
	static void printNode(FbxNode* node, unsigned int level = 0);
	static Joint* getJointByName(const char* jointName, Joint** joints, unsigned int jointCount);
	static void Triangulate(FbxNode* node, FbxGeometryConverter& geometryConverter);
	static FbxNode* getMainMeshNode(FbxNode* node, const char* mainMeshName);

	// FBX to DX matrix/vector conversions
	static dx::XMFLOAT4X4* MatrixFBXtoDX(FbxAMatrix fbxMatrix);
};