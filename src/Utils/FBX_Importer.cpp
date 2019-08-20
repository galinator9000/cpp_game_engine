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

	for (int polygonIndex = 0; polygonIndex < mesh->GetPolygonCount(); polygonIndex++){
		// Process every vertex in this polygon (triangle)
		for (int vertexIndex = 0; vertexIndex < 3; vertexIndex++) {
			int controlPointIndex = mesh->GetPolygonVertex(polygonIndex, vertexIndex);
			_vertices->push_back(
				{
					// Position of the Vertex
					{
						// Swap Y and Z axes.
						(float) fbxVertices[controlPointIndex].mData[0],
						(float) fbxVertices[controlPointIndex].mData[2],
						(float) fbxVertices[controlPointIndex].mData[1]
					},
					// Fill normals and colors of the Vertex with zeros temporarily.
					{
						0,0,0
					},
					// Fill colors with white as default.
					{
						1.0f, 1.0f, 1.0f, 1.0f
					}
				}
			);
		}
	}

	// Collect all indices.
	for (int polygonIndex = 0; polygonIndex < mesh->GetPolygonCount(); polygonIndex++) {
		// Process every vertex in this polygon (triangle)
		for (int vertexIndex = 0; vertexIndex < 3; vertexIndex++) {
			_indices->push_back(
				(unsigned int) polygonIndex * 3 + (2-vertexIndex)
			);
		}
	}

	// Collect normals from the mesh.
	FbxGeometryElementNormal* fbxNormalElement = mesh->GetElementNormal(0);
	FbxLayerElement::EMappingMode fbxNormalMapMode = fbxNormalElement->GetMappingMode();
	FbxLayerElement::EReferenceMode fbxNormalReferenceMode = fbxNormalElement->GetReferenceMode();

	int indexByPolygonVertex = 0;
	switch (fbxNormalMapMode) {
		// Comments are from Autodesk FBX SDK

		// The mapping is undetermined.
		case FbxGeometryElement::eNone:
			break;

		// There will be one mapping coordinate for each surface control point/vertex.
		// UNSUPPORTED
		case FbxGeometryElement::eByControlPoint:
			break;
			/*
			for (int vertexIndex = 0; vertexIndex < mesh->GetControlPointsCount(); vertexIndex++) {
				int normalIndex = -1;

				switch (fbxNormalReferenceMode) {
				// This indicates that the mapping information for the n'th element is found in the n'th place of FbxLayerElementTemplate::mDirectArray.
				case FbxGeometryElement::eDirect:
					normalIndex = vertexIndex;
					break;

				// This indicates that the FbxLayerElementTemplate::mIndexArray contains, for the n'th element, an index in the FbxLayerElementTemplate::mDirectArray array of mapping elements.
				// eIndexToDirect is usually useful for storing eByPolygonVertex mapping mode elements coordinates.
				// Since the same coordinates are usually repeated many times, this saves spaces by storing the coordinate only one time and then referring to them with an index.
				// Materials and Textures are also referenced with this mode and the actual Material/Texture can be accessed via the FbxLayerElementTemplate::mDirectArray
				case FbxGeometryElement::eIndexToDirect:
					normalIndex = fbxNormalElement->GetIndexArray().GetAt(vertexIndex);
					break;
				}

				// Place normal to our Vertex object.
				assert(normalIndex != -1);

				_vertices->at(vertexIndex).normal.x = (float) fbxNormalElement->GetDirectArray().GetAt(normalIndex)[0];
				_vertices->at(vertexIndex).normal.y = (float) fbxNormalElement->GetDirectArray().GetAt(normalIndex)[1];
				_vertices->at(vertexIndex).normal.z = (float) fbxNormalElement->GetDirectArray().GetAt(normalIndex)[2];
			}
			*/

		// There will be one mapping coordinate for each vertex, for every polygon of which it is a part.
		// This means that a vertex will have as many mapping coordinates as polygons of which it is a part.
		case FbxGeometryElement::eByPolygonVertex:
			for (int polygonIndex = 0; polygonIndex < mesh->GetPolygonCount(); polygonIndex++) {
				// Process every vertex in this polygon (triangle)
				for (int vertexIndex = 0; vertexIndex < 3; vertexIndex++) {
					indexByPolygonVertex = polygonIndex * 3 + vertexIndex;
					int normalIndex = -1;

					switch (fbxNormalReferenceMode) {
						// This indicates that the mapping information for the n'th element is found in the n'th place of FbxLayerElementTemplate::mDirectArray.
						case FbxGeometryElement::eDirect:
							normalIndex = indexByPolygonVertex;
							break;

						// This indicates that the FbxLayerElementTemplate::mIndexArray contains, for the n'th element, an index in the FbxLayerElementTemplate::mDirectArray array of mapping elements.
						// eIndexToDirect is usually useful for storing eByPolygonVertex mapping mode elements coordinates.
						// Since the same coordinates are usually repeated many times, this saves spaces by storing the coordinate only one time and then referring to them with an index.
						// Materials and Textures are also referenced with this mode and the actual Material/Texture can be accessed via the FbxLayerElementTemplate::mDirectArray
						case FbxGeometryElement::eIndexToDirect:
							normalIndex = fbxNormalElement->GetIndexArray().GetAt(indexByPolygonVertex);
							break;
					}

					// Place normal to our Vertex object.
					assert(normalIndex != -1);

					// Swap Y and Z axes.
					_vertices->at(indexByPolygonVertex).normal.x = (float) fbxNormalElement->GetDirectArray().GetAt(normalIndex)[0];
					_vertices->at(indexByPolygonVertex).normal.y = (float) fbxNormalElement->GetDirectArray().GetAt(normalIndex)[2];
					_vertices->at(indexByPolygonVertex).normal.z = (float) fbxNormalElement->GetDirectArray().GetAt(normalIndex)[1];
				}
			}

			break;

		// There can be only one mapping coordinate for the whole polygon.
		case FbxGeometryElement::eByPolygon:
			break;

		// There will be one mapping coordinate for each unique edge in the mesh.
		// This is meant to be used with smoothing layer elements.
		case FbxGeometryElement::eByEdge:
			break;

		// There can be only one mapping coordinate for the whole surface.
		case FbxGeometryElement::eAllSame:
			break;
	}

	return true;
};