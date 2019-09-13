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
		const char* fileName,
		std::vector<Vertex>* _vertices,
		std::vector<unsigned int>* _indices,
		std::vector<Joint*>* _joints,
		std::vector<Animation*>* _animations,
		std::map<int, int>& _indexed_vertices,
		std::map<int, std::map<int, float>>& _indexed_joint_weights,
		const char* mainMeshName
	);
	static void printNode(FbxNode* node, unsigned int level = 0);
	static Joint* getJointByName(const char* jointName, std::vector<Joint*>* _joints);
	static void Triangulate(FbxNode* node, FbxGeometryConverter& geometryConverter);
	static FbxNode* getMainMeshNode(FbxNode* node, const char* mainMeshName);

	// FBX to DX matrix/vector conversions
	static dx::XMFLOAT4X4* MatrixFBXtoDX(FbxAMatrix fbxMatrix);
};