#pragma once
#include <Structs.h>

const enum VECTOR_RELATION_TYPE {
	DCOPY,	// Direct copy
	COPY,	// (If modified) Direct copy
	RLTV	// (If modified) Copy relative to source
};

class VectorRelation {
public:
	unsigned int id;
	VECTOR_RELATION_TYPE type;
	bool* pSrcDataChanged;
	bool* pDstDataChanged;

	virtual void Update(){}
};

class Vector2Relation : public VectorRelation {
public:
	Vector2* pSrcVector;
	Vector2* pDstVector;
	Vector2 initialRelative;

	Vector2Relation(
		Vector2* pSrc, Vector2* pDst, VECTOR_RELATION_TYPE type, bool* pSrcDataChanged = NULL, bool* pDstDataChanged = NULL,
		bool updateInitially = false, Vector2 initialVector = Vector2(0, 0)
	) {
		this->pSrcVector = pSrc;
		this->pDstVector = pDst;
		this->type = type;
		this->pSrcDataChanged = pSrcDataChanged;
		this->pDstDataChanged = pDstDataChanged;

		if (updateInitially) {
			(*this->pDstVector) = (*this->pSrcVector) + initialVector;
			(*this->pDstDataChanged) = true;
		}
		
		this->initialRelative = (*this->pDstVector) - (*this->pSrcVector);

		this->_Update();
	}

	void Update() {
		if (this->type == VECTOR_RELATION_TYPE::DCOPY) {
			(*this->pDstVector) = (*this->pSrcVector);
			return;
		}

		this->_Update();
		return;
	}

	void _Update() {
		switch (this->type) {
			case VECTOR_RELATION_TYPE::COPY:
				(*this->pDstVector) = (*this->pSrcVector);
				break;
			case VECTOR_RELATION_TYPE::RLTV:
				(*this->pDstVector) = (*this->pSrcVector) + this->initialRelative;
				break;
		}

		(*this->pDstDataChanged) = true;
	}
};

class Vector3Relation : public VectorRelation {
public:
	Vector3* pSrcVector;
	Vector3* pDstVector;
	Vector3 initialRelative;

	Vector3Relation(
		Vector3* pSrc, Vector3* pDst, VECTOR_RELATION_TYPE type, bool* pSrcDataChanged = NULL, bool* pDstDataChanged = NULL,
		bool updateInitially = false, Vector3 initialVector = Vector3(0, 0, 0)
	) {
		this->pSrcVector = pSrc;
		this->pDstVector = pDst;
		this->type = type;
		this->pSrcDataChanged = pSrcDataChanged;
		this->pDstDataChanged = pDstDataChanged;

		if (updateInitially) {
			(*this->pDstVector) = (*this->pSrcVector) + initialVector;
			(*this->pDstDataChanged) = true;
		}
		
		this->initialRelative = (*this->pDstVector) - (*this->pSrcVector);

		this->_Update();
	}

	void Update() {
		if (this->type == VECTOR_RELATION_TYPE::DCOPY) {
			(*this->pDstVector) = (*this->pSrcVector);
			return;
		}

		this->_Update();
		return;
	}

	void _Update() {
		switch (this->type) {
			case VECTOR_RELATION_TYPE::COPY:
				(*this->pDstVector) = (*this->pSrcVector);
				break;
			case VECTOR_RELATION_TYPE::RLTV:
				(*this->pDstVector) = (*this->pSrcVector) + this->initialRelative;
				break;
		}

		(*this->pDstDataChanged) = true;
	}
};

class Vector3RelationXM : public VectorRelation {
public:
	dx::XMFLOAT3* pSrcVector;
	dx::XMFLOAT3* pDstVector;
	dx::XMFLOAT3 initialRelative;

	Vector3RelationXM(
		dx::XMFLOAT3* pSrc, dx::XMFLOAT3* pDst, VECTOR_RELATION_TYPE type, bool* pSrcDataChanged = NULL, bool* pDstDataChanged = NULL
	) {
		this->pSrcVector = pSrc;
		this->pDstVector = pDst;
		this->type = type;
		this->pSrcDataChanged = pSrcDataChanged;
		this->pDstDataChanged = pDstDataChanged;

		this->initialRelative.x = this->pDstVector->x - this->pSrcVector->x;
		this->initialRelative.y = this->pDstVector->y - this->pSrcVector->y;
		this->initialRelative.z = this->pDstVector->z - this->pSrcVector->z;

		this->_Update();
	}

	void Update() {
		if (this->type == VECTOR_RELATION_TYPE::DCOPY) {
			(*this->pDstVector) = (*this->pSrcVector);
			return;
		}

		this->_Update();
		return;
	}

	void _Update() {
		switch (this->type) {
			case VECTOR_RELATION_TYPE::COPY:
				(*this->pDstVector) = (*this->pSrcVector);
				break;
			case VECTOR_RELATION_TYPE::RLTV:
				this->pDstVector->x = this->pSrcVector->x + this->initialRelative.x;
				this->pDstVector->y = this->pSrcVector->y + this->initialRelative.y;
				this->pDstVector->z = this->pSrcVector->z + this->initialRelative.z;
				break;
		}

		(*this->pDstDataChanged) = true;
	}
};

class Vector4Relation : public VectorRelation {
public:
	Vector4* pSrcVector;
	Vector4* pDstVector;
	Vector4 initialRelative;

	Vector4Relation(
		Vector4* pSrc, Vector4* pDst, VECTOR_RELATION_TYPE type, bool* pSrcDataChanged = NULL, bool* pDstDataChanged = NULL,
		bool updateInitially = false, Vector4 initialVector = Vector4(0,0,0,0)
	) {
		this->pSrcVector = pSrc;
		this->pDstVector = pDst;
		this->type = type;
		this->pSrcDataChanged = pSrcDataChanged;
		this->pDstDataChanged = pDstDataChanged;

		if (updateInitially) {
			(*this->pDstVector) = (*this->pSrcVector) + initialVector;
			(*this->pDstDataChanged) = true;
		}
		
		this->initialRelative = (*this->pDstVector) - (*this->pSrcVector);

		this->_Update();
	}

	void Update() {
		if (this->type == VECTOR_RELATION_TYPE::DCOPY) {
			(*this->pDstVector) = (*this->pSrcVector);
			return;
		}

		this->_Update();
		return;
	}

	void _Update() {
		switch (this->type) {
			case VECTOR_RELATION_TYPE::COPY:
				(*this->pDstVector) = (*this->pSrcVector);
				break;
			case VECTOR_RELATION_TYPE::RLTV:
				(*this->pDstVector) = (*this->pSrcVector) + this->initialRelative;
				break;
		}

		(*this->pDstDataChanged) = true;
	}
};