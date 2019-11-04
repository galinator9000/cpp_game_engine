#pragma once
#include "StructsBuffer.h"
#include <string>

#include "Entity/Mesh/MeshDeformer/Joint.h"
#include "Entity/Mesh/MeshDeformer/JointTransform.h"
#include "Entity/Mesh/MeshDeformer/Skeleton.h"

const enum LIGHT_TYPE {
	DIRECTIONAL_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT
};

const enum PROJECTION_TYPE {
	PERSPECTIVE,
	ORTHOGRAPHIC
};

const enum COLLISION_ACTOR_TYPE {
	COLLISION_ACTOR_STATIC,
	COLLISION_ACTOR_DYNAMIC,
	COLLISION_ACTOR_KINEMATIC,
	COLLISION_ACTOR_CCT
};

struct Vector2 {
	Vector2() {
		this->x = 0;
		this->y = 0;
	}

	Vector2(float x, float y) {
		this->x = x;
		this->y = y;
	}

	// Operators
	bool operator==(const Vector2& other) {
		return (this->x == other.x) && (this->y == other.y);
	}

	// Element wise calculations
	Vector2 operator+(const Vector2& other) {
		return Vector2(
			this->x + other.x,
			this->y + other.y
		);
	}

	Vector2 operator-(const Vector2& other) {
		return Vector2(
			this->x - other.x,
			this->y - other.y
		);
	}

	Vector2 operator-() {
		return Vector2(
			-(this->x),
			-(this->y)
		);
	}

	Vector2 operator/(const Vector2& other) {
		return Vector2(
			this->x / other.x,
			this->y / other.y
		);
	}

	Vector2 operator*(const Vector2& other) {
		return Vector2(
			this->x * other.x,
			this->y * other.y
		);
	}

	Vector2 operator*(const float& other) {
		return Vector2(
			this->x * other,
			this->y * other
		);
	}

	Vector2 operator/(const float& other) {
		return Vector2(
			this->x / other,
			this->y / other
		);
	}

	float distance(Vector2 other) {
		return this->distance(
			(*this),
			other
		);
	}

	Vector2 normalize() {
		float _max = this->maximum();
		return Vector2(
			this->x / _max,
			this->y / _max
		);
	}

	float maximum() {
		float maxVal = -INFINITY;

		if (this->x > maxVal) {
			maxVal = this->x;
		}if (this->y > maxVal) {
			maxVal = this->y;
		}

		return maxVal;
	}

	float minimum() {
		float minVal = INFINITY;

		if (this->x < minVal) {
			minVal = this->x;
		}if (this->y < minVal) {
			minVal = this->y;
		}

		return minVal;
	}

	// DirectXMath & PhysX conversions
	void storeXMFLOAT(dx::XMFLOAT2* xmfloat) {
		this->x = xmfloat->x;
		this->y = xmfloat->y;
	}
	dx::XMFLOAT2 loadXMFLOAT() {
		return dx::XMFLOAT2(
			this->x,
			this->y
		);
	}
	void storeXMVECTOR(dx::XMVECTOR* xmvector) {
		dx::XMFLOAT2 xmfloat;

		dx::XMStoreFloat2(
			&xmfloat,
			*xmvector
		);

		this->storeXMFLOAT(&xmfloat);
	}
	dx::XMVECTOR loadXMVECTOR() {
		return dx::XMVectorSet(
			this->x,
			this->y,
			0,
			0
		);
	}
	PxVec2 toPxVec2() {
		return PxVec2(this->x, this->y);
	}

	// Static functions
	static float distance(Vector2 v1, Vector2 v2) {
		dx::XMVECTOR distVector = dx::XMVector2Length(
			(v1 - v2).loadXMVECTOR()
		);
		float dist;
		dx::XMStoreFloat(
			&dist,
			distVector
		);

		return dist;
	}
	static Vector2 XAxis() { return Vector2(1, 0); }
	static Vector2 YAxis() { return Vector2(0, 1); }

	float x;
	float y;
};

struct Vector3 {
	Vector3() {
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}
	
	Vector3(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	// Operators
	bool operator==(const Vector3& other) {
		return (this->x == other.x) && (this->y == other.y) && (this->z == other.z);
	}

	// Element wise calculations
	Vector3 operator+(const Vector3& other) {
		return Vector3(
			this->x + other.x,
			this->y + other.y,
			this->z + other.z
		);
	}

	Vector3 operator-(const Vector3& other) {
		return Vector3(
			this->x - other.x,
			this->y - other.y,
			this->z - other.z
		);
	}

	Vector3 operator-() {
		return Vector3(
			-(this->x),
			-(this->y),
			-(this->z)
		);
	}

	Vector3 operator/(const Vector3& other) {
		return Vector3(
			this->x / other.x,
			this->y / other.y,
			this->z / other.z
		);
	}

	Vector3 operator*(const Vector3& other) {
		return Vector3(
			this->x * other.x,
			this->y * other.y,
			this->z * other.z
		);
	}

	Vector3 operator*(const float& other) {
		return Vector3(
			this->x * other,
			this->y * other,
			this->z * other
		);
	}

	Vector3 operator/(const float& other) {
		return Vector3(
			this->x / other,
			this->y / other,
			this->z / other
		);
	}

	float distance(Vector3 other) {
		return this->distance(
			(*this),
			other
		);
	}

	Vector3 normalize() {
		float _max = this->maximum();
		return Vector3(
			this->x / _max,
			this->y / _max,
			this->z / _max
		);
	}

	float maximum() {
		float maxVal = -INFINITY;

		if (this->x > maxVal) {
			maxVal = this->x;
		}if (this->y > maxVal) {
			maxVal = this->y;
		}if (this->z > maxVal) {
			maxVal = this->z;
		}

		return maxVal;
	}

	float minimum() {
		float minVal = INFINITY;

		if (this->x < minVal) {
			minVal = this->x;
		}if (this->y < minVal) {
			minVal = this->y;
		}if (this->z < minVal) {
			minVal = this->z;
		}

		return minVal;
	}

	// DirectXMath & PhysX conversions
	void storeXMFLOAT(dx::XMFLOAT3* xmfloat) {
		this->x = xmfloat->x;
		this->y = xmfloat->y;
		this->z = xmfloat->z;
	}
	dx::XMFLOAT3 loadXMFLOAT() {
		return dx::XMFLOAT3(
			this->x,
			this->y,
			this->z
		);
	}
	void storeXMVECTOR(dx::XMVECTOR* xmvector) {
		dx::XMFLOAT3 xmfloat;

		dx::XMStoreFloat3(
			&xmfloat,
			*xmvector
		);

		this->storeXMFLOAT(&xmfloat);
	}
	dx::XMVECTOR loadXMVECTOR() {
		return dx::XMVectorSet(
			this->x,
			this->y,
			this->z,
			0
		);
	}
	PxTransform toPxTransform() {
		return PxTransform(PxVec3(this->x, this->y, this->z));
	}
	PxVec3 toPxVec3() {
		return PxVec3(this->x, this->y, this->z);
	}

	// Static functions
	static float distance(Vector3 v1, Vector3 v2) {
		dx::XMVECTOR distVector = dx::XMVector3Length(
			(v1 - v2).loadXMVECTOR()
		);
		float dist;
		dx::XMStoreFloat(
			&dist,
			distVector
		);

		return dist;
	}
	static Vector3 XAxis() { return Vector3(1, 0, 0); }
	static Vector3 YAxis() { return Vector3(0, 1, 0); }
	static Vector3 ZAxis() { return Vector3(0, 0, 1); }

	float x;
	float y;
	float z;
};

struct Vector4 {
	Vector4() {
		this->x = 0;
		this->y = 0;
		this->z = 0;
		this->w = 0;
	}

	Vector4(float x, float y, float z, float w) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	// Operators
	bool operator==(const Vector4& other) {
		return (this->x == other.x) && (this->y == other.y) && (this->z == other.z) && (this->w == other.w);
	}

	// Element wise calculations
	Vector4 operator+(const Vector4& other) {
		return Vector4(
			this->x + other.x,
			this->y + other.y,
			this->z + other.z,
			this->w + other.w
		);
	}

	Vector4 operator-(const Vector4& other) {
		return Vector4(
			this->x - other.x,
			this->y - other.y,
			this->z - other.z,
			this->w - other.w
		);
	}

	Vector4 operator-() {
		return Vector4(
			-(this->x),
			-(this->y),
			-(this->z),
			-(this->w)
		);
	}

	Vector4 operator/(const Vector4& other) {
		return Vector4(
			this->x / other.x,
			this->y / other.y,
			this->z / other.z,
			this->w / other.w
		);
	}

	Vector4 operator*(const Vector4& other) {
		return Vector4(
			this->x * other.x,
			this->y * other.y,
			this->z * other.z,
			this->w * other.w
		);
	}

	Vector4 operator*(const float& other) {
		return Vector4(
			this->x * other,
			this->y * other,
			this->z * other,
			this->w * other
		);
	}

	Vector4 operator/(const float& other) {
		return Vector4(
			this->x / other,
			this->y / other,
			this->z / other,
			this->w / other
		);
	}

	float distance(Vector4 other) {
		return this->distance(
			(*this),
			other
		);
	}

	Vector4 normalize() {
		float _max = this->maximum();
		return Vector4(
			this->x / _max,
			this->y / _max,
			this->z / _max,
			this->w / _max
		);
	}

	float maximum() {
		float maxVal = -INFINITY;

		if (this->x > maxVal) {
			maxVal = this->x;
		}if (this->y > maxVal) {
			maxVal = this->y;
		}if (this->z > maxVal) {
			maxVal = this->z;
		}if (this->w > maxVal) {
			maxVal = this->w;
		}

		return maxVal;
	}

	float minimum() {
		float minVal = INFINITY;

		if (this->x < minVal) {
			minVal = this->x;
		}if (this->y < minVal) {
			minVal = this->y;
		}if (this->z < minVal) {
			minVal = this->z;
		}if (this->w < minVal) {
			minVal = this->w;
		}

		return minVal;
	}

	// DirectXMath & PhysX conversions
	void storeXMFLOAT(dx::XMFLOAT4* xmfloat) {
		this->x = xmfloat->x;
		this->y = xmfloat->y;
		this->z = xmfloat->z;
		this->w = xmfloat->w;
	}
	dx::XMFLOAT4 loadXMFLOAT() {
		return dx::XMFLOAT4(
			this->x,
			this->y,
			this->z,
			this->w
		);
	}
	void storeXMVECTOR(dx::XMVECTOR* xmvector) {
		dx::XMFLOAT4 xmfloat;

		dx::XMStoreFloat4(
			&xmfloat,
			*xmvector
		);

		this->storeXMFLOAT(&xmfloat);
	}
	dx::XMVECTOR loadXMVECTOR() {
		return dx::XMVectorSet(
			this->x,
			this->y,
			this->z,
			this->w
		);
	}
	PxVec4 toPxVec4() {
		return PxVec4(this->x, this->y, this->z, this->w);
	}
	PxQuat toPxQuat() {
		return PxQuat(this->x, this->y, this->z, this->w);
	}

	// Static functions
	static float distance(Vector4 v1, Vector4 v2) {
		dx::XMVECTOR distVector = dx::XMVector4Length(
			(v1 - v2).loadXMVECTOR()
		);
		float dist;
		dx::XMStoreFloat(
			&dist,
			distVector
		);

		return dist;
	}
	static Vector4 XAxis() { return Vector4(1, 0, 0, 0); }
	static Vector4 YAxis() { return Vector4(0, 1, 0, 0); }
	static Vector4 ZAxis() { return Vector4(0, 0, 1, 0); }
	static Vector4 WAxis() { return Vector4(0, 0, 0, 1); }

	float x;
	float y;
	float z;
	float w;
};

struct Color {
	Color() {
		// Default: White.
		this->r = 1.0f;
		this->g = 1.0f;
		this->b = 1.0f;
		this->a = 1.0f;
	}

	Color(float r, float g, float b, float a=1.0f) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	// Operators
	bool operator==(const Color& other) {
		return (this->r == other.r) && (this->g == other.g) && (this->b == other.b) && (this->a == other.a);
	}

	// DirectXMath & PhysX conversions
	dx::XMFLOAT4 loadXMFLOAT() {
		return dx::XMFLOAT4(
			this->r,
			this->g,
			this->b,
			this->a
		);
	}

	float r;
	float g;
	float b;
	float a;
};

// Each vertex on every mesh is represented by this struct.
struct Vertex {
	Vector3 position;
	Vector2 textureUV;
	Vector3 normal;
	Vector3 tangent;
	Vector3 binormal;

	// Joint ID and corresponding weights for vertex
	float jointWeights[MAX_JOINT_PER_VERTEX];
	int jointIDs[MAX_JOINT_PER_VERTEX];
};

// Entity related structs.
struct Material {
	Material(
		Color color = Color(1, 1, 1, 1),
		float specularIntensity = 0.18f,
		float specularPower = 2.0f,
		Color specularHighlightColor = Color(0.18f, 0.18f, 0.18f)
	) {
		this->color = color;
		this->specularHighlightColor = specularHighlightColor;
		this->specularIntensity = specularIntensity;
		this->specularPower = specularPower;
	}

	Color color;
	Color specularHighlightColor;
	float specularIntensity;
	float specularPower;
};

struct CollisionMaterial {
	CollisionMaterial(
		float staticFriction = 1.0f,
		float dynamicFriction = 1.0f,
		float restitution = 0.0f,
		float density = 10.0f
	) {
		this->staticFriction = staticFriction;
		this->dynamicFriction = dynamicFriction;
		this->restitution = restitution;
		this->density = density;
	}

	float staticFriction = 1.0f;
	float dynamicFriction = 1.0f;
	float restitution = 0.0f;
	float density = 10.0f;
};

// Used for creating entities.
struct EntityProperties {
	// Default properties of an entity.
	EntityProperties(
		Vector3 size = Vector3(1, 1, 1), Vector3 position = Vector3(0, 0, 0), Vector4 rotationQ = Vector4(1, 0, 0, 0),
		Material material = Material(), CollisionMaterial collisionMaterial=CollisionMaterial(),
		void* pMesh = NULL,
		void* pCollisionShape = NULL,
		void* pMollisionActor = NULL
	) {
		this->size = size;
		this->position = position;
		// Check quaternion vector.
		if (rotationQ == Vector4(0, 0, 0, 0)) {
			rotationQ.x = 1.0f;
		}
		this->rotationQ = rotationQ;
		this->material = material;
		this->pMesh = pMesh;
		this->pCollisionShape = pCollisionShape;
		this->pCollisionActor = pMollisionActor;
		this->collisionMaterial = collisionMaterial;
	}

	Vector3 size = Vector3(1,1,1);
	Vector3 position = Vector3(0,0,0);
	Vector4 rotationQ = Vector4(1,0,0,0);
	Material material = Material();
	CollisionMaterial collisionMaterial;
	void* pMesh = NULL;
	void* pCollisionShape = NULL;
	void* pCollisionActor = NULL;
};

// Used for creating characters.
struct CharacterProperties {
	// Default properties of an haracter.
	CharacterProperties(float movementSpeed = 0.1f, Vector3 facingDirection = {0, 0, 1}) {
		this->movementSpeed = movementSpeed;
		this->facingDirection = facingDirection;
	}

	float movementSpeed = 0.1f;
	Vector3 facingDirection = Vector3(0, 0, 1);
};