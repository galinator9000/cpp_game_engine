#include "FBX_Importer.h"

FbxManager* FBX_Importer::fbxSdkManager = FbxManager::Create();
FbxImporter* FBX_Importer::fbxImporter = FbxImporter::Create(fbxSdkManager, "");

bool FBX_Importer::Load(const char* fileName, std::vector<Vertex>* _vertices, std::vector<unsigned int>* _indices) {
	// Load file.
	if (!FBX_Importer::fbxImporter->Initialize(fileName, -1, FBX_Importer::fbxSdkManager->GetIOSettings())) {
		std::ostringstream myStream;
		myStream << fileName;
		myStream << " failed to load." << "\n";
		OutputDebugStringA(myStream.str().c_str());

		return false;
	}

	// Create scene object.
	FbxScene* fbxScene = FbxScene::Create(FBX_Importer::fbxSdkManager, "scene");
	FBX_Importer::fbxImporter->Import(fbxScene);
	FBX_Importer::fbxImporter->Destroy();
	
	// Get mesh from scene object.
	FbxMesh* mesh = fbxScene->GetRootNode()->GetChild(0)->GetMesh();

	// Check if mesh contains non-triangles.
	for (int p = 0; p < mesh->GetPolygonCount(); p++) {
		if (mesh->GetPolygonSize(p) != 3) {
			std::ostringstream myStream;
			myStream << fileName;
			myStream << " contains polygons with more than 3 vertices." << "\n";
			OutputDebugStringA(myStream.str().c_str());
			return false;
		}
	}

	// Collect all vertices.
	FbxVector4* fbxVertices = mesh->GetControlPoints();

	// Method 1
	/*for (int p = 0; p < mesh->GetPolygonCount(); p++) {
		for (int v = 0; v < 3; v++) {
			int icp = mesh->GetPolygonVertex(p, v);

			_vertices->push_back(
				{
					{
						(float) fbxVertices[icp].mData[0],
						(float) fbxVertices[icp].mData[1],
						(float) fbxVertices[icp].mData[2]
					}
				}
			);
		}
	}*/

	// Method 2
	int fbxVertexCount = mesh->GetControlPointsCount();
	for(int v = 0; v < fbxVertexCount; v++) {
		_vertices->push_back(
			{
				{
					// Swap Y and Z axes
					(float) fbxVertices[v].mData[0],
					(float) fbxVertices[v].mData[2],
					(float) fbxVertices[v].mData[1]
				}
			}
		);
	}

	// Collect all indices.
	int* fbxIndices = mesh->GetPolygonVertices();
	for (int p=0; p < mesh->GetPolygonCount(); p++) {
		// Process every vertex in this polygon (triangle)
		for (int i = 0; i < 3; i++) {
			_indices->push_back(
				(unsigned int) fbxIndices[p * 3 + i]
			);
		}
	}

	return true;
};