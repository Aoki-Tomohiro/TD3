#include "Mesh.h"

void Mesh::Initialize(const MeshData& meshData)
{
	//頂点データの初期化
	meshData_ = meshData;

	//頂点バッファの作成
	CreateVertexBuffer();

	//インデックスバッファの作成
	CreateIndexBuffer();

	//更新
	Update();
}

void Mesh::Update()
{
	//頂点バッファにデータを書き込む
	VertexDataPosUVNormal* vertexData = static_cast<VertexDataPosUVNormal*>(vertexBuffer_->Map());
	std::memcpy(vertexData, meshData_.vertices.data(), sizeof(VertexDataPosUVNormal) * meshData_.vertices.size());
	vertexBuffer_->Unmap();

	//インデックスバッファのデータを書き込む
	uint32_t* indexData = static_cast<uint32_t*>(indexBuffer_->Map());
	std::memcpy(indexData, meshData_.indices.data(), sizeof(uint32_t) * meshData_.indices.size());
	indexBuffer_->Unmap();
}

void Mesh::CreateVertexBuffer()
{
	//頂点バッファを作成
	vertexBuffer_ = std::make_unique<UploadBuffer>();
	vertexBuffer_->Create(sizeof(VertexDataPosUVNormal) * meshData_.vertices.size());

	//頂点バッファビューを作成
	vertexBufferView_.BufferLocation = vertexBuffer_->GetGpuVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexDataPosUVNormal) * meshData_.vertices.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexDataPosUVNormal);
}

void Mesh::CreateIndexBuffer()
{
	//インデックスバッファの作成
	indexBuffer_ = std::make_unique<UploadBuffer>();
	indexBuffer_->Create(sizeof(uint32_t) * meshData_.indices.size());

	//インデックスバッファビューの作成
	indexBufferView_.BufferLocation = indexBuffer_->GetGpuVirtualAddress();
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * meshData_.indices.size());
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
}