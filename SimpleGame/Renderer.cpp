#include "stdafx.h"
#include "Renderer.h"
#include "Dependencies\freeglut.h"

Renderer::Renderer(int windowSizeX, int windowSizeY)
{
	Initialize(windowSizeX, windowSizeY);
}

Renderer::~Renderer()
{
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
}

void Renderer::DeleteAllShaderPrograms()
{
	glDeleteShader(m_SolidRectShader);
	glDeleteShader(m_TestShader);
	glDeleteShader(m_ParticleShader);
	glDeleteShader(m_GridMeshVertexShader);
	glDeleteShader(m_FullScreenShader);
	glDeleteShader(m_FSShader);
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
	int floatCountPervertex = 3 + 1 + 4 + 1 + 3 + 1 + 1 + 1; // x, y, z, value, r, g, b, a, sTime, Velocity, Lifetime, mass
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

		float r, g, b, a;
		r = (float)rand() / (float)RAND_MAX;
		g = (float)rand() / (float)RAND_MAX;
		b = (float)rand() / (float)RAND_MAX;
		a = (float)rand() / (float)RAND_MAX;

		float size;
		size = ((float)rand() / (float)RAND_MAX) * 0.01f;

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

		vertices[index++] = x - size;		// v1
		vertices[index++] = y - size;
		vertices[index++] = z;
		vertices[index++] = value;
		vertices[index++] = r;
		vertices[index++] = g;
		vertices[index++] = b;
		vertices[index++] = a;
		vertices[index++] = sTime;
		vertices[index++] = vx;
		vertices[index++] = vy;
		vertices[index++] = vz;
		vertices[index++] = lifeTime;
		vertices[index++] = mass;
		vertices[index++] = period;

		vertices[index++] = x + size;		// v2
		vertices[index++] = y - size;
		vertices[index++] = z;
		vertices[index++] = value;
		vertices[index++] = r;
		vertices[index++] = g;
		vertices[index++] = b;
		vertices[index++] = a;
		vertices[index++] = sTime;
		vertices[index++] = vx;
		vertices[index++] = vy;
		vertices[index++] = vz;
		vertices[index++] = lifeTime;
		vertices[index++] = mass;
		vertices[index++] = period;

		vertices[index++] = x + size;		// v3
		vertices[index++] = y + size;
		vertices[index++] = z;
		vertices[index++] = value;
		vertices[index++] = r;
		vertices[index++] = g;
		vertices[index++] = b;
		vertices[index++] = a;
		vertices[index++] = sTime;
		vertices[index++] = vx;
		vertices[index++] = vy;
		vertices[index++] = vz;
		vertices[index++] = lifeTime;
		vertices[index++] = mass;
		vertices[index++] = period;

		vertices[index++] = x - size;		// v4
		vertices[index++] = y + size;
		vertices[index++] = z;
		vertices[index++] = value;
		vertices[index++] = r;
		vertices[index++] = g;
		vertices[index++] = b;
		vertices[index++] = a;
		vertices[index++] = sTime;
		vertices[index++] = vx;
		vertices[index++] = vy;
		vertices[index++] = vz;
		vertices[index++] = lifeTime;
		vertices[index++] = mass;
		vertices[index++] = period;

		vertices[index++] = x - size;		// v5
		vertices[index++] = y - size;
		vertices[index++] = z;
		vertices[index++] = value;
		vertices[index++] = r;
		vertices[index++] = g;
		vertices[index++] = b;
		vertices[index++] = a;
		vertices[index++] = sTime;
		vertices[index++] = vx;
		vertices[index++] = vy;
		vertices[index++] = vz;
		vertices[index++] = lifeTime;
		vertices[index++] = mass;
		vertices[index++] = period;

		vertices[index++] = x + size;		// v6
		vertices[index++] = y + size;
		vertices[index++] = z;
		vertices[index++] = value;
		vertices[index++] = r;
		vertices[index++] = g;
		vertices[index++] = b;
		vertices[index++] = a;
		vertices[index++] = sTime;
		vertices[index++] = vx;
		vertices[index++] = vy;
		vertices[index++] = vz;
		vertices[index++] = lifeTime;
		vertices[index++] = mass;
		vertices[index++] = period;

	}

	glGenBuffers(1, &m_VBOParticle);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOParticle);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * totalFloatCount, vertices, GL_STATIC_DRAW);

	delete[] vertices;

	m_VBOParticleVertexCount = totalVerticesCount;
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
	std::string fs_final_source = fs;
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
	}

	// 셰이더 컴파일 및 attach
	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);
	AddShader(ShaderProgram, fs_final_source.c_str(), GL_FRAGMENT_SHADER);

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

	int aPosLoc = glGetAttribLocation(shader, "a_Position");
	int aValueLoc = glGetAttribLocation(shader, "a_Value");
	int aColLoc = glGetAttribLocation(shader, "a_Color");
	int asTimeLoc = glGetAttribLocation(shader, "a_sTime");
	int aVelLoc = glGetAttribLocation(shader, "a_Velocity");
	int aLifeTimeLoc = glGetAttribLocation(shader, "a_LifeTime");
	int aMassLoc = glGetAttribLocation(shader, "a_Mass");
	int aPeriodLoc = glGetAttribLocation(shader, "a_Period");

	int stride = sizeof(float) * 15;

	glEnableVertexAttribArray(aPosLoc);
	glEnableVertexAttribArray(aValueLoc);
	glEnableVertexAttribArray(aColLoc);
	glEnableVertexAttribArray(asTimeLoc);
	glEnableVertexAttribArray(aVelLoc);
	glEnableVertexAttribArray(aLifeTimeLoc);
	glEnableVertexAttribArray(aMassLoc);
	glEnableVertexAttribArray(aPeriodLoc);

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

	int uResolutionLoc = glGetUniformLocation(shader, "u_resolution");
	glUniform2f(uResolutionLoc, (float)m_WindowSizeX, (float)m_WindowSizeY);

	// 카메라 위치 전달 (Camera, Reflection 구현용)
	int uCameraPosLoc = glGetUniformLocation(shader, "u_camera_position");
	glUniform3f(uCameraPosLoc, 0.0f, 0.0f, 2.0f); // 예시 카메라 위치

	// --- Vertex Attribute 설정 ---
	glBindBuffer(GL_ARRAY_BUFFER, m_GridMeshVBO);

	int stride = sizeof(float) * 8; // 정점 하나당 8개의 float (pos, norm, uv)

	// a_Position (location = 0)
	GLuint attribPosition = glGetAttribLocation(shader, "a_Position");
	glEnableVertexAttribArray(attribPosition);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, stride, 0);

	// a_Normal (location = 1)
	GLuint attribNormal = glGetAttribLocation(shader, "a_Normal");
	glEnableVertexAttribArray(attribNormal);
	glVertexAttribPointer(attribNormal, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(sizeof(float) * 3));

	// a_UV (location = 2)
	GLuint attribUV = glGetAttribLocation(shader, "a_UV");
	glEnableVertexAttribArray(attribUV);
	glVertexAttribPointer(attribUV, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(sizeof(float) * 6));


	int rampColorsLoc = glGetUniformLocation(shader, "u_ramp_colors");
	glUniform3fv(rampColorsLoc, RAMP_SIZE, &m_RampColors[0][0]);

	int rampPositionsLoc = glGetUniformLocation(shader, "u_ramp_positions");
	glUniform1fv(rampPositionsLoc, RAMP_SIZE, m_RampPositions);


	glDrawArrays(GL_TRIANGLES, 0, m_GridMeshVertexCount);

	glDisableVertexAttribArray(attribPosition);
	glDisableVertexAttribArray(attribNormal);
	glDisableVertexAttribArray(attribUV);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawFS()
{

	GLuint shader = m_FSShader;
	glUseProgram(shader);

	float timeValue = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	int timeLoc = glGetUniformLocation(shader, "iTime");
	if (timeLoc != -1) {
		glUniform1f(timeLoc, timeValue);
	}

	// 3. iResolution (해상도) 값 전달
	// "m_WindowWidth"와 "m_WindowHeight"는 예시입니다.
	// TODO: 'Renderer.h' 파일에서 실제 창 크기 멤버 변수명을 확인하고 대체하세요.
	int resLoc = glGetUniformLocation(shader, "iResolution");
	if (resLoc != -1) {
		// 예: glUniform2f(resLoc, (float)m_width, (float)m_height);
		glUniform2f(resLoc, (float)m_WindowSizeX, (float)m_WindowSizeY);
	}	

	int stride = sizeof(float) * 3;

	GLuint attribPosition = glGetAttribLocation(shader, "a_Position");
	glEnableVertexAttribArray(attribPosition);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOFS);
	glVertexAttribPointer(attribPosition, 3, 
		GL_FLOAT, GL_FALSE, 
		stride, 0);


	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(attribPosition);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::AddTime()
{
	m_Time += 0.016f;
}

void Renderer::GetGLPosition(float x, float y, float* newX, float* newY)
{
	*newX = x * 2.f / m_WindowSizeX;
	*newY = y * 2.f / m_WindowSizeY;
}

