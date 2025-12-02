#pragma once

#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <cassert>

#include "Dependencies\glew.h"
#include "LoadPng.h"

class Renderer
{
public:
	Renderer(int windowSizeX, int windowSizeY);
	~Renderer();

	bool IsInitialized();
	void ReloadAllShaderPrograms();
	void DrawSolidRect(float x, float y, float z, float size, float r, float g, float b, float a);
	void DrawTest();
	void DrawBloomParticle();
	void DrawParticle();
	void DrawGridMesh();
	void AddTime();
	void DrawFullScreenColor(float r, float g, float b, float a);
	void DrawFS();
	void DrawTexture(float x, float y, float sx, float sy, GLuint TexID, GLuint TexID1, GLuint method);
	void DrawDebugTexture();
	void DrawFBOs();

private:
	void Initialize(int windowSizeX, int windowSizeY);
	void CompileAllShaderPrograms();
	void DeleteAllShaderPrograms();

	GLuint CreatePngTexture(char* filePath, GLuint samplingMethod);

	bool ReadFile(char* filename, std::string* target);
	void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
	GLuint CompileShaders(char* filenameVS, char* filenameFS);
	void CreateVertexBufferObjects();
	void CreateGridMesh(int x, int y);
	void GetGLPosition(float x, float y, float* newX, float* newY);

	void GenerateParticles(int numParticle);
	void CreateFBOs();

	bool m_Initialized = false;

	unsigned int m_WindowSizeX = 0;
	unsigned int m_WindowSizeY = 0;

	GLuint m_VBORect = 0;
	GLuint m_SolidRectShader = 0;

	GLuint m_VBOTestPos = 0;
	GLuint m_VBOTestColor = 0;
	GLuint m_TestShader = 0;

	float  m_Time = 0;

	GLuint m_ParticleShader = 0;
	GLuint m_VBOParticle = 0;
	GLuint m_VBOParticleVertexCount = 0;

	GLuint m_GridMeshVertexShader = 0;
	GLuint m_GridMeshVertexCount = 0;
	GLuint m_GridMeshVBO = 0;
	
	GLuint m_VBOFullScreen = 0;
	GLuint m_FullScreenShader = 0;

	static const int RAMP_SIZE = 4; // 컬러 램프의 색상 개수
	float m_RampColors[RAMP_SIZE][3];
	float m_RampPositions[RAMP_SIZE];

	float m_Points[100 * 4];

	GLuint m_VBOFS = 0;
	GLuint m_FSShader = 0;

	GLuint m_RGBTexture = 0;
	GLuint m_IVETexture = 0;
	GLuint m_NUMTexture = 0;
	GLuint m_ParticleTexture = 0;
	
	GLuint m_0Texture = 0;
	GLuint m_1Texture = 0;
	GLuint m_2Texture = 0;
	GLuint m_3Texture = 0;
	GLuint m_4Texture = 0;
	GLuint m_5Texture = 0;
	GLuint m_6Texture = 0;
	GLuint m_7Texture = 0;
	GLuint m_8Texture = 0;
	GLuint m_9Texture = 0;

	GLuint m_TexVBO = 0;
	GLuint m_TexShader = 0;

	GLuint m_RT0_0 = 0;
	GLuint m_RT0_1 = 0;
	GLuint m_RT1_0 = 0;
	GLuint m_RT1_1 = 0;
	GLuint m_RT2_0 = 0;
	GLuint m_RT2_1 = 0;
	GLuint m_RT3_0 = 0;
	GLuint m_RT4_0 = 0;

	GLuint m_FBO0 = 0;
	GLuint m_FBO1 = 0;
	GLuint m_FBO2 = 0;
	GLuint m_FBO3 = 0;
	GLuint m_FBO4 = 0;
	
	
	GLuint m_HDRFBO = 0;
	GLuint m_HDRLowTexture = 0;  // 일반 화면 저장 [cite: 19]
	GLuint m_HDRHighTexture = 0; // 밝은 빛 저장 [cite: 20]

	// [Ping-Pong Blur] 블러를 번갈아가며 하기 위한 FBO 2개 [cite: 72]
	GLuint m_PingpongFBO[2] = {0, 0};
	GLuint m_PingpongTexture[2] = {0, 0};

	// 쉐이더 ID
	GLuint m_GaussianBlurHShader = 0;
	GLuint m_GaussianBlurVShader = 0;
	GLuint m_DrawMergeTextureShader = 0;
	
	void DrawGaussianBlur(GLuint texID, GLuint targetFBOID, GLuint shader);
	void DrawMergeBloomTexture(GLuint sceneTexID, GLuint bloomTexID, float exposure);
};



