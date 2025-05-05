#include <SGR.h>
#include <iostream>

#include <stdio.h>

#define STB_TRUETYPE_IMPLEMENTATION
	#include "stb_truetype.h"

#define TINYOBJLOADER_IMPLEMENTATION
	#include "tiny_obj_loader.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
	#include "tiny_gltf.h"


std::string resourcePath;



SGR sgr_object1;

SgrTime_t frame_dr;
SgrTime_t lastDraw = SgrTime::now();
SgrTime_t lastTextureChange = SgrTime::now();

// data structure for instance uses shader presenter as "instance shader"
struct InstanceData {
	glm::mat4 model;
	glm::vec4 color;
	glm::vec2 deltaText;
	glm::vec2 startMesh;
	glm::vec2 startText;
};

struct ModelInstanceData {
	glm::mat4 model{1.f};
};

SgrInstancesUBO rectangles;
SgrInstancesUBO models;
SgrGlobalUBO ubo;
std::map<std::string, SgrInstancesUBO> gltfPrimitives;
std::map<std::string, uint32_t> gltfPrimitivesCounter;

struct GLTFVertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec3 color;
};

struct Primitive {
	std::string name;
	std::vector<GLTFVertex> verts;
	std::vector<uint32_t> inds;
	int textureInd;
};

struct Mesh {
	std::vector<Primitive> primitives;
};

struct Node {
	std::string name;
	Node* parent;
	std::vector<Node*> children;
	Mesh mesh;
	glm::mat4 matrix;
	~Node() {
		for (auto& child : children) {
			delete child;
		}
	}
};

struct Material {
	glm::vec4 baseColorFactor = glm::vec4(1.0f);
	uint32_t baseColorTextureIndex;
};

void loadNode(const tinygltf::Node& inputNode, const tinygltf::Model& input, Node* parent, std::vector<Node*>& nodes)
{
	Node* node = new Node{};
	node->name = inputNode.name;
	node->matrix = glm::mat4(1.0f);
	node->parent = parent;

	// Get the local node matrix
	// It's either made up from translation, rotation, scale or a 4x4 matrix
	if (inputNode.translation.size() == 3) {
		node->matrix = glm::translate(node->matrix, glm::vec3(glm::make_vec3(inputNode.translation.data())));
	}
	if (inputNode.rotation.size() == 4) {
		glm::quat q = glm::make_quat(inputNode.rotation.data());
		node->matrix *= glm::mat4(q);
	}
	if (inputNode.scale.size() == 3) {
		node->matrix = glm::scale(node->matrix, glm::vec3(glm::make_vec3(inputNode.scale.data())));
	}
	if (inputNode.matrix.size() == 16) {
		node->matrix = glm::make_mat4x4(inputNode.matrix.data());
	};

	// Load node's children
	if (inputNode.children.size() > 0) {
		for (size_t i = 0; i < inputNode.children.size(); i++) {
			loadNode(input.nodes[inputNode.children[i]], input , node, nodes);
		}
	}

	// If the node contains mesh data, we load vertices and indices from the buffers
	// In glTF this is done via accessors and buffer views
	if (inputNode.mesh > -1) {
		const tinygltf::Mesh mesh = input.meshes[inputNode.mesh];
		// Iterate through all primitives of this node's mesh
		for (size_t i = 0; i < mesh.primitives.size(); i++) {
			const tinygltf::Primitive& glTFPrimitive = mesh.primitives[i];
			Primitive* primitive = new Primitive{};
			primitive->name = mesh.name + "_" + std::to_string(i);
			uint32_t indexCount = 0;
			// Vertices
			{
				const float* positionBuffer = nullptr;
				const float* normalsBuffer = nullptr;
				const float* texCoordsBuffer = nullptr;
				size_t vertexCount = 0;

				// Get buffer data for vertex positions
				if (glTFPrimitive.attributes.find("POSITION") != glTFPrimitive.attributes.end()) {
					const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("POSITION")->second];
					const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
					positionBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
					vertexCount = accessor.count;
				}
				// Get buffer data for vertex normals
				if (glTFPrimitive.attributes.find("NORMAL") != glTFPrimitive.attributes.end()) {
					const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("NORMAL")->second];
					const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
					normalsBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
				}
				// Get buffer data for vertex texture coordinates
				// glTF supports multiple sets, we only load the first one
				if (glTFPrimitive.attributes.find("TEXCOORD_0") != glTFPrimitive.attributes.end()) {
					const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("TEXCOORD_0")->second];
					const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
					texCoordsBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
				}

				// Append data to model's vertex buffer
				for (size_t v = 0; v < vertexCount; v++) {
					GLTFVertex vert{};
					vert.pos = glm::vec4(glm::make_vec3(&positionBuffer[v * 3]), 1.0f);
					vert.normal = glm::normalize(glm::vec3(normalsBuffer ? glm::make_vec3(&normalsBuffer[v * 3]) : glm::vec3(0.0f)));
					vert.uv = texCoordsBuffer ? glm::make_vec2(&texCoordsBuffer[v * 2]) : glm::vec3(0.0f);
					vert.color = glm::vec3(1.0f);
					primitive->verts.push_back(vert);
				}

				primitive->textureInd = input.materials[glTFPrimitive.material].values.find("baseColorTexture")->second.TextureIndex();
			}
			// Indices
			{
				const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.indices];
				const tinygltf::BufferView& bufferView = input.bufferViews[accessor.bufferView];
				const tinygltf::Buffer& buffer = input.buffers[bufferView.buffer];

				indexCount += static_cast<uint32_t>(accessor.count);

				// glTF supports different component types of indices
				switch (accessor.componentType) {
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
					const uint32_t* buf = reinterpret_cast<const uint32_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
					for (size_t index = 0; index < accessor.count; index++) {
						primitive->inds.push_back(buf[index]);
					}
					break;
				}
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
					const uint16_t* buf = reinterpret_cast<const uint16_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
					for (size_t index = 0; index < accessor.count; index++) {
						primitive->inds.push_back(buf[index]);
					}
					break;
				}
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
					const uint8_t* buf = reinterpret_cast<const uint8_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
					for (size_t index = 0; index < accessor.count; index++) {
						primitive->inds.push_back(buf[index]);
					}
					break;
				}
				default:
					std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
					return;
				}
			}
			//primitive.materialIndex = glTFPrimitive.material;
			node->mesh.primitives.push_back(*primitive);

			gltfPrimitives[primitive->name] = *new SgrInstancesUBO{};
			gltfPrimitives[primitive->name].instnaceCount = 1;
			gltfPrimitives[primitive->name].instanceSize = sizeof(ModelInstanceData);
			if (MemoryManager::createDynamicUniformMemory(gltfPrimitives[primitive->name]) != sgrOK)
				return;

			if (MemoryManager::get()->createDynamicUniformBuffer(gltfPrimitives[primitive->name].ubo, gltfPrimitives[primitive->name].dataSize, gltfPrimitives[primitive->name].dynamicAlignment) != sgrOK)
				return;
		}
	}

	if (parent) {
		parent->children.push_back(node);
	}
	else {
		nodes.push_back(node);
	}
}

bool loadImages(tinygltf::Model input, std::vector<SgrImage*>& images)
{
	// Images can be stored inside the glTF (which is the case for the sample model), so instead of directly
	// loading them from disk, we fetch them from the glTF loader and upload the buffers
	images.resize(input.images.size());
	for (size_t i = 0; i < input.images.size(); i++) {
		tinygltf::Image& glTFImage = input.images[i];
		// Get the image data from the glTF loader
		unsigned char* buffer = nullptr;
		VkDeviceSize bufferSize = 0;
		bool deleteBuffer = false;
		// We convert RGB-only images to RGBA, as most devices don't support RGB-formats in Vulkan
		if (glTFImage.component == 3) {
			bufferSize = glTFImage.width * glTFImage.height * 4;
			buffer = new unsigned char[bufferSize];
			unsigned char* rgba = buffer;
			unsigned char* rgb = &glTFImage.image[0];
			for (size_t i = 0; i < glTFImage.width * glTFImage.height; ++i) {
				memcpy(rgba, rgb, sizeof(unsigned char) * 3);
				rgba += 4;
				rgb += 3;
			}
			deleteBuffer = true;
		}
		else {
			buffer = &glTFImage.image[0];
			bufferSize = glTFImage.image.size();
		}
		// Load texture from image buffer
		//images[i] = nullptr;
		if (TextureManager::createTextureImage((void*)buffer, glTFImage.width, glTFImage.height, images[i]) != sgrOK)
			return false;

		if (deleteBuffer) {
			delete[] buffer;
		}
	}

	return true;
}

bool loadGLTFModel(std::string filename, std::vector<Node*>& nodes, std::vector<SgrImage*>& images)
{
	std::vector<int32_t> textures;
	std::vector<Material> materials;

	tinygltf::Model glTFInput;
	tinygltf::TinyGLTF gltfContext;
	std::string error, warning;

	bool fileLoaded = gltfContext.LoadASCIIFromFile(&glTFInput, &error, &warning, filename);

	if (!loadImages(glTFInput, images))
		return false;
	/*glTFModel.loadMaterials(glTFInput);
	glTFModel.loadTextures(glTFInput);*/

	const tinygltf::Scene& scene = glTFInput.scenes[0];
	for (size_t i = 0; i < scene.nodes.size(); i++) {
		const tinygltf::Node node = glTFInput.nodes[scene.nodes[i]];
		loadNode(node, glTFInput, nullptr, nodes);
	}

	return fileLoaded;
}

struct ModelVertex {
	SgrVertex vert;
	int texInd;
	glm::vec2 texCoord;
	glm::vec4 color;

	bool operator==(const ModelVertex& other) const
	{
		return vert == other.vert;
	}
};


bool loadObjectModel(std::string modelPath, std::string modelName, std::vector<ModelVertex>& vertices, std::vector<uint32_t>& indices, std::vector<SgrImage*>& textures)
{
	std::string objPath = modelPath+"/"+modelName+".obj";
	printf("\n\nLoad model: %s\n",modelName.c_str());

	tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objPath.c_str(), modelPath.c_str())) {
        return false;
    }

	bool noMaterials = materials.empty();
	if (noMaterials) {
		SgrImage* texture = nullptr;
		if (TextureManager::createTextureImage(modelPath+"/"+modelName+".png", texture) != sgrOK)
			return false;
		textures.push_back(texture);
	}

	int shapeAdded = 0;
	std::vector<int> addedTextures;
	for (const auto& shape : shapes) {
		printf("\rLoad process...[%d]",(int)(shapeAdded/(float)shapes.size()*100));
		fflush(stdout);
		for (const auto& index : shape.mesh.indices) {
			ModelVertex vertex{};
			vertex.vert.x = attrib.vertices[3 * index.vertex_index + 0];
			vertex.vert.y = attrib.vertices[3 * index.vertex_index + 1];
			vertex.vert.z = attrib.vertices[3 * index.vertex_index + 2];

#if NDBUG == FALSE
			auto it = std::find(vertices.begin(), vertices.end(), vertex);
			if (it != vertices.end()) {
				indices.push_back(std::distance(vertices.begin(), it));
				continue;
			}
#endif

			vertex.texCoord = {			attrib.texcoords[2 * index.texcoord_index + 0],
    						 	 1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};

			vertex.texInd = 0;
			vertex.color[3] = 1;
			if (!noMaterials) {
				int requiredTexture = shape.mesh.material_ids[0];

				vertex.color[3] = materials[requiredTexture].dissolve;
				if (materials[requiredTexture].diffuse_texname.empty()) {
					vertex.texInd = -1;
					vertex.color[0] = materials[requiredTexture].diffuse[0];
					vertex.color[1] = materials[requiredTexture].diffuse[1];
					vertex.color[2] = materials[requiredTexture].diffuse[2];
				}
				else {
					bool added = std::find(addedTextures.begin(), addedTextures.end(), requiredTexture) != addedTextures.end();

					if (!added) {
						SgrImage* texture = nullptr;
						std::string textName = modelPath + "/" + materials[requiredTexture].diffuse_texname;
						if (TextureManager::createTextureImage(textName, texture) != sgrOK)
							return false;
						else {
							textures.push_back(texture);
							addedTextures.push_back(requiredTexture);
							vertex.texInd = (int)(textures.size()) - 1;
						}
					}
					else {
						auto it = std::find(addedTextures.begin(), addedTextures.end(), requiredTexture);
						vertex.texInd = (int)std::distance(addedTextures.begin(), it);
						if (vertex.texInd < 0 || vertex.texInd > addedTextures.size() - 1) {
							return false;
						}
					}
				}
			}

			vertices.push_back(vertex);
			indices.push_back((uint32_t)(vertices.size())-1);
    	}
		shapeAdded++;
	}

	SgrVertex centroid{ 0,0,0 };
	for (const auto& vertex : vertices) {
		centroid += vertex.vert;
	}

	for (auto& vertex : vertices) {
		vertex.vert -= (centroid / (float)vertices.size());
	}

	printf("\nLoad process ...[%d]",100);
	printf("\nLoaded");

	printf("\nLoaded %d vertices | draw %d indices",(int)vertices.size(), (int)indices.size());

	return true;
}

bool getFontData(std::string font_path, unsigned char* &fontPixels, stbtt_bakedchar* &backedChars, uint32_t &width, uint32_t &height)
{
	FILE* font_ttf = fopen(font_path.c_str(), "rb");
	if (!font_ttf) {
		printf("Font file not found! :(\n");
		return false;
	}

	long file_size;
	if (fseek(font_ttf, 0, SEEK_END) !=0) {
		printf("File is corrupted\n");
		return false;
	} else {
		file_size = ftell(font_ttf);
		fseek(font_ttf, 0, SEEK_SET);
	}

	unsigned char* font_data = (unsigned char*)malloc(file_size);
	if(font_data && fread(font_data, 1, file_size, font_ttf) != file_size) {
		printf("Real time corrupted file\n");
		free(font_data);
		return false;
	};
	fclose(font_ttf);

	size_t bitmapWidth = 512;
	size_t bitmapHeight = 512;
	width = static_cast<uint32_t>(bitmapWidth);
	height = static_cast<uint32_t>(bitmapHeight);
	fontPixels = (unsigned char*)malloc(bitmapWidth*bitmapHeight);
	const int char_number_to_bake = 96;
	backedChars = (stbtt_bakedchar*)malloc(char_number_to_bake*sizeof(stbtt_bakedchar));

	int result = stbtt_BakeFontBitmap(font_data, 0, 64, fontPixels, width, height, 32, char_number_to_bake, backedChars);
	free(font_data);

	return result;
}


void getLetterFontData(char letter, stbtt_bakedchar* fontData, glm::vec4& mesh, glm::vec4& text)
{
	stbtt_aligned_quad charData;
	float xstart=0, ystart=0;
	stbtt_GetBakedQuad(fontData, 512, 512, letter - 32, &xstart, &ystart, &charData, 1);

	mesh = glm::vec4(charData.x0, charData.y0, charData.x1, charData.y1);
	mesh*=2.f/512; // conversion from bitmap size to frame [-1; 1] size
	text = glm::vec4(charData.s0, charData.t0, charData.s1, charData.t1);
}


std::vector<VkDescriptorSetLayoutBinding> createDescriptorSetLayoutBinding()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding;
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	samplerLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding instanceUBOLayoutBinding{};
	instanceUBOLayoutBinding.binding = 2;
	instanceUBOLayoutBinding.descriptorCount = 1;
	instanceUBOLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	instanceUBOLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	instanceUBOLayoutBinding.pImmutableSamplers = nullptr;

	std::vector<VkDescriptorSetLayoutBinding> layoutBindings = { uboLayoutBinding,  samplerLayoutBinding, instanceUBOLayoutBinding };
	return layoutBindings;
}

std::vector<VkVertexInputBindingDescription> createBindingDescr()
{
	VkVertexInputBindingDescription vertexBindingDescription;
	vertexBindingDescription.binding = 0;
	vertexBindingDescription.stride = sizeof(SgrVertex);
	vertexBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions{ vertexBindingDescription };
	return vertexBindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> createAttrDescr()
{
	VkVertexInputAttributeDescription positionDescr;
	positionDescr.binding = 0;
	positionDescr.location = 0;
	positionDescr.format = VK_FORMAT_R32G32B32_SFLOAT;
	positionDescr.offset = 0;

	std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions;
	vertexAttributeDescriptions.push_back(positionDescr);
	return vertexAttributeDescriptions; 
}

std::vector<VkVertexInputBindingDescription> createBindingDescrModel()
{
	VkVertexInputBindingDescription vertexBindingDescription;
	vertexBindingDescription.binding = 0;
	vertexBindingDescription.stride = sizeof(ModelVertex);
	vertexBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions{ vertexBindingDescription };
	return vertexBindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> createAttrDescrModel()
{
	VkVertexInputAttributeDescription positionDescr;
	positionDescr.binding = 0;
	positionDescr.location = 0;
	positionDescr.format = VK_FORMAT_R32G32B32_SFLOAT;
	positionDescr.offset = offsetof(ModelVertex, vert);

	VkVertexInputAttributeDescription texIndDescr;
	texIndDescr.binding = 0;
	texIndDescr.location = 1;
	texIndDescr.format = VK_FORMAT_R32_SINT;
	texIndDescr.offset = offsetof(ModelVertex, texInd);

	VkVertexInputAttributeDescription texDescr;
	texDescr.binding = 0;
	texDescr.location = 2;
	texDescr.format = VK_FORMAT_R32G32_SFLOAT;
	texDescr.offset = offsetof(ModelVertex, texCoord);

	VkVertexInputAttributeDescription colorDescr;
	colorDescr.binding = 0;
	colorDescr.location = 3;
	colorDescr.format = VK_FORMAT_R32G32B32A32_SFLOAT;
	colorDescr.offset = offsetof(ModelVertex, color);

	std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions;
	vertexAttributeDescriptions.push_back(positionDescr);
	vertexAttributeDescriptions.push_back(texIndDescr);
	vertexAttributeDescriptions.push_back(texDescr);
	vertexAttributeDescriptions.push_back(colorDescr);
	return vertexAttributeDescriptions; 
}

std::vector<VkDescriptorSetLayoutBinding> createDescriptorSetLayoutBindingModel(uint8_t textureCount)
{
	VkDescriptorSetLayoutBinding uboLayoutBinding;
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding instanceUBOLayoutBinding{};
	instanceUBOLayoutBinding.binding = 1;
	instanceUBOLayoutBinding.descriptorCount = 1;
	instanceUBOLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	instanceUBOLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	instanceUBOLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 2;
	samplerLayoutBinding.descriptorCount = textureCount;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	samplerLayoutBinding.pImmutableSamplers = nullptr;

	std::vector<VkDescriptorSetLayoutBinding> layoutBindings = { uboLayoutBinding, instanceUBOLayoutBinding, samplerLayoutBinding };
	return layoutBindings;
}

std::vector<VkVertexInputBindingDescription> createBindingDescrGLTF()
{
	VkVertexInputBindingDescription vertexBindingDescription;
	vertexBindingDescription.binding = 0;
	vertexBindingDescription.stride = sizeof(GLTFVertex);
	vertexBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions{ vertexBindingDescription };
	return vertexBindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> createAttrDescrGLTF()
{
	VkVertexInputAttributeDescription positionDescr;
	positionDescr.binding = 0;
	positionDescr.location = 0;
	positionDescr.format = VK_FORMAT_R32G32B32_SFLOAT;
	positionDescr.offset = offsetof(GLTFVertex, pos);

	VkVertexInputAttributeDescription normalDescr;
	normalDescr.binding = 0;
	normalDescr.location = 1;
	normalDescr.format = VK_FORMAT_R32G32B32_SFLOAT;
	normalDescr.offset = offsetof(GLTFVertex, normal);

	VkVertexInputAttributeDescription uvDescr;
	uvDescr.binding = 0;
	uvDescr.location = 2;
	uvDescr.format = VK_FORMAT_R32G32_SFLOAT;
	uvDescr.offset = offsetof(GLTFVertex, uv);

	VkVertexInputAttributeDescription colorDescr;
	colorDescr.binding = 0;
	colorDescr.location = 3;
	colorDescr.format = VK_FORMAT_R32G32B32_SFLOAT;
	colorDescr.offset = offsetof(GLTFVertex, color);

	std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions;
	vertexAttributeDescriptions.push_back(positionDescr);
	vertexAttributeDescriptions.push_back(normalDescr);
	vertexAttributeDescriptions.push_back(uvDescr);
	vertexAttributeDescriptions.push_back(colorDescr);
	return vertexAttributeDescriptions; 
}

void changeGLTFModel()
{
	for (const auto& prim : gltfPrimitives) {
		ModelInstanceData* data = (ModelInstanceData*)((uint64_t)prim.second.data);
		data->model = glm::rotate(data->model, glm::radians(2.f), glm::vec3(0,1,0));
	}
}

bool registerGLTFModel(std::vector<Node*> nodes, std::vector<SgrImage*> images, SgrBuffer* ubo)
{
	for (const auto& parrent : nodes) {
		for (const auto& node : parrent->children) {
			for (auto& prim : node->mesh.primitives) {
				std::vector<GLTFVertex>& primitiveVerts = prim.verts;
				std::vector<uint32_t>& primitiveInds = prim.inds;
				SGR_CHECK_RES(sgr_object1.addNewObjectGeometry(prim.name, primitiveVerts.data(), primitiveVerts.size() * sizeof(GLTFVertex), primitiveInds,
					resourcePath + "/shaders/vertGLTF.spv",
					resourcePath + "/shaders/fragGLTF.spv",
					true,
					createBindingDescrGLTF(),
					createAttrDescrGLTF(),
					createDescriptorSetLayoutBinding()));

				uint32_t dynAlign = gltfPrimitivesCounter[prim.name]++ * gltfPrimitives[prim.name].dynamicAlignment;
				if (sgr_object1.addObjectInstance(node->name + "_" + prim.name, prim.name, dynAlign) != sgrOK)
					return false;

				ModelInstanceData* data = (ModelInstanceData*)((uint64_t)gltfPrimitives[prim.name].data + dynAlign);

				Node* currentParrent = node->parent;
				while (currentParrent) {
					data->model = currentParrent->matrix * node->matrix;
					currentParrent = currentParrent->parent;
				}

				node->matrix = glm::rotate(node->matrix, glm::radians(180.f), glm::vec3(1, 0, 0));

				std::vector<void*> objData;
				objData.push_back((void*)(ubo));
				objData.push_back((void*)(&images[prim.textureInd]));
				objData.push_back((void*)(gltfPrimitives[prim.name].ubo));
				if (sgr_object1.writeDescriptorSets(node->name + "_" + prim.name, objData) != sgrOK)
					return false;
				if (sgr_object1.drawObject(node->name + "_" + prim.name) != sgrOK)
					return false;
			}
		}
	}

	return true;
}


void updateData() {
	if (getTimeDuration(lastDraw, SgrTime::now()) > 0.1) {
		InstanceData* iData = (InstanceData*)((uint64_t)rectangles.data + 0*rectangles.dynamicAlignment);
		glm::vec2* texCoord = &iData->startText;
		texCoord->x += 0.111f;
		if (texCoord->x >= 1)
			texCoord->x = 0;

		iData->model = glm::translate(iData->model, glm::vec3(0, 0, -0.09));

		ModelInstanceData* iMData = (ModelInstanceData*)((uint64_t)models.data + 0*models.dynamicAlignment);
		iMData->model = glm::rotate(iMData->model, glm::radians(2.f), glm::vec3{0,1,0});

		iMData = (ModelInstanceData*)((uint64_t)models.data + 1*models.dynamicAlignment);
		iMData->model = glm::rotate(iMData->model, glm::radians(-2.f), glm::vec3{ 0,1,0 });

		iMData = (ModelInstanceData*)((uint64_t)models.data + 2*models.dynamicAlignment);
		iMData->model = glm::rotate(iMData->model, glm::radians(2.f), glm::vec3{ 0,0,1 });

		iMData = (ModelInstanceData*)((uint64_t)models.data + 3*models.dynamicAlignment);
		iMData->model = glm::rotate(iMData->model, glm::radians(2.f), glm::vec3{ 0,1,0 });

		changeGLTFModel();

		lastDraw = SgrTime::now();
	}

	sgr_object1.updateGlobalUBO(ubo);
	sgr_object1.updateInstancesUBO(rectangles);
	sgr_object1.updateInstancesUBO(models);
};

bool exitFlag = false;

void exitFunction()
{
	exitFlag = true;
};

uint8_t roadText = 1;
bool roadToggled = false;
void toggleRoadText()
{
	roadToggled = true;

	if (roadText == 1)
		roadText = 2;
	else
		roadText = 1;
}

bool textHided = false;
bool toggleTextFlag = false;
void toggleText()
{
	toggleTextFlag = true;

	if (textHided)
		textHided = false;
	else
		textHided = true;
}

int main()
{
	// This source code is example for using SGR library
	
	SGR_CHECK_RES(sgr_object1.init());

	std::string executablePath = getExecutablePath();
	if (executablePath.length() == 0)
		return 11;

#if __APPLE__ && !NDBUG
	resourcePath = executablePath + "/../Resources";
#else
	resourcePath = executablePath + "/Resources";
#endif


	sgr_object1.setApplicationLogo(resourcePath + "/logo/Logo64.png");

	// scheme to setup 2D-object layout:
	// 1. setup geometry (mesh)
	// 2. setup minimum two shaders: vertex and fragment
	
	// then we need to setup uniform buffer pointer for all objects same mesh
	// setup texture for each object

	// create geometry layout (mesh)

	// rectangle mesh
	std::vector<uint32_t> obMeshIndices = { 0, 1, 2, 2, 3, 0 };
	std::vector<SgrVertex> obMeshVertices = {{-0.5f, -0.5f, 0},
											 { 0.5f, -0.5f, 0},
											 { 0.5f,  0.5f, 0},
											 {-0.5f,  0.5f, 0}};

	// trianle mesh
	std::vector<uint32_t> obMeshIndices1 = { 0, 1, 2 };
	std::vector<SgrVertex> obMeshVertices1 = {{-0.5f, -0.5f, 0},
											  { 0.5f, -0.5f, 0},	
											  { 0.5f,  0.5f, 0}};


	// simple shaders
	std::string obShaderVert = resourcePath + "/shaders/vertInstanceSh.spv";
	std::string obShaderFrag = resourcePath + "/shaders/fragTextureSh.spv";
	std::string obShaderFragColor = resourcePath + "/shaders/fragColorSh.spv";

	// model shaders
	std::string obModelShaderVert = resourcePath + "/shaders/vertObjModel.spv";
	std::string obModelShaderFrag = resourcePath + "/shaders/fragObjModel.spv";

	// new object layout creating done. Next step - setup this data to SGR and add command to draw.
	std::vector<VkDescriptorSetLayoutBinding> setLayoutBinding = createDescriptorSetLayoutBinding();
	std::vector<VkVertexInputBindingDescription> bindInpDescr = createBindingDescr();
	std::vector<VkVertexInputAttributeDescription> attDescr = createAttrDescr();

	rectangles.instnaceCount = 4;
	rectangles.instanceSize = sizeof(InstanceData);
	if (MemoryManager::createDynamicUniformMemory(rectangles) != sgrOK)
		return 0;
	SGR_CHECK_RES(MemoryManager::get()->createDynamicUniformBuffer(rectangles.ubo, rectangles.dataSize, rectangles.dynamicAlignment));


	models.instnaceCount = 4;
	models.instanceSize = sizeof(ModelInstanceData);
	if (MemoryManager::createDynamicUniformMemory(models) != sgrOK)
		return 10;
	SGR_CHECK_RES(MemoryManager::get()->createDynamicUniformBuffer(models.ubo, models.dataSize, models.dynamicAlignment));

	SgrBuffer* uboBuffer = nullptr;
	SGR_CHECK_RES(MemoryManager::get()->createUniformBuffer(uboBuffer, sizeof(SgrGlobalUBO)));

	SGR_CHECK_RES(sgr_object1.addNewObjectGeometry("rectangle", obMeshVertices.data(), obMeshVertices.size() * sizeof(SgrVertex), obMeshIndices, obShaderVert, obShaderFrag, true, bindInpDescr, attDescr, setLayoutBinding));

	// add instance man
	sgr_object1.addObjectInstance("man","rectangle",static_cast<uint32_t>(0*rectangles.dynamicAlignment));

	SgrImage* texture1 = nullptr;
	SGR_CHECK_RES(TextureManager::createTextureImage(resourcePath + "/textures/man.png", texture1));

	std::vector<void*> objectData;
	objectData.push_back((void*)(uboBuffer));
	objectData.push_back((void*)(&texture1));
	objectData.push_back((void*)(rectangles.ubo));
	sgr_object1.writeDescriptorSets("man", objectData);


	// new geometry
	SGR_CHECK_RES(sgr_object1.addNewObjectGeometry("triangle", obMeshVertices1.data(), obMeshVertices1.size() * sizeof(SgrVertex), obMeshIndices1, obShaderVert, obShaderFragColor, false, bindInpDescr, attDescr, setLayoutBinding));


	sgr_object1.addObjectInstance("tree","triangle",static_cast<uint32_t>(1*rectangles.dynamicAlignment));

	objectData.clear();
	objectData.push_back((void*)(uboBuffer));
	objectData.push_back((void*)(&texture1));
	objectData.push_back((void*)(rectangles.ubo));
	sgr_object1.writeDescriptorSets("tree", objectData);

	// add instance road
	sgr_object1.addObjectInstance("road","rectangle",static_cast<uint32_t>(2*rectangles.dynamicAlignment));

	SgrImage* texture2 = nullptr;
	SGR_CHECK_RES(TextureManager::createTextureImage(resourcePath + "/textures/road.png", texture2));

	objectData.clear();
	objectData.push_back((void*)(uboBuffer));
	objectData.push_back((void*)(&texture2));
	objectData.push_back((void*)(rectangles.ubo));
	sgr_object1.writeDescriptorSets("road", objectData);


	// add geometry and texture for letter
	SgrImage* textImage = nullptr;
	unsigned char* fontPixels = nullptr;
	stbtt_bakedchar* fontData;
	uint32_t pixelsWidth, pixelsHeight;
	std::string font_path = resourcePath + "/fonts/times new roman.ttf";
	getFontData(font_path, fontPixels, fontData, pixelsWidth, pixelsHeight);
	SgrErrCode resultCreateTextureImage = TextureManager::createFontTextureImage(fontPixels, pixelsWidth, pixelsHeight, textImage);
	if (resultCreateTextureImage != sgrOK) {
		free(fontData);
		return resultCreateTextureImage;
	}

	glm::vec4 meshLetter;
	glm::vec4 textLetter;
	getLetterFontData('S', fontData,meshLetter,textLetter);
	std::vector<SgrVertex> letterMesh = {{meshLetter.x, meshLetter.y, 0},
										 {meshLetter.z, meshLetter.y, 0},
										 {meshLetter.z, meshLetter.w, 0},
										 {meshLetter.x, meshLetter.w, 0}};
	free(fontData);

	SGR_CHECK_RES(sgr_object1.addNewObjectGeometry("letterMesh", letterMesh.data(), letterMesh.size() * sizeof(SgrVertex), obMeshIndices, obShaderVert, obShaderFrag, true, bindInpDescr, attDescr, setLayoutBinding));
	glm::vec2 letStartMesh(meshLetter.x, meshLetter.y);
	glm::vec2 letStartText(textLetter.x, textLetter.y);
	glm::vec2 deltaText;
	deltaText.x = (textLetter.z - textLetter.x) / (meshLetter.z - meshLetter.x);
	deltaText.t = (textLetter.w - textLetter.y) / (meshLetter.w - meshLetter.y);
	
	sgr_object1.addObjectInstance("letter", "letterMesh", static_cast<uint32_t>(3*rectangles.dynamicAlignment));

	objectData.clear();
	objectData.push_back((void*)(uboBuffer));
	objectData.push_back((void*)(&textImage));
	objectData.push_back((void*)(rectangles.ubo));
	sgr_object1.writeDescriptorSets("letter", objectData);


	// load models

	// load car model
	std::vector<ModelVertex> audiVerts;
	std::vector<uint32_t> audiInds;
	std::vector<SgrImage*> audiTextures;
	if (!loadObjectModel(resourcePath + "/3d_models/Audi_S5_Sportback", "Audi_S5_Sportback", audiVerts, audiInds, audiTextures))
		return 666;


	SGR_CHECK_RES(sgr_object1.addNewObjectGeometry("car", audiVerts.data(), audiVerts.size() * sizeof(ModelVertex), audiInds,
																						obModelShaderVert,
																						obModelShaderFrag,
																						true,
																						createBindingDescrModel(),
																						createAttrDescrModel(),
																						createDescriptorSetLayoutBindingModel((uint8_t)audiTextures.size())));


	// add model instance 1
	sgr_object1.addObjectInstance("audi1", "car", static_cast<uint32_t>(0*models.dynamicAlignment));
	objectData.clear();
	objectData.push_back((void*)(uboBuffer));
	objectData.push_back((void*)(models.ubo));
	objectData.push_back((void*)(audiTextures.data()));
	sgr_object1.writeDescriptorSets("audi1", objectData);

	// add model instance 2
	sgr_object1.addObjectInstance("audi2", "car", static_cast<uint32_t>(1*models.dynamicAlignment));
	objectData.clear();
	objectData.push_back((void*)(uboBuffer));
	objectData.push_back((void*)(models.ubo));
	objectData.push_back((void*)(audiTextures.data()));
	sgr_object1.writeDescriptorSets("audi2", objectData);


	// load room model
	std::vector<ModelVertex> roomVerts;
	std::vector<uint32_t> roomInds;
	std::vector<SgrImage*> roomTextures;
	if (!loadObjectModel(resourcePath + "/3d_models/viking_room", "viking_room", roomVerts, roomInds, roomTextures))
		return 777;

	SGR_CHECK_RES(sgr_object1.addNewObjectGeometry("room", roomVerts.data(), roomVerts.size() * sizeof(ModelVertex), roomInds,
																							obModelShaderVert,
																							obModelShaderFrag,
																							true,
																							createBindingDescrModel(),
																							createAttrDescrModel(),
																							createDescriptorSetLayoutBindingModel((uint8_t)roomTextures.size())));


	// add room instance
	sgr_object1.addObjectInstance("viking_room", "room", static_cast<uint32_t>(2*models.dynamicAlignment));
	objectData.clear();
	objectData.push_back((void*)(uboBuffer));
	objectData.push_back((void*)(models.ubo));
	objectData.push_back((void*)(roomTextures.data()));
	sgr_object1.writeDescriptorSets("viking_room", objectData);


	std::vector<ModelVertex> bmwVerts;
	std::vector<uint32_t> bmwInds;
	std::vector<SgrImage*> bmwTextures;
	if (!loadObjectModel(resourcePath + "/3d_models/bmw_m3", "bmw_m3", bmwVerts, bmwInds, bmwTextures))
		return 888;

	SGR_CHECK_RES(sgr_object1.addNewObjectGeometry("bmw", bmwVerts.data(), bmwVerts.size() * sizeof(ModelVertex), bmwInds,
																							obModelShaderVert,
																							obModelShaderFrag,
																							true,
																							createBindingDescrModel(),
																							createAttrDescrModel(),
																							createDescriptorSetLayoutBindingModel((uint8_t)bmwTextures.size())));

	sgr_object1.addObjectInstance("bmwm3", "bmw", static_cast<uint32_t>(3*models.dynamicAlignment));
	objectData.clear();
	objectData.push_back((void*)(uboBuffer));
	objectData.push_back((void*)(models.ubo));
	objectData.push_back((void*)(bmwTextures.data()));
	sgr_object1.writeDescriptorSets("bmwm3", objectData);


	std::vector<Node*> helmetNodes;
	std::vector<SgrImage*> helmetImages;
	if (!loadGLTFModel(resourcePath + "/3d_models/FlightHelmet/FlightHelmet.gltf", helmetNodes, helmetImages))
		return 999;

	// now register all objects

	if (sgr_object1.drawObject("man") != sgrOK)
		return 100;

	if (sgr_object1.drawObject("tree") != sgrOK)
		return 200;

	if (sgr_object1.drawObject("road") != sgrOK)
		return 300;

	if (sgr_object1.drawObject("letter") != sgrOK)
		return 400;

	if (sgr_object1.drawObject("audi1") != sgrOK)
		return 500;

	if (sgr_object1.drawObject("audi2") != sgrOK)
	 	return 600;

	if (sgr_object1.drawObject("viking_room") != sgrOK)
		return 700;

	if (sgr_object1.drawObject("bmwm3") != sgrOK)
		return 800;

	if (!registerGLTFModel(helmetNodes, helmetImages, uboBuffer))
		return 900;


	sgr_object1.setupGlobalUBO(uboBuffer);


// MAN
	InstanceData* iData = (InstanceData*)((uint64_t)rectangles.data + 0*rectangles.dynamicAlignment);
	iData->model = glm::mat4(1.f);
	iData->model = glm::translate(iData->model, glm::vec3(0.3, 0, 0));
	iData->startMesh = glm::vec2(-0.5,-0.5);
	iData->startText = glm::vec2(0,0);
	iData->deltaText.x = (0.111f - 0) / (0.5 - -0.5);
	iData->deltaText.y = (0.250f - 0) / (0.5 - -0.5);

// TRIANGLE
	iData = (InstanceData*)((uint64_t)rectangles.data + 1*rectangles.dynamicAlignment);
	iData->model = glm::mat4(1.f);
	iData->model = glm::translate(iData->model,glm::vec3(1.5,-1.5,-3));
	iData->color.r = 1;
	iData->color.g = 0;
	iData->color.b = 0;

// ROAD
	iData = (InstanceData*)((uint64_t)rectangles.data + 2*rectangles.dynamicAlignment);
	iData->model = glm::mat4(1.f);
	iData->model = glm::translate(iData->model,glm::vec3(-2.5,-2.2,-5));
	iData->model = glm::scale(iData->model,glm::vec3(1.f,1.f,1.f));
	iData->startMesh = glm::vec2(-0.5,-0.5);
	iData->startText = glm::vec2(0,0);
	iData->deltaText.x = (1 - 0) / (0.5 - -0.5);
	iData->deltaText.y = (1 - 0) / (0.5 - -0.5);

// LETTER
	InstanceData* current = (InstanceData*)((uint64_t)rectangles.data + 3*rectangles.dynamicAlignment);
	current->model = glm::mat4(1.f);
	current->model = glm::translate(current->model,glm::vec3(0,1.5,-3));
	current->startMesh = letStartMesh;
	current->startText = letStartText;
	current->deltaText = deltaText;

// 3D Models
	ModelInstanceData* iMData = (ModelInstanceData*)((uint64_t)models.data + 0*models.dynamicAlignment);
	iMData->model = glm::mat4(1.f);
	iMData->model = glm::scale(iMData->model,{0.03,0.03,0.03});
	iMData->model = glm::rotate(iMData->model,glm::radians(180.f),{0,0,1});
	iMData->model = glm::translate(iMData->model,{0,8,0});

	iMData = (ModelInstanceData*)((uint64_t)models.data + 1*models.dynamicAlignment);
	iMData->model = glm::mat4(1.f);
	iMData->model = glm::scale(iMData->model, { 0.03,0.03,0.03 });
	iMData->model = glm::rotate(iMData->model, glm::radians(180.f), { 0,0,1 });
	iMData->model = glm::translate(iMData->model, { 8,-8,0 });

	//viking room
	iMData = (ModelInstanceData*)((uint64_t)models.data + 2*models.dynamicAlignment);
	iMData->model = glm::mat4(1.f);
	iMData->model = glm::scale(iMData->model, { 0.3,0.3,0.3 });
	iMData->model = glm::translate(iMData->model, {1.5,1.5,-2});
	iMData->model = glm::rotate(iMData->model, glm::radians(90.f), { 1,0,0 });

	iMData = (ModelInstanceData*)((uint64_t)models.data + 3*models.dynamicAlignment);
	iMData->model = glm::mat4(1.f);
	iMData->model = glm::scale(iMData->model, { 0.02,0.02,0.02 });
	iMData->model = glm::translate(iMData->model, {-10,0,0});
	iMData->model = glm::rotate(iMData->model, glm::radians(180.f), { 0,0,1 });
	

	sgr_object1.updateInstancesUBO(rectangles);
	sgr_object1.updateInstancesUBO(models);

	for (auto& instUBO : gltfPrimitives)
		sgr_object1.updateInstancesUBO(instUBO.second);

	ubo.view = glm::translate(ubo.view, glm::vec3(0, 0, -1));
	ubo.proj = glm::perspective(45.f, 1.f/1.f, 0.1f, 100.f);
	sgr_object1.updateGlobalUBO(ubo);

	// add UI
	SgrUIButton exitButton("Button1", {0.9, 0.9}, exitFunction, "Exit");
	exitButton._size = {50,50};
	SgrUIButton b1("ButtonC1", {0.1, 0.3}, toggleRoadText, "Change road texture");

	SgrUIButton hideT("HideText", {0.5, 0.87}, toggleText, "Hide");
	hideT._size = { 50,20 };
	SgrUIText t1("Text1", {0.5, 0.9}, "Simple Graphics Renderer");

	sgr_object1.drawUIElement(exitButton);
	sgr_object1.drawUIElement(b1);
	sgr_object1.drawUIElement(t1);
	sgr_object1.drawUIElement(hideT);

	sgr_object1.setUpdateFunction(updateData);
	sgr_object1.setupUICallback();
	while (!exitFlag) {
		if (!sgr_object1.isSGRRunning())
			break;

		SGR_CHECK_RES(sgr_object1.drawFrame());

		if (toggleTextFlag) {
			t1._visible= !textHided;
			toggleTextFlag = false;
			if (textHided)
				hideT.changeText("Show");
			else
				hideT.changeText("Hide");
		}

		if (roadToggled) {
			roadToggled = false;
			objectData.clear();
			objectData.push_back((void*)(uboBuffer));
			if (roadText == 1) objectData.push_back((void*)(&texture2));
			else objectData.push_back((void*)(&texture1));
			objectData.push_back((void*)(rectangles.ubo));
			sgr_object1.writeDescriptorSets("road", objectData);
		}
	}

	SgrErrCode resultSGRDestroy = sgr_object1.destroy();
	return resultSGRDestroy;
}


#if _WIN64
	#include "Windows.h"


	int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
		return main();
	}

#endif