#pragma once

#include <engine/assets/texture.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <map>
#include <iostream>

namespace engine::assets {

	struct Vertices {
		// position
		std::vector<glm::vec3> Positions;
		// normal
		std::vector<glm::vec3> Normals;
		// texCoords
		std::vector<glm::vec2> TexCoords;
		// tangent
		std::vector<glm::vec3> Tangents;
		// bitangent
		std::vector<glm::vec3> Bitangents;
	};

	struct Material {
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float shininess;
	};

	/**
	* Texture including type of texture and path
	* @param id of texture was initialized with
	* @param type of texture e.g. diffuse or normalmap
	* @param path of texture drom disk
	*/
	struct ModelTexture {
		unsigned int id;
		std::string type;
		std::string path;
	};

	/**
	* BoneInfo holds the bones offset and it's transformation used for the transformation model
	* @param boneOffset
	* @param finalTransform
	*/
	struct BoneInfo
	{
		glm::mat4 boneOffset;
		glm::mat4 finalTransformation;

		BoneInfo()
		{
			boneOffset = glm::mat4(1.0f);
			finalTransformation = glm::mat4(1.0f);
		}
	};

	/**
	* VertexBoneData holds the bones id's and weights a vertex can be manipulated with.
	* @param ids of bones
	* @param weights of bones
	*/
	struct VertexBoneData {
		unsigned int ids[8];
		float weights[8];

		VertexBoneData()
		{
			Reset();
		};

		void Reset()
		{
			memset(ids, 0, sizeof(ids));
			memset(weights, 0, sizeof(weights));
		}

		void AddBoneData(unsigned int BoneID, float Weight)
		{
			for (unsigned int i = 0; i < sizeof(ids) / sizeof(ids[0]); i++) {
				if (weights[i] == 0.0) {
					ids[i] = BoneID;
					weights[i] = Weight;
					return;
				}
			}
			// should never get here - more bones than we have space for
			assert(0);
		};
	};

	/** NodeAnimation holds all the animation transformations over time */
	struct NodeAnimation {
		std::vector<glm::quat> rotations;
		std::vector<double> rotationTime;
		unsigned int numberOfRotations;

		std::vector<glm::vec3> positions;
		std::vector<double> positionTime;
		unsigned int numberOfPositions;

		std::vector<glm::vec3> scalings;
		std::vector<double> scaleTime;
		unsigned int numberOfScalings;
	};

	struct Animation {
		double ticksPerSecond;
		double duration;
		unsigned int numberOfChannels;
		std::map<std::string, NodeAnimation> nodeAnimation;
	};

	struct Node {
		std::string name;
		unsigned int numberOfChildren;
		glm::mat4 transformation;
		std::vector<Node> children;
	};

	struct MeshData {
		Vertices _vertices;
		std::vector<unsigned int> _indices;
		std::vector<ModelTexture> _textures;
		Material _material;
		std::vector<VertexBoneData> _bones;
	};

	/**
	* A Model loader.
	* @param meshData Holds all vertices, normales, texturecoordinates... of loaded model.
	* @param bones Holds all bone informations of loaded model.
	* @param meshData Holds all animations of loaded model.
	*/
    class Model {
        public:
            Model(std::filesystem::path relativeAssetPath, bool gamma = false) : gammaCorrection(gamma) {
                auto assetPath  = resolveAssetPath(relativeAssetPath);

				// read file via ASSIMP
				Assimp::Importer importer;
				const aiScene* scene = importer.ReadFile(assetPath.string(), aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices); // | aiProcess_FlipUVs ToDo How do I know when to use this????
																																												   // check for errors
				if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
				{
					std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
					return;
				}
				// retrieve the directory path of the filepath
				std::string pathString = relativeAssetPath.string();
				directory = pathString.substr(0, pathString.find_last_of('/'));

				// process ASSIMP's root node recursively
				processNode(scene->mRootNode, scene);

				_animations = LoadAnims(scene, _animations);

				_nodes = LoadNodes(scene->mRootNode);
            }

			// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
			void processNode(aiNode *node, const aiScene *scene)
			{
				// process each mesh located at the current node
				for (unsigned int i = 0; i < node->mNumMeshes; i++)
				{
					// the node object only contains indices to index the actual objects in the scene. 
					// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
					aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

					processMesh(meshIndex, mesh, scene, scene->mRootNode);

					meshIndex += mesh->mNumVertices;
				}

				// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
				for (unsigned int i = 0; i < node->mNumChildren; i++)
				{
					processNode(node->mChildren[i], scene);
				}

			}

			void processMesh(unsigned int meshIndex, aiMesh *mesh, const aiScene *scene, aiNode *node)
			{
				Vertices vertices;
				std::vector<unsigned int> indices;
				std::vector<ModelTexture> textures;
				Material material;
				std::vector<VertexBoneData> bones;
				std::vector<BoneInfo> boneInfo;
				std::map<std::string, unsigned int> boneMapping;

				for (unsigned int i = 0; i < mesh->mNumVertices; i++)
				{
					glm::vec3 vector; 
					
					// positions
					vector.x = mesh->mVertices[i].x;
					vector.y = mesh->mVertices[i].y;
					vector.z = mesh->mVertices[i].z;
					vertices.Positions.push_back(vector);
					
					// normals
					vector.x = mesh->mNormals[i].x;
					vector.y = mesh->mNormals[i].y;
					vector.z = mesh->mNormals[i].z;
					vertices.Normals.push_back(vector);
					
					// texture coordinates
					if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
					{
						glm::vec2 vec;
						// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
						// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
						vec.x = mesh->mTextureCoords[0][i].x;
						vec.y = mesh->mTextureCoords[0][i].y;
						vertices.TexCoords.push_back(vec);
					}
					else {
						vertices.TexCoords.push_back(glm::vec2(0.0f, 0.0f));
					}

					// tangent
					if (mesh->mTangents != NULL) {
						vector.x = mesh->mTangents[i].x;
						vector.y = mesh->mTangents[i].y;
						vector.z = mesh->mTangents[i].z;
						vertices.Tangents.push_back(vector);
					}

					// bitangent
					if (mesh->mBitangents != NULL) {
						vector.x = mesh->mBitangents[i].x;
						vector.y = mesh->mBitangents[i].y;
						vector.z = mesh->mBitangents[i].z;
						vertices.Bitangents.push_back(vector);
					}
				}

				// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
				for (unsigned int i = 0; i < mesh->mNumFaces; i++)
				{
					aiFace face = mesh->mFaces[i];
					// retrieve all indices of the face and store them in the indices vector
					for (unsigned int j = 0; j < face.mNumIndices; j++)
						indices.push_back(face.mIndices[j]);
				}

				// process materials
				aiMaterial* aiMaterial = scene->mMaterials[mesh->mMaterialIndex];
				// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
				// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 

				// 1. diffuse maps
				std::vector<ModelTexture> diffuseMaps = loadMaterialTextures(aiMaterial, aiTextureType_DIFFUSE, "texture_diffuse");
				textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
				// 2. specular maps
				std::vector<ModelTexture> specularMaps = loadMaterialTextures(aiMaterial, aiTextureType_SPECULAR, "texture_specular");
				textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
				// 3. normal maps
				std::vector<ModelTexture> normalMaps = loadMaterialTextures(aiMaterial, aiTextureType_NORMALS, "texture_normal");
				textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
				// 4. height maps
				std::vector<ModelTexture> heightMaps = loadMaterialTextures(aiMaterial, aiTextureType_AMBIENT, "texture_height");
				textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

				material = loadMaterial(aiMaterial);

				bones.resize(vertices.Positions.size());	// Need to resize because of the bone vertex ids...
				bones = LoadBones(meshIndex, mesh, bones);

				MeshData data = { vertices, indices, textures, material, bones };
				meshData.push_back(data);
			}

			/**
			* checks all material textures of a given type and loads the textures if they're not loaded yet. 
			* @param mat material from assimp
			* @param type of wanted assimp material
			* @param typeName of wanted texture
			* @return the required ModelTexture
			*/
			std::vector<ModelTexture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
			{

				std::vector<ModelTexture> modelTextures;
				for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
				{
					aiString str;
					mat->GetTexture(type, i, &str);
					// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
					bool skip = false;
					for (unsigned int j = 0; j < textures_loaded.size(); j++)
					{
						if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
						{
							modelTextures.push_back(textures_loaded[j]);
							skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
							break;
						}
					}
					if (!skip)
					{   // if texture hasn't been loaded already, load it
						ModelTexture modelTexture;

						std::string filename = std::string(str.C_Str());
						filename = directory + '/' + filename;

						Texture texture = Texture(filename);

						modelTexture.id = texture.id();
						modelTexture.type = typeName;
						modelTexture.path = str.C_Str();
						modelTextures.push_back(modelTexture);
						textures_loaded.push_back(modelTexture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
					}
				}
				return modelTextures;
			}

			Material loadMaterial(aiMaterial* mat) {
				Material material;
				aiColor3D color(0.f, 0.f, 0.f);
				float shininess;

				mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
				material.diffuse = glm::vec3(color.r, color.g, color.b);

				mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
				material.ambient = glm::vec3(color.r, color.g, color.b);

				mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
				material.specular = glm::vec3(color.r, color.g, color.b);

				mat->Get(AI_MATKEY_SHININESS, shininess);
				material.shininess = shininess;

				return material;
			}

			/**
			* Loads all bones of a given mesh as well as mapping all vertices of the mesh to the affecting bones.
			* @param meshIndex index of mesh, important for composed meshes
			* @param pMesh mesh of assimp
			* @param Bones vertex bone mapping infomations
			* @return the required bone mappings informations
			*/
			std::vector<VertexBoneData> LoadBones(unsigned int meshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& Bones)
			{
				for (unsigned int i = 0; i < pMesh->mNumBones; i++) {
					unsigned int boneIndex = 0;
					std::string boneName(pMesh->mBones[i]->mName.data);

					if (_boneMapping.find(boneName) == _boneMapping.end()) {
						boneIndex = numBones;
						numBones++;
						BoneInfo bi;
						_boneInfo.push_back(bi);

						_boneInfo[boneIndex].boneOffset = convertMatrix(pMesh->mBones[i]->mOffsetMatrix);
						_boneMapping[boneName] = boneIndex;
					}
					else {
						boneIndex = _boneMapping[boneName];
					}

					for (unsigned int j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {
						unsigned int vertexID = pMesh->mBones[i]->mWeights[j].mVertexId;
						float weight = pMesh->mBones[i]->mWeights[j].mWeight;
						Bones[vertexID].AddBoneData(boneIndex, weight);
					}
				}
				return Bones;
			}

			/**
			* Loads all available animations of the loaded model
			* @param scene assimp scene
			* @param animations vector of all animations to be stored to
			* @return the required animations
			*/
			std::vector<Animation> LoadAnims(const aiScene* scene, std::vector<Animation> animations) {
				
				for (unsigned int i = 0; i < scene->mNumAnimations; i++)
				{
					Animation animation;

					aiAnimation* anim = scene->mAnimations[i];
					animation.duration = anim->mDuration;
					animation.ticksPerSecond = anim->mTicksPerSecond;
					animation.numberOfChannels = anim->mNumChannels;

					for (unsigned int j = 0; j < anim->mNumChannels; j++)
					{
						NodeAnimation nodeAnimation;

						std::string name = anim->mChannels[j]->mNodeName.data;

						unsigned int p = anim->mChannels[j]->mNumPositionKeys;
						unsigned int s = anim->mChannels[j]->mNumScalingKeys;
						unsigned int r = anim->mChannels[j]->mNumRotationKeys;

						nodeAnimation.numberOfPositions = p;
						nodeAnimation.numberOfScalings = s;
						nodeAnimation.numberOfRotations = r;

						for (unsigned int x = 0; x < p; x++)
						{
							glm::vec3 vector;
							vector.x = anim->mChannels[j]->mPositionKeys[x].mValue.x;
							vector.y = anim->mChannels[j]->mPositionKeys[x].mValue.y;
							vector.z = anim->mChannels[j]->mPositionKeys[x].mValue.z;
							nodeAnimation.positions.push_back(vector);
							nodeAnimation.positionTime.push_back(anim->mChannels[j]->mPositionKeys[x].mTime);
						}

						for (unsigned int x = 0; x < s; x++)
						{
							glm::vec3 vectorS;
							vectorS.x = anim->mChannels[j]->mScalingKeys[x].mValue.x;
							vectorS.y = anim->mChannels[j]->mScalingKeys[x].mValue.y;
							vectorS.z = anim->mChannels[j]->mScalingKeys[x].mValue.z;
							nodeAnimation.scalings.push_back(vectorS);
							nodeAnimation.scaleTime.push_back(anim->mChannels[j]->mScalingKeys[x].mTime);
						}

						for (unsigned int x = 0; x < r; x++)
						{
							glm::quat quat;
							quat.x = anim->mChannels[j]->mRotationKeys[x].mValue.x;
							quat.y = anim->mChannels[j]->mRotationKeys[x].mValue.y;
							quat.z = anim->mChannels[j]->mRotationKeys[x].mValue.z;
							quat.w = anim->mChannels[j]->mRotationKeys[x].mValue.w;
							nodeAnimation.rotations.push_back(quat);
							nodeAnimation.rotationTime.push_back(anim->mChannels[j]->mRotationKeys[x].mTime);
						}
						animation.nodeAnimation[name] = nodeAnimation;
					}
					animations.push_back(animation);
				}
				return animations;
			}

			/**
			* Loads all nodes of the model recursively to traverse the all model informations
			* @param node assimp node
			* @return the current node
			*/
			Node LoadNodes(aiNode *node) {
				Node n;
				n.name = node->mName.data;
				n.transformation = convertMatrix(node->mTransformation);
				n.numberOfChildren = node->mNumChildren;

				for (unsigned int i = 0; i < n.numberOfChildren; i++)
				{
					Node childNode = LoadNodes(node->mChildren[i]);
					n.children.push_back(childNode);
				}

				return n;
			}

			glm::mat4 convertMatrix(const aiMatrix4x4& AssimpMatrix)
			{
				glm::mat4 m;

				m[0][0] = AssimpMatrix.a1; m[0][1] = AssimpMatrix.a2; m[0][2] = AssimpMatrix.a3; m[0][3] = AssimpMatrix.a4;
				m[1][0] = AssimpMatrix.b1; m[1][1] = AssimpMatrix.b2; m[1][2] = AssimpMatrix.b3; m[1][3] = AssimpMatrix.b4;
				m[2][0] = AssimpMatrix.c1; m[2][1] = AssimpMatrix.c2; m[2][2] = AssimpMatrix.c3; m[2][3] = AssimpMatrix.c4;
				m[3][0] = AssimpMatrix.d1; m[3][1] = AssimpMatrix.d2; m[3][2] = AssimpMatrix.d3; m[3][3] = AssimpMatrix.d4;

				return m;
			}


			const Vertices & vertices(unsigned int i) const {
				return meshData[i]._vertices;
			}

            const std::vector<unsigned int> & indices(unsigned int i) const {
                return meshData[i]._indices;
            } 

			const std::vector<ModelTexture> & textures (unsigned int i) const {
				return meshData[i]._textures;
			}

			const Material & material(unsigned int i) const {
				return meshData[i]._material;
			}

			const std::vector<VertexBoneData> & bones(unsigned int i) const {
				return meshData[i]._bones;
			}

			const std::vector<BoneInfo> & boneInfo() const {
				return _boneInfo;
			}

			const std::map<std::string, unsigned int> & boneMapping() const {
				return _boneMapping;
			}

			const std::vector<Animation> & animations() const {
				return _animations;
			}

			const Node & nodes() const {
				return _nodes;
			}

			const unsigned int numberOfMeshParts() {
				return meshData.size();
			}

        private:

			std::vector<MeshData> meshData;
			std::vector<BoneInfo> _boneInfo;
			std::map<std::string, unsigned int> _boneMapping;
			std::vector<Animation> _animations;
			Node _nodes;

			std::vector<ModelTexture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
			
			std::string directory;
			bool gammaCorrection;
			unsigned int meshIndex = 0;
			unsigned int numBones = 0;

    };
}

