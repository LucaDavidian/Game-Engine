#pragma once

#include "Error.h"
#include "GraphicsSystem.h"
#include <d3d11.h>
#include <map>
#include <vector>
#include <string>

class Mesh
{
private:
	class ReferenceCount
	{
	public:
		ReferenceCount() : mReferenceCount(1) {}
		void Increment() { ++mReferenceCount; }
		void Decrement() { --mReferenceCount; }
		int GetCount() const { return mReferenceCount; }
	private:
		int mReferenceCount;
	};
public:
	Mesh() : mReferenceCount(new ReferenceCount) {}
	Mesh(const Mesh &other);
	Mesh(Mesh &&other);

	~Mesh();

	Mesh &operator=(const Mesh &other);
	Mesh &operator=(Mesh &&other);

	template <typename T>
	void LoadAttribute(const std::string &attributeSemantic, T *data, unsigned int numElements, bool dynamic = false);
	
	void LoadIndexBuffer(std::vector<unsigned int> indices);
	void SetVertexCount(unsigned int vertexCount) { mVertexCount = vertexCount; }
	
	template <typename T>
	void UpdateDynamicAttribute(std::string const &attributeSemantic, T* data, unsigned int numElements, unsigned int offset = 0);

	void Bind() const;
	void BindAttribute(std::string attributeSemantic, unsigned int slot) const;
	void BindIndexBuffer() const;
	
	void Draw() const;

	void Clear();

	void Swap(Mesh &other);
private:
	std::vector<ID3D11Buffer*> mVertexBuffers;
	std::vector<unsigned int> mStrides;
	unsigned int mMapIndex = 0;
	mutable std::map<std::string, unsigned int> mAttributeMapping;

	ID3D11Buffer *mIndexBuffer = nullptr;
	unsigned int mVertexCount = 0;	
	
	ReferenceCount *mReferenceCount;
};

template <typename T>
void Mesh::LoadAttribute(const std::string &attributeSemantic, T *data, unsigned int numElements, bool dynamic)
{
	HRESULT hr;

	// vertex buffer description
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.ByteWidth = sizeof(T) * numElements;
	bufferDesc.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	// vertex buffer data - bufferData.SysMemPitch and bufferData.SysMemSlicePitch not needed (1D buffer)
	D3D11_SUBRESOURCE_DATA bufferData = {};
	bufferData.pSysMem = (void*)data; 

	// create Vertex buffer
	ID3D11Buffer *buffer;
	hr = GraphicsSystem::GetInstance().GetDevice()->CreateBuffer(&bufferDesc, &bufferData, &buffer);
	if (FAILED(hr))
		ErrorBox("vertex buffer creation failed");

	// add vertex buffer to list
	mVertexBuffers.push_back(buffer);
	mStrides.push_back(sizeof(T));

	// add attribute to map
	mAttributeMapping[attributeSemantic] = mMapIndex++;
}

template <typename T> 
void Mesh::UpdateDynamicAttribute(std::string const &attributeSemantic, T* data, unsigned int numElements, unsigned int offset)
{
	ID3D11Buffer *dynamicVertexBuffer = mVertexBuffers[mAttributeMapping[attributeSemantic]];  // TODO: attribute not present or not dynamic (error/exception)
	
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	
	GraphicsSystem::GetInstance().GetDeviceContext()->Map(dynamicVertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);
	
	T *vertexData(static_cast<T*>(mappedResource.pData));
	for (int i = offset; i < offset + numElements; i++)
		vertexData[i] = data[i - offset];

	GraphicsSystem::GetInstance().GetDeviceContext()->Unmap(dynamicVertexBuffer, 0);
}
