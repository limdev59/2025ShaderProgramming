#include "stdafx.h"
#include "Renderer.h"
#include "Dependencies\freeglut.h"
#include "LoadPng.h"

Renderer::Renderer(int windowSizeX, int windowSizeY)
{
	Initialize(windowSizeX, windowSizeY);
}

Renderer::~Renderer()
{
}

GLuint Renderer::CreatePngTexture(char* filePath, GLuint samplingMethod)
{
	//Load Png
	std::vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, filePath);
	if (error != 0)
	{
		std::cout << "PNG image loading failed:" << filePath << std::endl;
		assert(0);
	}

	GLuint temp;
	glGenTextures(1, &temp);
	glBindTexture(GL_TEXTURE_2D, temp);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, &image[0]);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, samplingMethod);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, samplingMethod);

	return temp;
}


//
void Renderer::Initialize(int windowSizeX, int windowSizeY)
{
	//Set window size
	m_WindowSizeX = windowSizeX;
	m_WindowSizeY = windowSizeY;

	m_RampPositions[0] = 0.0f;
	m_RampPositions[1] = 0.3f;
	m_RampPositions[2] = 0.7f;
	m_RampPositions[3] = 1.0f;

	// Colors (R, G, B)
	m_RampColors[0][0] = 0.0f; m_RampColors[0][1] = 0.0f; m_RampColors[0][2] = 0.0f; // Black
	m_RampColors[1][0] = 0.0f; m_RampColors[1][1] = 0.1f; m_RampColors[1][2] = 0.8f; // Blue
	m_RampColors[2][0] = 0.9f; m_RampColors[2][1] = 1.0f; m_RampColors[2][2] = 0.2f; // Yellow
	m_RampColors[3][0] = 1.0f; m_RampColors[3][1] = 1.0f; m_RampColors[3][2] = 1.0f; // White

	//셰이더 컴파일
	CompileAllShaderPrograms();

	//Create VBOs
	CreateVertexBufferObjects();

	CreateVertexBufferObjects();

	GenerateParticles(10000);

	CreateGridMesh(100, 100);

	m_0Texture = CreatePngTexture("./0.png", GL_NEAREST);
	m_1Texture = CreatePngTexture("./1.png", GL_NEAREST);
	m_2Texture = CreatePngTexture("./2.png", GL_NEAREST);
	m_3Texture = CreatePngTexture("./3.png", GL_NEAREST);
	m_4Texture = CreatePngTexture("./4.png", GL_NEAREST);
	m_5Texture = CreatePngTexture("./5.png", GL_NEAREST);
	m_6Texture = CreatePngTexture("./6.png", GL_NEAREST);
	m_7Texture = CreatePngTexture("./7.png", GL_NEAREST);
	m_8Texture = CreatePngTexture("./8.png", GL_NEAREST);
	m_9Texture = CreatePngTexture("./9.png", GL_NEAREST);
	m_RGBTexture = CreatePngTexture("./rgb.png", GL_NEAREST);
	m_IVETexture = CreatePngTexture("./cj.png", GL_NEAREST);
	m_NUMTexture = CreatePngTexture("./numbers.png", GL_NEAREST);
	m_ParticleTexture = CreatePngTexture("./particles.png", GL_LINEAR);

	CreateFBOs();

	int index = 0;
	for (int i{}; i < 100; ++i) {
		float x = (float)rand() / (float)RAND_MAX - 1.f;
		float y = (float)rand() / (float)RAND_MAX - 1.f;
		float st = 10 * (float)rand() / (float)RAND_MAX;
		float lt = 10 * (float)rand() / (float)RAND_MAX;
		m_Points[index++] = x;
		m_Points[index++] = y;
		m_Points[index++] = st;
		m_Points[index++] = lt;
	}


	if (m_SolidRectShader > 0 && m_VBORect > 0)
	{
		m_Initialized = true;
	}
}

void Renderer::CompileAllShaderPrograms()
{
	//Load shaders
	m_SolidRectShader = CompileShaders(
		"./Shaders/SolidRect.vs",
		"./Shaders/SolidRect.fs");

	m_TestShader = CompileShaders(
		"./Shaders/Test.vs",
		"./Shaders/Test.fs");

	m_ParticleShader = CompileShaders(
		"./Shaders/Particle.vs",
		"./Shaders/Particle.fs");

	m_GridMeshVertexShader = CompileShaders(
		"./Shaders/GridMesh.vs",
		"./Shaders/GridMesh.fs");

	m_FullScreenShader = CompileShaders(
		"./Shaders/FullScreen.vs",
		"./Shaders/FullScreen.fs");

	m_FSShader = CompileShaders(
		"./Shaders/FS.vs",
		"./Shaders/FS.fs");

	m_TexShader = CompileShaders(
		"./Shaders/Texture.vs",
		"./Shaders/Texture.fs");
	
	// 기존 쉐이더 컴파일 아래에 추가
	m_GaussianBlurHShader = CompileShaders("./Shaders/Blur.vs", "./Shaders/BlurHorizontal.fs");
	m_GaussianBlurVShader = CompileShaders("./Shaders/Blur.vs", "./Shaders/BlurVertical.fs");
	m_DrawMergeTextureShader = CompileShaders("./Shaders/Blur.vs", "./Shaders/MergeBloom.fs");
}

void Renderer::DeleteAllShaderPrograms()
{
	glDeleteShader(m_SolidRectShader);
	glDeleteShader(m_TestShader);
	glDeleteShader(m_ParticleShader);
	glDeleteShader(m_GridMeshVertexShader);
	glDeleteShader(m_FullScreenShader);
	glDeleteShader(m_FSShader);
	glDeleteShader(m_TexShader);
}


bool Renderer::IsInitialized()
{
	return m_Initialized;
}

void Renderer::ReloadAllShaderPrograms()
{
	DeleteAllShaderPrograms();
	CompileAllShaderPrograms();
}

//
void Renderer::CreateVertexBufferObjects()
{
	float rect[]
		=
	{
		-1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f, -1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, 1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, //Triangle1
		-1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f,  1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, 1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f, //Triangle2
	};

	glGenBuffers(1, &m_VBORect);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);

	// lecture2


	float temp = 0.5f;
	float size = 0.01f;
	float testPos[]
		=
	{
		(0.f - temp) * size, (0.f - temp) * size, 0.f, 0.5,
		(1.f - temp) * size, (0.f - temp) * size, 0.f, 0.5,
		(1.f - temp) * size, (1.f - temp) * size, 0.f, 0.5,
		(0.f - temp) * size, (0.f - temp) * size, 0.f, 0.5,
		(1.f - temp) * size, (1.f - temp) * size, 1.f, 0.5,
		(0.f - temp) * size, (1.f - temp) * size, 0.f, 0.5,	// Quard1

		(0.f - temp) * size, (0.f - temp) * size, 0.f, 1.f,
		(1.f - temp) * size, (0.f - temp) * size, 0.f, 1.f,
		(1.f - temp) * size, (1.f - temp) * size, 0.f, 1.f,
		(0.f - temp) * size, (0.f - temp) * size, 0.f, 1.f,
		(1.f - temp) * size, (1.f - temp) * size, 1.f, 1.f,
		(0.f - temp) * size, (1.f - temp) * size, 0.f, 1.f,	// Quard2
	};

	glGenBuffers(1, &m_VBOTestPos);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOTestPos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(testPos), testPos, GL_STATIC_DRAW);


	float testColor[]
		=
	{
		1.f, 0.f, 0.f, 1.f,
		0.f, 1.f, 0.f, 1.f,
		0.f, 0.f, 1.f, 1.f,	//Triangle1
		1.f, 0.f, 0.f, 1.f,
		0.f, 1.f, 0.f, 1.f,
		0.f, 0.f, 1.f, 1.f,	//Triangle2

		1.f, 0.f, 0.f, 1.f,
		0.f, 1.f, 0.f, 1.f,
		0.f, 0.f, 1.f, 1.f,	//Triangle1
		1.f, 0.f, 0.f, 1.f,
		0.f, 1.f, 0.f, 1.f,
		0.f, 0.f, 1.f, 1.f	//Triangle2
	};

	glGenBuffers(1, &m_VBOTestColor);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOTestColor);
	glBufferData(GL_ARRAY_BUFFER, sizeof(testColor), testColor, GL_STATIC_DRAW);

	float fullRect[]
		=
	{
		-1.f , -1.f , 0.f, -1.f , 1.f , 0.f, 1.f , 1.f , 0.f,
		-1.f , -1.f , 0.f,  1.f , 1.f , 0.f, 1.f , -1.f , 0.f,
	};

	glGenBuffers(1, &m_VBOFullScreen);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOFullScreen);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fullRect),
		fullRect, GL_STATIC_DRAW);

	float rect2[]
		=
	{
		-1.f , -1.f , 0.f, -1.f , 1.f , 0.f, 1.f , 1.f , 0.f,
		-1.f , -1.f , 0.f,  1.f , 1.f , 0.f, 1.f , -1.f , 0.f,
	};

	glGenBuffers(1, &m_VBOFS);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOFS);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect2), rect2, GL_STATIC_DRAW);


	float fullRect2[]
		=
	{
		-1.f,	-1.f,	0.f,	0.f,	1.f,
		-1.f,	1.f,	0.f,	0.f,	0.f,
		1.f,	1.f,	0.f,	1.f,	0.f,
		-1.f,	-1.f,	0.f,	0.f,	1.f,
		1.f,	-1.f,	0.f,	1.f,	1.f,
		1.f ,	1.f,	0.f,	1.f,	0.f
	};

	glGenBuffers(1, &m_TexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_TexVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fullRect2),
		fullRect2, GL_STATIC_DRAW);
}



void Renderer::CreateGridMesh(int x, int y)
{
	float basePosX = -1.f;
	float basePosY = -1.f;
	float targetPosX = 1.f;
	float targetPosY = 1.f;

	int pointCountX = x;
	int pointCountY = y;

	float width = targetPosX - basePosX;
	float height = targetPosY - basePosY;

	// 정점 당 데이터: 위치(x,y,z), 노멀(nx,ny,nz), UV(u,v) -> 총 8개의 float
	int floatsPerVertex = 8;
	float* vertices = new float[(pointCountX - 1) * (pointCountY - 1) * 2 * 3 * floatsPerVertex];
	m_GridMeshVertexCount = (pointCountX - 1) * (pointCountY - 1) * 2 * 3;

	int vertIndex = 0;
	for (int ix = 0; ix < pointCountX - 1; ix++)
	{
		for (int iy = 0; iy < pointCountY - 1; iy++)
		{
			// 네 꼭짓점의 위치와 UV 계산
			float x1 = basePosX + width * (ix / (float)(pointCountX - 1));
			float y1 = basePosY + height * (iy / (float)(pointCountY - 1));
			float u1 = (float)ix / (pointCountX - 1);
			float v1 = (float)iy / (pointCountY - 1);

			float x2 = basePosX + width * ((ix + 1) / (float)(pointCountX - 1));
			float y2 = basePosY + height * (iy / (float)(pointCountY - 1));
			float u2 = (float)(ix + 1) / (pointCountX - 1);
			float v2 = (float)iy / (pointCountY - 1);

			float x3 = basePosX + width * (ix / (float)(pointCountX - 1));
			float y3 = basePosY + height * ((iy + 1) / (float)(pointCountY - 1));
			float u3 = (float)ix / (pointCountX - 1);
			float v3 = (float)(iy + 1) / (pointCountY - 1);

			float x4 = basePosX + width * ((ix + 1) / (float)(pointCountX - 1));
			float y4 = basePosY + height * ((iy + 1) / (float)(pointCountY - 1));
			float u4 = (float)(ix + 1) / (pointCountX - 1);
			float v4 = (float)(iy + 1) / (pointCountY - 1);

			// 정점 데이터 (위치, 노멀, UV)
			float verts[] = {
				// 삼각형 1
				x1, y1, 0.f,  0.f, 0.f, 1.f,  u1, v1,
				x3, y3, 0.f,  0.f, 0.f, 1.f,  u3, v3,
				x4, y4, 0.f,  0.f, 0.f, 1.f,  u4, v4,
				// 삼각형 2
				x1, y1, 0.f,  0.f, 0.f, 1.f,  u1, v1,
				x4, y4, 0.f,  0.f, 0.f, 1.f,  u4, v4,
				x2, y2, 0.f,  0.f, 0.f, 1.f,  u2, v2
			};

			memcpy(&vertices[vertIndex], verts, sizeof(verts));
			vertIndex += sizeof(verts) / sizeof(float);
		}
	}

	glGenBuffers(1, &m_GridMeshVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_GridMeshVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_GridMeshVertexCount * floatsPerVertex, vertices, GL_STATIC_DRAW);

	delete[] vertices;
}

void Renderer::GenerateParticles(int numParticle)
{
	int floatCountPervertex = 3 + 1 + 4 + 1 + 3 + 1 + 1 + 1 + 2; // x, y, z, value, r, g, b, a, sTime, Velocity, Lifetime, mass
	int verticesCountPerParticle = 6;
	int floatCountPerParticle = floatCountPervertex * verticesCountPerParticle;
	int totalVerticesCount = numParticle * verticesCountPerParticle;
	int totalFloatCount = floatCountPervertex * totalVerticesCount;

	float* vertices = new float[totalFloatCount];

	for (int i = 0; i < numParticle; ++i) {
		float x, y, z;
		x = 0;// (float)rand() / (float)RAND_MAX * 2.f - 1.f;
		y = 0;// (float)rand() / (float)RAND_MAX * 2.f - 1.f;
		z = 0.f;

		float value;
		value = (float)rand() / (float)RAND_MAX;

		float intensity = 4.0f;
		float r, g, b, a;
		r = (float)rand() / (float)RAND_MAX * intensity;
		g = (float)rand() / (float)RAND_MAX * intensity;
		b = (float)rand() / (float)RAND_MAX * intensity;
		a = (float)rand() / (float)RAND_MAX;

		float size;
		size = 0.05f + ((float)rand() / (float)RAND_MAX) * 0.05f;

		float sTime;
		sTime = ((float)rand() / (float)RAND_MAX) * 2.0f;

		float vx, vy, vz;
		vx = 0; //((float)rand() / (float)RAND_MAX * 2.f - 1.f) * 1.0f;
		vy = 0; //(((float)rand() / (float)RAND_MAX * 2.f - 1.f) + 1.5f) * 0.8f;
		vz = 0.f;

		float lifeTime;
		lifeTime = ((float)rand() / (float)RAND_MAX);

		float mass;
		mass = ((float)rand() / (float)RAND_MAX) + 1;

		float period;
		period = (float)rand() / (float)RAND_MAX + 1.f; (float)rand() / (float)RAND_MAX + 1.f;

		int index = i * floatCountPervertex * verticesCountPerParticle;

		// V1 (Left-Bottom)
        vertices[index++] = x - size; vertices[index++] = y - size; vertices[index++] = z;
        vertices[index++] = value;
        vertices[index++] = r; vertices[index++] = g; vertices[index++] = b; vertices[index++] = a;
        vertices[index++] = sTime;
        vertices[index++] = vx; vertices[index++] = vy; vertices[index++] = vz;
        vertices[index++] = lifeTime;
        vertices[index++] = mass;
        vertices[index++] = period;
        vertices[index++] = 0.0f; vertices[index++] = 0.0f; // [추가] UV (0, 0)

        // V2 (Right-Bottom)
        vertices[index++] = x + size; vertices[index++] = y - size; vertices[index++] = z;
        vertices[index++] = value;
        vertices[index++] = r; vertices[index++] = g; vertices[index++] = b; vertices[index++] = a;
        vertices[index++] = sTime;
        vertices[index++] = vx; vertices[index++] = vy; vertices[index++] = vz;
        vertices[index++] = lifeTime;
        vertices[index++] = mass;
        vertices[index++] = period;
        vertices[index++] = 1.0f; vertices[index++] = 0.0f; // [추가] UV (1, 0)

        // V3 (Right-Top)
        vertices[index++] = x + size; vertices[index++] = y + size; vertices[index++] = z;
        vertices[index++] = value;
        vertices[index++] = r; vertices[index++] = g; vertices[index++] = b; vertices[index++] = a;
        vertices[index++] = sTime;
        vertices[index++] = vx; vertices[index++] = vy; vertices[index++] = vz;
        vertices[index++] = lifeTime;
        vertices[index++] = mass;
        vertices[index++] = period;
        vertices[index++] = 1.0f; vertices[index++] = 1.0f; // [추가] UV (1, 1)

        // V4 (Left-Top)
        vertices[index++] = x - size; vertices[index++] = y + size; vertices[index++] = z;
        vertices[index++] = value;
        vertices[index++] = r; vertices[index++] = g; vertices[index++] = b; vertices[index++] = a;
        vertices[index++] = sTime;
        vertices[index++] = vx; vertices[index++] = vy; vertices[index++] = vz;
        vertices[index++] = lifeTime;
        vertices[index++] = mass;
        vertices[index++] = period;
        vertices[index++] = 0.0f; vertices[index++] = 1.0f; // [추가] UV (0, 1)

        // V5 (Left-Bottom) - V1과 동일
        vertices[index++] = x - size; vertices[index++] = y - size; vertices[index++] = z;
        vertices[index++] = value;
        vertices[index++] = r; vertices[index++] = g; vertices[index++] = b; vertices[index++] = a;
        vertices[index++] = sTime;
        vertices[index++] = vx; vertices[index++] = vy; vertices[index++] = vz;
        vertices[index++] = lifeTime;
        vertices[index++] = mass;
        vertices[index++] = period;
        vertices[index++] = 0.0f; vertices[index++] = 0.0f; // [추가] UV (0, 0)

        // V6 (Right-Top) - V3와 동일
        vertices[index++] = x + size; vertices[index++] = y + size; vertices[index++] = z;
        vertices[index++] = value;
        vertices[index++] = r; vertices[index++] = g; vertices[index++] = b; vertices[index++] = a;
        vertices[index++] = sTime;
        vertices[index++] = vx; vertices[index++] = vy; vertices[index++] = vz;
        vertices[index++] = lifeTime;
        vertices[index++] = mass;
        vertices[index++] = period;
        vertices[index++] = 1.0f; vertices[index++] = 1.0f;
	}

	glGenBuffers(1, &m_VBOParticle);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOParticle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * totalFloatCount, vertices, GL_STATIC_DRAW);

	delete[] vertices;
	m_VBOParticleVertexCount = totalVerticesCount;
}

void Renderer::CreateFBOs()
{
	{
		GLuint textureId;
		glGenTextures(1, &m_RT0_0);
		glBindTexture(GL_TEXTURE_2D, m_RT0_0);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

		GLuint depthBuffer;
		glGenRenderbuffers(1, &depthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 512, 512);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glGenFramebuffers(1, &m_FBO0);

		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, m_RT0_0, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_RENDERBUFFER, depthBuffer);

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			assert(0);
		}
	}
	{
		GLuint textureId;
		glGenTextures(1, &m_RT1_0);
		glBindTexture(GL_TEXTURE_2D, m_RT1_0);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

		GLuint depthBuffer;
		glGenRenderbuffers(1, &depthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 512, 512);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glGenFramebuffers(1, &m_FBO1);

		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, m_RT1_0, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_RENDERBUFFER, depthBuffer);

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			assert(1);
		}
	}

	glGenFramebuffers(1, &m_HDRFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_HDRFBO);

    // [Attachment 1] High Texture (밝은 부분) - 부동소수점(GL_RGBA16F) 사용 필수 [cite: 26]
    glGenTextures(1, &m_HDRHighTexture);
    glBindTexture(GL_TEXTURE_2D, m_HDRHighTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_WindowSizeX, m_WindowSizeY, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // [cite: 29]
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // [Attachment 0] Low Texture (일반 부분) [cite: 31]
    glGenTextures(1, &m_HDRLowTexture);
    glBindTexture(GL_TEXTURE_2D, m_HDRLowTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_WindowSizeX, m_WindowSizeY, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // 두 개의 텍스처를 FBO에 붙임 [cite: 38, 39]
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_HDRLowTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_HDRHighTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "HDR FBO Failed" << std::endl;

    // 2. Ping-Pong FBO (블러용) [cite: 76]
    glGenFramebuffers(2, m_PingpongFBO);
    glGenTextures(2, m_PingpongTexture);
    for (int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_PingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, m_PingpongTexture[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_WindowSizeX, m_WindowSizeY, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_PingpongTexture[i], 0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Renderer::AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	//쉐이더 오브젝트 생성
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
	}

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];

	size_t slen = strlen(pShaderText);
	if (slen > INT_MAX) {
		// Handle error
	}
	GLint len = (GLint)slen;

	Lengths[0] = len;
	//쉐이더 코드를 쉐이더 오브젝트에 할당
	glShaderSource(ShaderObj, 1, p, Lengths);

	//할당된 쉐이더 코드를 컴파일
	glCompileShader(ShaderObj);

	GLint success;
	// ShaderObj 가 성공적으로 컴파일 되었는지 확인
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];

		//OpenGL 의 shader log 데이터를 가져옴
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		printf("%s \n", pShaderText);
	}

	// ShaderProgram 에 attach!!
	glAttachShader(ShaderProgram, ShaderObj);
}

bool Renderer::ReadFile(char* filename, std::string* target)
{
	std::ifstream file(filename);
	if (file.fail())
	{
		std::cout << filename << " file loading failed.. \n";
		file.close();
		return false;
	}
	std::string line;
	while (getline(file, line)) {
		target->append(line.c_str());
		target->append("\n");
	}
	return true;
}

GLuint Renderer::CompileShaders(char* filenameVS, char* filenameFS)
{
	GLuint ShaderProgram = glCreateProgram(); //빈 쉐이더 프로그램 생성

	if (ShaderProgram == 0) { //쉐이더 프로그램이 만들어졌는지 확인
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, fs;

	// 버텍스 셰이더 로딩
	if (!ReadFile(filenameVS, &vs)) {
		printf("Error reading vertex shader: %s\n", filenameVS);
		return -1;
	};

	// 프래그먼트 셰이더 로딩
	if (!ReadFile(filenameFS, &fs)) {
		printf("Error reading fragment shader: %s\n", filenameFS);
		return -1;
	};

	// GridMesh.fs를 컴파일하는 경우에만 blender_nodes.glsl 파일을 앞에 붙여줍니다.
	/*std::string fs_final_source = fs;
	if (strcmp(filenameFS, "./Shaders/GridMesh.fs") == 0)
	{
		std::string node_utils_source;
		if (ReadFile((char*)"./Shaders/blender_nodes.glsl", &node_utils_source))
		{
			fs_final_source = node_utils_source + fs;
			printf("Successfully included blender_nodes.glsl for GridMesh.fs\n");
		}
		else
		{
			printf("Warning: Could not find ./Shaders/blender_nodes.glsl for GridMesh.fs\n");
		}
	}*/

	// 셰이더 컴파일 및 attach
	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	// 링킹 및 에러 확인 (이하 동일)
	glLinkProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error linking shader program\n" << ErrorLog;
		return -1;
	}

	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error validating shader program\n" << ErrorLog;
		return -1;
	}

	glUseProgram(ShaderProgram);
	std::cout << filenameVS << ", " << filenameFS << " Shader compiling is done.\n";

	return ShaderProgram;
}

void Renderer::DrawSolidRect(float x, float y, float z, float size, float r, float g, float b, float a)
{
	float newX, newY;

	GetGLPosition(x, y, &newX, &newY);

	//Program select
	glUseProgram(m_SolidRectShader);

	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Trans"), newX, newY, 0, size);
	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Color"), r, g, b, a);

	int attribPosition = glGetAttribLocation(m_SolidRectShader, "a_Position");
	glEnableVertexAttribArray(attribPosition);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(attribPosition);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawTest()
{
	glUseProgram(m_TestShader);

	int uTimeLoc = glGetUniformLocation(m_TestShader, "u_Time");
	glUniform1f(uTimeLoc, m_Time);

	int aPosLoc = glGetAttribLocation(m_TestShader, "a_Position");
	int aValueLoc = glGetAttribLocation(m_TestShader, "a_Value");
	int aColorLoc = glGetAttribLocation(m_TestShader, "a_Color");

	glEnableVertexAttribArray(aPosLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOTestPos);
	glVertexAttribPointer(aPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);

	glEnableVertexAttribArray(aValueLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOTestPos);
	glVertexAttribPointer(aValueLoc, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (GLvoid*)(sizeof(float) * 3));

	glEnableVertexAttribArray(aColorLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOTestColor);
	glVertexAttribPointer(aColorLoc, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);


	glDrawArrays(GL_TRIANGLES, 0, 12);

	glDisableVertexAttribArray(aPosLoc);
	glDisableVertexAttribArray(aValueLoc);
	glDisableVertexAttribArray(aColorLoc);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawBloomParticle()
{
	
}

void Renderer::DrawParticle()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GLuint shader = m_ParticleShader;
	glUseProgram(shader);

	int uTimeLoc = glGetUniformLocation(shader, "u_Time");
	glUniform1f(uTimeLoc, m_Time);

	int uForceLoc = glGetUniformLocation(shader, "u_Force");
	glUniform3f(uForceLoc, std::sin(m_Time), 0, 0);
	
	int uTexLoc = glGetUniformLocation(shader, "u_Texture");
	glUniform1i(uTexLoc, 0); // 0번 슬롯 사용
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_ParticleTexture);

	int aPosLoc = glGetAttribLocation(shader, "a_Position");
	int aValueLoc = glGetAttribLocation(shader, "a_Value");
	int aColLoc = glGetAttribLocation(shader, "a_Color");
	int asTimeLoc = glGetAttribLocation(shader, "a_sTime");
	int aVelLoc = glGetAttribLocation(shader, "a_Velocity");
	int aLifeTimeLoc = glGetAttribLocation(shader, "a_LifeTime");
	int aMassLoc = glGetAttribLocation(shader, "a_Mass");
	int aPeriodLoc = glGetAttribLocation(shader, "a_Period");

	int aUVLoc = glGetAttribLocation(shader, "a_UV");
	
	int stride = sizeof(float) * 17;

	glEnableVertexAttribArray(aPosLoc);
	glEnableVertexAttribArray(aValueLoc);
	glEnableVertexAttribArray(aColLoc);
	glEnableVertexAttribArray(asTimeLoc);
	glEnableVertexAttribArray(aVelLoc);
	glEnableVertexAttribArray(aLifeTimeLoc);
	glEnableVertexAttribArray(aMassLoc);
	glEnableVertexAttribArray(aPeriodLoc);
	glEnableVertexAttribArray(aUVLoc);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOParticle);

	glVertexAttribPointer(aPosLoc,
		3,
		GL_FLOAT, GL_FALSE,
		stride,
		0);

	glVertexAttribPointer(aValueLoc,
		1,
		GL_FLOAT, GL_FALSE,
		stride,
		(GLvoid*)(sizeof(float) * 3));

	glVertexAttribPointer(aColLoc,
		4,
		GL_FLOAT, GL_FALSE,
		stride,
		(GLvoid*)(sizeof(float) * 4));


	glVertexAttribPointer(asTimeLoc,
		1,
		GL_FLOAT, GL_FALSE,
		stride,
		(GLvoid*)(sizeof(float) * 8));


	glVertexAttribPointer(aVelLoc,
		3,
		GL_FLOAT, GL_FALSE,
		stride,
		(GLvoid*)(sizeof(float) * 9));


	glVertexAttribPointer(aLifeTimeLoc,
		1,
		GL_FLOAT, GL_FALSE,
		stride,
		(GLvoid*)(sizeof(float) * 12));

	glVertexAttribPointer(aMassLoc,
		1,
		GL_FLOAT, GL_FALSE,
		stride,
		(GLvoid*)(sizeof(float) * 13));

	glVertexAttribPointer(aPeriodLoc,
		1,
		GL_FLOAT, GL_FALSE,
		stride,
		(GLvoid*)(sizeof(float) * 14));
	
	glVertexAttribPointer(aUVLoc,
		2,
		GL_FLOAT, GL_FALSE,
		stride,
		(GLvoid*)(sizeof(float) * 15));
	glDrawArrays(GL_TRIANGLES, 0, m_VBOParticleVertexCount);

	glDisableVertexAttribArray(aPosLoc);
	glDisableVertexAttribArray(aValueLoc);
	glDisableVertexAttribArray(aColLoc);

	glDisableVertexAttribArray(asTimeLoc);
	glDisableVertexAttribArray(aVelLoc);
	glDisableVertexAttribArray(aLifeTimeLoc);
	glDisableVertexAttribArray(aMassLoc);
	glDisableVertexAttribArray(aPeriodLoc);
	

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_BLEND);
}

void Renderer::DrawFullScreenColor(float r, float g, float b, float a)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLuint shader = m_FullScreenShader;
	//Program select
	glUseProgram(shader);

	glUniform4f(glGetUniformLocation(shader, "u_Color"), r, g, b, a);

	int attribPosition = glGetAttribLocation(shader, "a_Position");
	glEnableVertexAttribArray(attribPosition);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOFullScreen);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE,
		sizeof(float) * 3, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(attribPosition);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_BLEND);
}

void Renderer::DrawGridMesh()
{

	GLuint shader = m_GridMeshVertexShader;
	glUseProgram(shader);

	// --- Uniform 변수 전달 ---
	int uTimeLoc = glGetUniformLocation(shader, "u_Time");
	glUniform1f(uTimeLoc, m_Time);

	int uPointsLoc = glGetUniformLocation(shader, "u_Points");
	glUniform4fv(uPointsLoc, 100, m_Points);


	// --- Vertex Attribute 설정 ---
	glBindBuffer(GL_ARRAY_BUFFER, m_GridMeshVBO);

	int stride = sizeof(float) * 8; // 3(Pos) + 3(Normal) + 2(UV) = 8

	// a_Position (location = 0)
	GLuint aPos = glGetAttribLocation(shader, "a_Position");
	glEnableVertexAttribArray(aPos);
	glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, stride, 0);



	glDrawArrays(GL_TRIANGLES, 0, m_GridMeshVertexCount);

	glDisableVertexAttribArray(aPos);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawFS()
{

	GLuint shader = m_FSShader;
	glUseProgram(shader);

	GLenum DrawBuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, DrawBuffers);

	int uTimeLoc = glGetUniformLocation(shader, "u_Time");
	glUniform1f(uTimeLoc, m_Time);

	int uTextureLoc = glGetUniformLocation(shader, "u_RGBTexture");
	glUniform1i(uTextureLoc, 11);

	int uDigitTextureLoc = glGetUniformLocation(shader, "u_DigitTexture");
	glUniform1i(uDigitTextureLoc, (int)floor(m_Time) % 10);


	glBindTexture(GL_TEXTURE_2D, m_IVETexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_0Texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_1Texture);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_2Texture);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_3Texture);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_4Texture);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, m_5Texture);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, m_6Texture);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, m_7Texture);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, m_8Texture);
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, m_9Texture);
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, m_NUMTexture);

	glActiveTexture(GL_TEXTURE15);
	glBindTexture(GL_TEXTURE_2D, m_RGBTexture);
	glActiveTexture(GL_TEXTURE16);

	GLint loc;

	GLuint digitTex[10] = {
		m_0Texture, m_1Texture, m_2Texture, m_3Texture, m_4Texture,
		m_5Texture, m_6Texture, m_7Texture, m_8Texture, m_9Texture
	};

	for (int i = 0; i < 10; ++i) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, digitTex[i]);

		char uniName[16];
		snprintf(uniName, sizeof(uniName), "u_%dTexture", i);
		loc = glGetUniformLocation(shader, uniName);
		if (loc != -1) glUniform1i(loc, i);
	}

	int digit = ((int)m_Time) % 10;
	if (digit < 0) digit += 10;
	loc = glGetUniformLocation(shader, "uDigitTexture");
	if (loc != -1) glUniform1i(loc, digit);

	// RGB, IVE, NUM -> 텍스처 유닛 10, 11, 12
	glActiveTexture(GL_TEXTURE0 + 10);
	glBindTexture(GL_TEXTURE_2D, m_RGBTexture);
	loc = glGetUniformLocation(shader, "u_RGBTexture");
	if (loc != -1) glUniform1i(loc, 10);

	glActiveTexture(GL_TEXTURE0 + 11);
	glBindTexture(GL_TEXTURE_2D, m_IVETexture);
	loc = glGetUniformLocation(shader, "u_IVETexture");
	if (loc != -1) glUniform1i(loc, 11);

	glActiveTexture(GL_TEXTURE0 + 12);
	glBindTexture(GL_TEXTURE_2D, m_NUMTexture);
	loc = glGetUniformLocation(shader, "u_NUMTexture");
	if (loc != -1) glUniform1i(loc, 12);

	// 정점 어트리뷰트 설정 (기존과 동일)
	int stride = sizeof(float) * 3;

	GLuint attribPosition = glGetAttribLocation(shader, "a_Position");
	glEnableVertexAttribArray(attribPosition);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOFS);
	glVertexAttribPointer(attribPosition, 3,
		GL_FLOAT, GL_FALSE,
		stride, 0);


	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(attribPosition);

	// 바인딩 정리 (선택 사항) — 0..12 유닛 언바인드
	for (int i = 0; i <= 12; ++i) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glActiveTexture(GL_TEXTURE0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Renderer::DrawTexture(float x, float y, float sx, float sy, GLuint TexID, GLuint TexID1, GLuint method)
{
	//sx, sy --???? ratio
	GLuint shader = m_TexShader;
	//Program select
	glUseProgram(shader);

	int uTex = glGetUniformLocation(shader, "u_TexID");
	glUniform1i(uTex, 0);
	int uSize = glGetUniformLocation(shader, "u_Size");
	glUniform2f(uSize, sx, sy);
	int uTran = glGetUniformLocation(shader, "u_Tran");
	glUniform2f(uTran, x, y);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TexID);

	int aPos = glGetAttribLocation(shader, "a_Pos");
	int aTex = glGetAttribLocation(shader, "a_Tex");
	glEnableVertexAttribArray(aPos);
	glEnableVertexAttribArray(aTex);

	glBindBuffer(GL_ARRAY_BUFFER, m_TexVBO);
	glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	glVertexAttribPointer(aTex, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(aPos);
	glDisableVertexAttribArray(aTex);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawDebugTexture()
{
	// [왼쪽] 일반 영상 (m_HDRLowTexture)
	// 위치(-0.5, 0), 크기(0.5, 1.0) -> 왼쪽 절반 채움
	DrawTexture(-0.5f, 0.0f, 0.5f, 0.5f, m_HDRLowTexture,0,0);
	
	DrawTexture(0.5f, 0.0f, 0.5f, 0.5f, m_PingpongTexture[0],0,0);
}
void Renderer::DrawFBOs()
{
	// 1. Scene 렌더링 (파티클 그리기)
	glBindFramebuffer(GL_FRAMEBUFFER, m_HDRFBO);
	GLenum drawbuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, drawbuffers);
    
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, m_WindowSizeX, m_WindowSizeY);

	DrawParticle(); 

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 2. Blur 처리 (Ping-Pong)
	bool horizontal = true;
	int amount = 10; 

	// 첫 번째 블러: High Texture -> Pingpong[0]
	DrawGaussianBlur(m_HDRHighTexture, m_PingpongFBO[0], m_GaussianBlurHShader);

	// 이후 반복 블러
	for (int i = 1; i < amount; i++)
	{
		horizontal = !horizontal; 
		int destIndex = horizontal ? 0 : 1;
		int srcIndex = horizontal ? 1 : 0;
		GLuint shader = horizontal ? m_GaussianBlurHShader : m_GaussianBlurVShader;

		DrawGaussianBlur(m_PingpongTexture[srcIndex], m_PingpongFBO[destIndex], shader);
	}
    
	// ---------------------------------------------------------
	// [추가된 부분] 3. 최종 합성 (Merge) 및 화면 출력
	// ---------------------------------------------------------
    
	// 기본 프레임버퍼(화면) 바인딩
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, m_WindowSizeX, m_WindowSizeY);
    
	// 화면을 깨끗이 지움
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// m_HDRLowTexture: 원본 파티클
	// m_PingpongTexture[0]: 최종 블러된 빛 (반복문이 짝수번 돌면 0번에 저장됨)
	// Exposure: 2.0f (빛의 세기 조절)
	DrawMergeBloomTexture(m_HDRLowTexture, m_PingpongTexture[0], 2.0f);
}

void Renderer::AddTime()
{
	m_Time += 0.016f;
}

void Renderer::GetGLPosition(float x, float y, float* newX, float* newY)
{
	// 윈도우 사이즈를 float으로 캐스팅하여 안전하게 계산
	*newX = x * 2.0f / static_cast<float>(m_WindowSizeX);
	*newY = y * 2.0f / static_cast<float>(m_WindowSizeY);
}

void Renderer::DrawGaussianBlur(GLuint texID, GLuint targetFBOID, GLuint shader) {
    glBindFramebuffer(GL_FRAMEBUFFER, targetFBOID);
    glUseProgram(shader);
    
    // Attribute 설정 (a_Position, a_TexPos)
    // [주의] m_TexVBO 사용 (Position 3 + UV 2)
    int stride = sizeof(float) * 5; 
    glBindBuffer(GL_ARRAY_BUFFER, m_TexVBO);
    
    int posLoc = glGetAttribLocation(shader, "a_Position");
    glEnableVertexAttribArray(posLoc);
    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, stride, 0);
    
    int texLoc = glGetAttribLocation(shader, "a_TexPos");
    glEnableVertexAttribArray(texLoc);
    glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)*3));

    glUniform1i(glGetUniformLocation(shader, "u_Texture"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    glDisableVertexAttribArray(posLoc);
    glDisableVertexAttribArray(texLoc);
}

void Renderer::DrawMergeBloomTexture(GLuint sceneTexID, GLuint bloomTexID, float exposure) {
    glUseProgram(m_DrawMergeTextureShader);
    // Attribute 설정은 위와 동일 (생략)
    int stride = sizeof(float) * 5;
    glBindBuffer(GL_ARRAY_BUFFER, m_TexVBO);
    
    int posLoc = glGetAttribLocation(m_DrawMergeTextureShader, "a_Position");
    glEnableVertexAttribArray(posLoc);
    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, stride, 0);
    
    int texLoc = glGetAttribLocation(m_DrawMergeTextureShader, "a_TexPos");
    glEnableVertexAttribArray(texLoc);
    glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)*3));

    glUniform1i(glGetUniformLocation(m_DrawMergeTextureShader, "u_TextureScene"), 0);
    glUniform1i(glGetUniformLocation(m_DrawMergeTextureShader, "u_TextureBloom"), 1);
    glUniform1f(glGetUniformLocation(m_DrawMergeTextureShader, "u_Exposure"), exposure);

    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, sceneTexID);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, bloomTexID);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(posLoc);
    glDisableVertexAttribArray(texLoc);
}