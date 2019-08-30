#include "FBX_Importer.h"

bool FBX_Importer::Load(
		const char* fileName,
		std::vector<Vertex>* _vertices,
		std::vector<unsigned int>* _indices,
		std::vector<Joint>* _joints
	)
{
	FbxManager* fbxSdkManager = FbxManager::Create();
	FbxImporter* fbxImporter = FbxImporter::Create(fbxSdkManager, "");

	// Status object for holding errors.
	FbxStatus status;

	// Load file.
	if (!fbxImporter->Initialize(fileName, -1, fbxSdkManager->GetIOSettings())) {
		std::ostringstream myStream;
		myStream << fileName;
		myStream << " failed to load." << "\n";
		OutputDebugStringA(myStream.str().c_str());
		std::cout << myStream.str().c_str();

		return false;
	}

	// Create scene object.
	FbxScene* fbxScene = FbxScene::Create(fbxSdkManager, "scene");
	fbxImporter->Import(fbxScene);
	fbxImporter->Destroy();

	//// Process scene.
	FbxNode* rootNode = fbxScene->GetRootNode();
	// Walk scene and print out all nodes.
	FBX_Importer::printNode(rootNode);
	
	// Check if any mesh exist on current scene.
	int meshChildNodeIndex = -1;
	for (int c = 0; c < rootNode->GetChildCount(); c++) {
		if (rootNode->GetChild(c)->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh) {
			meshChildNodeIndex = c;
			break;
		}
	}

	if (meshChildNodeIndex == -1) {
		std::ostringstream myStream;
		myStream << fileName;
		myStream << ": There isn't a mesh node in current scene." << "\n";
		OutputDebugStringA(myStream.str().c_str());
		std::cout << myStream.str().c_str();
		return false;
	}

	//// Process mesh.
	// Get mesh from scene object.
	FbxNode* baseNode = rootNode->GetChild(0);
	FbxNode* meshNode = rootNode->GetChild(meshChildNodeIndex);
	FbxMesh* mesh = meshNode->GetMesh();

	// Check if mesh contains non-triangles.
	for (int p = 0; p < mesh->GetPolygonCount(); p++) {
		if (mesh->GetPolygonSize(p) != 3) {
			std::ostringstream myStream;
			myStream << fileName;
			myStream << " contains polygons with more than 3 vertices." << "\n";
			OutputDebugStringA(myStream.str().c_str());
			std::cout << myStream.str().c_str();
			return false;
		}
	}

	//// Process the deformer of the mesh. (Skeleton)
	int meshDeformerCount = mesh->GetDeformerCount();

	if (meshDeformerCount > 0) {
		FbxDeformer* meshDeformer = mesh->GetDeformer(0);
		FbxDeformer::EDeformerType meshDeformerType = meshDeformer->GetDeformerType();

		// Check mesh deformer type if it is supported.
		if (meshDeformerType == FbxDeformer::eSkin) {
			FbxSkin* skin = (FbxSkin*) mesh->GetDeformer(0, FbxDeformer::eSkin, &status);
			int skinClusterCount = skin->GetClusterCount();
			FbxCluster* skinCluster;

			FbxNode* linkNode;
			FbxCluster::ELinkMode linkMode;

			// Process each cluster.
			for (int c=0; c < skinClusterCount; c++) {
				skinCluster = skin->GetCluster(c);

				// Get joint of the cluster.
				linkNode = skinCluster->GetLink();
				linkMode = skinCluster->GetLinkMode();

				switch (linkMode) {
					case FbxCluster::eNormalize:


						break;
					case FbxCluster::eAdditive: break;
					case FbxCluster::eTotalOne: break;
				}
			}

		}else {
			/*
			Other mesh deformer types are:
				FbxDeformer::eBlendShape:
				FbxDeformer::eUnknown:
				FbxDeformer::eVertexCache:
			*/

			std::ostringstream myStream;
			myStream << fileName;
			myStream << ": Mesh deformer found but deformer type is not supported." << "\n";
			OutputDebugStringA(myStream.str().c_str());
			std::cout << myStream.str().c_str();
		}
	}else {
		std::ostringstream myStream;
		myStream << fileName;
		myStream << ": There isn't any mesh deformer binded to mesh." << "\n";
		OutputDebugStringA(myStream.str().c_str());
		std::cout << myStream.str().c_str();
	}

	//// Process all vertices, indices, normals (per-vertex), and UV values from the mesh.
	// Get values that indicates how normals are represented in the mesh.
	// If faces of the mesh shares vertices, that means one vertex has more than one normal vector.
	// If that's the case, we duplicate the vertices and make faces independent from each other.
	FbxGeometryElementNormal* fbxNormalElement = mesh->GetElementNormal();
	FbxLayerElement::EMappingMode fbxNormalMapMode = fbxNormalElement->GetMappingMode();
	FbxLayerElement::EReferenceMode fbxNormalReferenceMode = fbxNormalElement->GetReferenceMode();

	// Get vertices (control points).
	FbxVector4* fbxVertices = mesh->GetControlPoints();

	// Get first UV set.
	FbxStringList uvNames;
	mesh->GetUVSetNames(uvNames);
	const char* uvName = uvNames[0];

	switch (fbxNormalMapMode) {
		// Comments are from Autodesk FBX SDK

		// The mapping is undetermined.
		case FbxGeometryElement::eNone:
			return false;
			break;

		// There will be one mapping coordinate for each surface control point/vertex.
		case FbxGeometryElement::eByControlPoint:
			break;

		// There will be one mapping coordinate for each vertex, for every polygon of which it is a part.
		// This means that a vertex will have as many mapping coordinates as polygons of which it is a part.
		case FbxGeometryElement::eByPolygonVertex:
			// Collect all vertices.
			for (int polygonIndex = 0; polygonIndex < mesh->GetPolygonCount(); polygonIndex++) {
				// Process every vertex in this polygon (triangle)
				for (int vertexIndex = 0; vertexIndex < 3; vertexIndex++) {

					// Get the control point index of the current vertex.
					int controlPointIndex = mesh->GetPolygonVertex(polygonIndex, vertexIndex);

					// Get the UV coordinates of the current vertex for texture.
					FbxVector2 texture_UV;
					bool unmapped;
					bool result = mesh->GetPolygonVertexUV(polygonIndex, vertexIndex, uvName, texture_UV, unmapped);

					float texture_U = 0;
					float texture_V = 0;

					if (result && !unmapped) {
						texture_U = (float) texture_UV.mData[0];
						texture_V = 1.0f - (float) texture_UV.mData[1];
					}

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
							// Texture UV coordinates of the Vertex
							{
								texture_U, texture_V
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
						(unsigned int)polygonIndex * 3 + (2 - vertexIndex)
					);
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

	int indexByPolygonVertex = 0;
	switch (fbxNormalMapMode) {
		// Comments are from Autodesk FBX SDK

		// The mapping is undetermined.
		case FbxGeometryElement::eNone:
			break;

		// There will be one mapping coordinate for each surface control point/vertex.
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

// Walks the node child-by-child and works recursively.
// Function simply prints out name and child count of the node.
void FBX_Importer::printNode(FbxNode* node, unsigned int level) {
	std::ostringstream myStream;

	if (level > 0) {
		myStream << '^' << std::string(level, '-');
	}
	myStream << node->GetName() << ", " << node->GetChildCount() << "\n";

	OutputDebugStringA(myStream.str().c_str());
	std::cout << myStream.str().c_str();

	for (int c=0; c < node->GetChildCount(); c++) {
		FBX_Importer::printNode(node->GetChild(c), level+1);
	}
}