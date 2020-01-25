#include "Mesh.h"

Mesh::Mesh(const Mesh &other) 
	: mVertexBuffers(other.mVertexBuffers), mIndexBuffer(other.mIndexBuffer), mVertexCount(other.mVertexCount), mStrides(other.mStrides), mAttributeMapping(other.mAttributeMapping), mMapIndex(other.mMapIndex), mReferenceCount(other.mReferenceCount)
{
	mReferenceCount->Increment();
}

Mesh::Mesh(Mesh &&other)
	: mVertexBuffers(other.mVertexBuffers), mIndexBuffer(other.mIndexBuffer), mVertexCount(other.mVertexCount), mStrides(other.mStrides), mAttributeMapping(other.mAttributeMapping), mMapIndex(other.mMapIndex), mReferenceCount(other.mReferenceCount)
{
	other.mVertexBuffers.clear();
	other.mIndexBuffer = nullptr;
	other.mReferenceCount = nullptr;
}

Mesh::~Mesh()
{
	if (mReferenceCount)
	{
		mReferenceCount->Decrement();

		if (mReferenceCount == 0)
		{
			for (ID3D11Buffer *buffer : mVertexBuffers)
				buffer->Release();

			if (mIndexBuffer)
				mIndexBuffer->Release();

			delete mReferenceCount;
		}
	}
}

Mesh &Mesh::operator=(const Mesh &other)
{
	Mesh temp(other);
	Swap(temp);

	mReferenceCount->Increment();

	return *this;
}

Mesh &Mesh::operator=(Mesh &&other)
{
	Swap(other);

	return *this;
}

void Mesh::Swap(Mesh &other)
{
	ReferenceCount *tempRefCount = mReferenceCount;
	mReferenceCount = other.mReferenceCount;
	other.mReferenceCount = tempRefCount;

	std::vector<ID3D11Buffer*> tempBuffs = mVertexBuffers;
	mVertexBuffers = other.mVertexBuffers;
	other.mVertexBuffers = tempBuffs;

	std::vector<unsigned int> tempStrides = mStrides;
	mStrides = other.mStrides;
	other.mStrides = tempStrides;

	unsigned int tempMapIndex = mMapIndex;
	mMapIndex = other.mMapIndex;
	other.mMapIndex = tempMapIndex;

	std::map<std::string, unsigned int> tempAttributeMap = mAttributeMapping;
	mAttributeMapping = other.mAttributeMapping;
	other.mAttributeMapping = tempAttributeMap;

	ID3D11Buffer *tempRes = mIndexBuffer;
	mIndexBuffer = other.mIndexBuffer;
	other.mIndexBuffer = tempRes;

	unsigned int tempVertexCount = mVertexCount;
	mVertexCount = other.mVertexCount;
	other.mVertexCount = tempVertexCount;
}

void Mesh::Clear()
{
	mReferenceCount->Decrement();

	if (mReferenceCount == 0)
	{
		for (ID3D11Buffer *buffer : mVertexBuffers)
			buffer->Release();

		if (mIndexBuffer)
			mIndexBuffer->Release();

		delete mReferenceCount;
	}

	mReferenceCount = new ReferenceCount;

	mVertexBuffers.clear();
	mStrides.clear();
	mAttributeMapping.clear();
	mMapIndex = 0;
	mIndexBuffer = nullptr;
	mVertexCount = 0;
}

void Mesh::LoadIndexBuffer(std::vector<unsigned int> indices)
{
	// index buffer description
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.ByteWidth = sizeof(unsigned int) * indices.size();
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	// index buffer data
	D3D11_SUBRESOURCE_DATA bufferData = {};
	bufferData.pSysMem = &indices[0];

	// create index buffer
	GraphicsSystem::GetInstance().GetDevice()->CreateBuffer(&bufferDesc, &bufferData, &mIndexBuffer);

	// set number of vertices to draw
	mVertexCount = indices.size();
}

void Mesh::BindAttribute(std::string attributeSemantic, unsigned int slot) const
{
	unsigned int index = mAttributeMapping[attributeSemantic];
	ID3D11Buffer *vertexBuffer = mVertexBuffers[index];

	unsigned int offset = 0;
	GraphicsSystem::GetInstance().GetDeviceContext()->IASetVertexBuffers(slot, 1, &vertexBuffer, &mStrides[index], &offset);

	// if present, bind index buffer 
	if (mIndexBuffer)
		GraphicsSystem::GetInstance().GetDeviceContext()->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
}

void Mesh::BindIndexBuffer() const
{
	if (mIndexBuffer)
		GraphicsSystem::GetInstance().GetDeviceContext()->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
}

void Mesh::Bind() const
{
	std::vector<unsigned int> offsets(mVertexBuffers.size()); // all elements are 0
	GraphicsSystem::GetInstance().GetDeviceContext()->IASetVertexBuffers(0, mVertexBuffers.size(), &mVertexBuffers[0], &mStrides[0], &offsets[0]);

	// if present, bind index buffer 
	if (mIndexBuffer)
		GraphicsSystem::GetInstance().GetDeviceContext()->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
}

void Mesh::Draw() const
{
	if (mIndexBuffer)  
		GraphicsSystem::GetInstance().GetDeviceContext()->DrawIndexed(mVertexCount, 0, 0);
	else  
		GraphicsSystem::GetInstance().GetDeviceContext()->Draw(mVertexCount, 0);
}