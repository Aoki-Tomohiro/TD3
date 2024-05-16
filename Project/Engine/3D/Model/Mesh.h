#pragma once
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"
#include <vector>
#include <memory>

class Mesh
{
public:
	struct MeshData
	{
		std::vector<VertexDataPosUVNormal> vertices;
		std::vector<uint32_t> indices;
		uint32_t materialIndex;
	};

	void Initialize(const MeshData& meshData);

	void Update();

	const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const { return vertexBufferView_; };

	const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const { return indexBufferView_; };

	const size_t GetVerticesSize() const { return meshData_.vertices.size(); };

	const size_t GetIndicesSize() const { return meshData_.indices.size(); };

	const uint32_t GetMaterialIndex() const { return meshData_.materialIndex; };

private:
	void CreateVertexBuffer();

	void CreateIndexBuffer();

private:
	MeshData meshData_{};

	std::unique_ptr<UploadBuffer> vertexBuffer_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	std::unique_ptr<UploadBuffer> indexBuffer_ = nullptr;

	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
};

