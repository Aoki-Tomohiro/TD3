#include "Mesh.h"

void Mesh::Initialize(const std::vector<VertexDataPosUVNormal>& vertices, const std::vector<uint32_t>& indices)
{
	//頂点データの初期化
	vertices_ = vertices;

	//頂点バッファの作成
	CreateVertexBuffer();

	//インデックスの初期化
	indices_ = indices;

	//インデックスバッファの作成
	CreateIndexBuffer();

	//更新
	Update();
}

void Mesh::Update()
{
	//頂点バッファにデータを書き込む
	VertexDataPosUVNormal* vertexData = static_cast<VertexDataPosUVNormal*>(vertexBuffer_->Map());
	std::memcpy(vertexData, vertices_.data(), sizeof(VertexDataPosUVNormal) * vertices_.size());
	vertexBuffer_->Unmap();

	//インデックスバッファのデータを書き込む
	uint32_t* indexData = static_cast<uint32_t*>(indexBuffer_->Map());
	std::memcpy(indexData, indices_.data(), sizeof(uint32_t) * indices_.size());
	indexBuffer_->Unmap();
}

void Mesh::CreateVertexBuffer()
{
	//頂点バッファを作成
	vertexBuffer_ = std::make_unique<UploadBuffer>();
	vertexBuffer_->Create(sizeof(VertexDataPosUVNormal) * vertices_.size());

	//頂点バッファビューを作成
	vertexBufferView_.BufferLocation = vertexBuffer_->GetGpuVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexDataPosUVNormal) * vertices_.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexDataPosUVNormal);
}

void Mesh::CreateIndexBuffer()
{
	//インデックスバッファの作成
	indexBuffer_ = std::make_unique<UploadBuffer>();
	indexBuffer_->Create(sizeof(uint32_t) * indices_.size());

	//インデックスバッファビューの作成
	indexBufferView_.BufferLocation = indexBuffer_->GetGpuVirtualAddress();
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * indices_.size());
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
}