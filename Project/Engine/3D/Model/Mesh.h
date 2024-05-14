#pragma once
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"
#include <vector>
#include <memory>

class Mesh
{
public:
	void Initialize(const std::vector<VertexDataPosUVNormal>& vertices, const std::vector<uint32_t>& indices);

	void Update();

	const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const { return vertexBufferView_; };

	const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const { return indexBufferView_; };

	const size_t GetVerticesSize() const { return vertices_.size(); };

	const size_t GetIndicesSize() const { return indices_.size(); };

private:
	void CreateVertexBuffer();

	void CreateIndexBuffer();

private:
	std::unique_ptr<UploadBuffer> vertexBuffer_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	std::vector<VertexDataPosUVNormal> vertices_;

	std::unique_ptr<UploadBuffer> indexBuffer_ = nullptr;

	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	std::vector<uint32_t> indices_{};
};

