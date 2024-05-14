#include "Model.h"
#include "Engine/Base/GraphicsCore.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Math/MathFunction.h"

void Model::Create(const ModelData& modelData, const std::vector<Animation::AnimationData>& animationData, DrawPass drawPass)
{
	//モデルデータを設定
	modelData_ = modelData;

	//メッシュの作成
	mesh_ = std::make_unique<Mesh>();
	mesh_->Initialize(modelData_.vertices, modelData_.indices);

	//マテリアルの作成
	material_ = std::make_unique<Material>();
	material_->Initialize(modelData_.material.textureFilePath);

	//アニメーションの作成
	animation_ = std::make_unique<Animation>();
	animation_->Initialize(animationData, modelData_.rootNode);

	//SkinClusterの作成
	skinCluster_ = CreateSkinCluster(animation_->GetSkeleton(), modelData_);

	//Debug用のVertexBufferの生成
	if (!modelData_.skinClusterData.empty())
	{
		Animation::Skeleton skeleton = animation_->GetSkeleton();
		CreateBoneLineVertices(skeleton, skeleton.root, debugVertices_);
		CreateDebugVertexBuffer();
	}

	//描画パスを設定
	drawPass_ = drawPass;
}

void Model::Update(WorldTransform& worldTransform, const uint32_t animationNumber)
{
	//アニメーションの更新を行って、骨ごとのLocal情報を更新する
	animation_->ApplyAnimation(modelData_.rootNode.name, animationNumber);

	//アニメーションを適用
	worldTransform.matWorld_ = animation_->GetLocalMatrix() * worldTransform.matWorld_;

	//現在の骨ごとのLocal情報を基にSkeletonSpaceの情報を更新する
	animation_->Update();

	//SkeletonSpaceの情報を基に、SkinClusterのMatrixPaletteを更新する
	const std::vector<Animation::Joint>& joints = animation_->GetJoints();
	for (size_t jointIndex = 0; jointIndex < joints.size(); ++jointIndex)
	{
		assert(jointIndex < skinCluster_.inverseBindPoseMatrices.size());
		skinCluster_.mappedPalette[jointIndex].skeletonSpaceMatrix = skinCluster_.inverseBindPoseMatrices[jointIndex] * joints[jointIndex].skeletonSpaceMatrix;
		skinCluster_.mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix = Mathf::Transpose(Mathf::Inverse(skinCluster_.mappedPalette[jointIndex].skeletonSpaceMatrix));
	}
}

void Model::Draw(WorldTransform& worldTransform, const Camera& camera)
{
	//マテリアルの更新
	material_->Update();

	//RootのMatrixを適用
	if (modelData_.skinClusterData.empty())
	{
		worldTransform.matWorld_ = modelData_.rootNode.localMatrix * worldTransform.matWorld_;
		worldTransform.TransferMatrix();
	}

	//レンダラーのインスタンスを取得
	Renderer* renderer_ = Renderer::GetInstance();
	//SortObjectの追加
	renderer_->AddObject(mesh_->GetVertexBufferView(), skinCluster_.influenceBufferView, mesh_->GetIndexBufferView(), material_->GetConstantBuffer()->GetGpuVirtualAddress(),
		worldTransform.GetConstantBuffer()->GetGpuVirtualAddress(), camera.GetConstantBuffer()->GetGpuVirtualAddress(),
		material_->GetTexture()->GetSRVHandle(), skinCluster_.paletteResource->GetSRVHandle(), UINT(mesh_->GetIndicesSize()), drawPass_);

	//DebugObjectの追加
	if (isDebug_ && !modelData_.skinClusterData.empty())
	{
		UpdateVertexData(animation_->GetSkeleton(), animation_->GetSkeleton().root, debugVertices_);
		renderer_->AddDebugObject(debugVertexBufferView_, worldTransform.GetConstantBuffer()->GetGpuVirtualAddress(), camera.GetConstantBuffer()->GetGpuVirtualAddress(), UINT(debugVertices_.size()));
	}
}

Model::SkinCluster Model::CreateSkinCluster(const Animation::Skeleton& skeleton, const ModelData& modelData)
{
	//palette用のResourceを確保
	SkinCluster skinCluster;
	skinCluster.paletteResource = std::make_unique<StructuredBuffer>();
	skinCluster.paletteResource->Create(uint32_t(skeleton.joints.size()), sizeof(WellForGPU));
	WellForGPU* mappedPalette = static_cast<WellForGPU*>(skinCluster.paletteResource->Map());
	skinCluster.mappedPalette = { mappedPalette,skeleton.joints.size() };//spanを使ってアクセスするようにする
	//skinCluster.paletteResource->Unmap();

	//influence用のResourceを確保。頂点ごとにinfluenced情報を追加できるようにする
	skinCluster.influenceResource = std::make_unique<UploadBuffer>();
	skinCluster.influenceResource->Create(sizeof(VertexInfluence) * modelData.vertices.size());
	VertexInfluence* mappedInfluence = static_cast<VertexInfluence*>(skinCluster.influenceResource->Map());
	std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * modelData.vertices.size());//0埋め。weightを0にしておく。
	skinCluster.mappedInfluence = { mappedInfluence,modelData.vertices.size() };
	//skinCluster.influenceResource->Unmap();

	//Influence用のVBVを作成
	skinCluster.influenceBufferView.BufferLocation = skinCluster.influenceResource->GetGpuVirtualAddress();
	skinCluster.influenceBufferView.SizeInBytes = UINT(sizeof(VertexInfluence) * modelData.vertices.size());
	skinCluster.influenceBufferView.StrideInBytes = sizeof(VertexInfluence);

	//InverseBindPoseMatrixを格納する場所を作成して、単位行列で埋める
	skinCluster.inverseBindPoseMatrices.resize(skeleton.joints.size());
	for (Matrix4x4& inverseBindPoseMatrix : skinCluster.inverseBindPoseMatrices)
	{
		inverseBindPoseMatrix = Mathf::MakeIdentity4x4();
	}

	//ModelDataを解析してInfluenceを埋める
	for (const auto& jointWeight : modelData.skinClusterData)//ModelのSkinClusterの情報を解析
	{
		auto it = skeleton.jointMap.find(jointWeight.first);//JointWeight,firstはjoint名なので、skeletonに対象となるjointが含まれているか判断
		if (it == skeleton.jointMap.end())//そんな名前のJointは存在しない。なので次に回す
		{
			continue;
		}

		//(*it).secondにはjointのindexが入っているので、該当のindexのinverseBindPoseMatrixを代入
		skinCluster.inverseBindPoseMatrices[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
		for (const auto& vertexWeight : jointWeight.second.vertexWeights)
		{
			auto& currentInfluence = skinCluster.mappedInfluence[vertexWeight.vertexIndex];//該当のvertexIndexのinfluence情報を参照しておく
			for (uint32_t index = 0; index < kNumMaxInfluence; ++index)//空いているところに入れる
			{
				if (currentInfluence.weights[index] == 0.0f)//Weight == 0が空いている状態なので、その場所にweightとjointのindexを代入
				{
					currentInfluence.weights[index] = vertexWeight.weight;
					currentInfluence.jointIndices[index] = (*it).second;
					break;
				}
			}
		}
	}

	return skinCluster;
}

void Model::CreateDebugVertexBuffer()
{
	//頂点バッファを作成
	debugVertexBuffer_ = std::make_unique<UploadBuffer>();
	debugVertexBuffer_->Create(sizeof(Vector4) * debugVertices_.size());

	//頂点バッファビューを作成
	debugVertexBufferView_.BufferLocation = debugVertexBuffer_->GetGpuVirtualAddress();
	debugVertexBufferView_.SizeInBytes = UINT(sizeof(Vector4) * debugVertices_.size());
	debugVertexBufferView_.StrideInBytes = sizeof(Vector4);

	//頂点バッファにデータを書き込む
	Vector4* vertexData = static_cast<Vector4*>(debugVertexBuffer_->Map());
	std::memcpy(vertexData, debugVertices_.data(), sizeof(Vector4) * debugVertices_.size());
	debugVertexBuffer_->Unmap();
}

void Model::CreateBoneLineVertices(const Animation::Skeleton& skeleton, int32_t parentIndex, std::vector<Vector4>& vertices)
{
	const Animation::Joint& parentJoint = skeleton.joints[parentIndex];
	for (int32_t childIndex : parentJoint.children)
	{
		const Animation::Joint& childJoint = skeleton.joints[childIndex];
		vertices.push_back({ parentJoint.skeletonSpaceMatrix.m[3][0],parentJoint.skeletonSpaceMatrix.m[3][1],parentJoint.skeletonSpaceMatrix.m[3][2],1.0f });
		vertices.push_back({ childJoint.skeletonSpaceMatrix.m[3][0],childJoint.skeletonSpaceMatrix.m[3][1],childJoint.skeletonSpaceMatrix.m[3][2],1.0f });
		CreateBoneLineVertices(skeleton, childIndex, vertices);
	}
}

void Model::UpdateVertexData(const Animation::Skeleton& skeleton, int32_t parentIndex, std::vector<Vector4>& vertices)
{
	vertices.clear();
	const Animation::Joint& parentJoint = skeleton.joints[parentIndex];
	for (int32_t childIndex : parentJoint.children)
	{
		const Animation::Joint& childJoint = skeleton.joints[childIndex];
		vertices.push_back({ parentJoint.skeletonSpaceMatrix.m[3][0],parentJoint.skeletonSpaceMatrix.m[3][1],parentJoint.skeletonSpaceMatrix.m[3][2],1.0f });
		vertices.push_back({ childJoint.skeletonSpaceMatrix.m[3][0],childJoint.skeletonSpaceMatrix.m[3][1],childJoint.skeletonSpaceMatrix.m[3][2],1.0f });
		CreateBoneLineVertices(skeleton, childIndex, vertices);
	}
	Vector4* vertexData = static_cast<Vector4*>(debugVertexBuffer_->Map());
	std::memcpy(vertexData, vertices.data(), sizeof(Vector4) * vertices.size());
	debugVertexBuffer_->Unmap();
}