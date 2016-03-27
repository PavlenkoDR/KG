
/*
 * VBO - буфер вершин, используется для хранения вершин
 */
/*
 * Конвейер OpenGL:
 * Вершинный процессор. Сюда попадают вершины, трансформируются и идут дальше
 * ->
 * Геометрический процессор. Тут делаются фигурки из точек
 * ->
 * Клиппер. Обрезает вершины выходящие за наш кругозор
 * ->
 * Растеризатор(Фрагментный процессор).
 * Для каждой из вершин растеризатор вызывает фрагментный процессор
 * В нём вы имеете возможность определить цвет пикселя выбрав его из текстуры
 * или используя любой метод, какой вам нравится
 *
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "pipeline.h"
#include "camera.h"
#include "texture.h"
#include "geometry.h"

#define WINDOW_WIDTH  1920
#define WINDOW_HEIGHT 1080

using namespace std;

struct DirectionLight
{
    Vector3f Color;
    float AmbientIntensity;
    Vector3f Direction;
    float DiffuseIntensity;
};

DirectionLight m_directionalLight;

GLuint IBO;
GLuint gWVPLocation;
GLuint gWorldLocation;
GLuint gCamera;
GLuint Proj;

GLuint Color;
GLuint ShaderFlag;
GLuint ShaderFlagSquare;
int ShaderFlagSquarei;

//GLuint VAO_LINES;
geometry grid;
//GLuint VAO_CUBE;
geometry cube;
//GLuint VAO_CubeLines;
geometry cubeLines;
//GLuint VAO_Sphere;
geometry sphere;
geometry squareMass;
geometry square;
int squareCount = 8;

GLuint gSampler;
GLuint m_dirLightColorLocation;
GLuint m_dirLightAmbientIntensityLocation;
GLuint m_dirLightDirectionLocation;
GLuint m_dirLightDiffuseIntensityLocation;
GLuint LightPosition;

GLuint StartPos;
GLuint WidthHeight;
GLuint HowCell;
GLuint Color0;
GLuint Color1;

Vector3f LightPos(0.0f, 2.0f, 0.0f);

texture* pTexture[12] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

const float pi = 3.1415926535897932384626433832795f;

/*
 * Мы назначим GLuint в качестве глобальной переменной для хранения указателя на буфер вершин
 */

Camera* pGameCamera = NULL;

static const char* pVS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
                                                                                    \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
layout (location = 1) in vec2 TexCoord;                                             \n\
layout (location = 2) in vec3 Normal;                                               \n\
                                                                                    \n\
uniform sampler2D gSampler;                                                         \n\
                                                                                    \n\
uniform mat4 gWVP;                                                                  \n\
uniform mat4 gWorld;                                                                \n\
uniform mat4 gCamera;                                                               \n\
uniform mat4 Proj;                                                                  \n\
uniform vec3 Color;                                                                 \n\
uniform vec3 LightPosition;															\n\
                                                                                    \n\
out vec4 ColorOut;                                                                  \n\
out vec2 TexCoord0;                                                                 \n\
out vec3 Normal0;                                                                   \n\
out vec3 Position0;                                                                 \n\
out vec3 Position1;                                                                 \n\
out vec3 Normal_cameraspace;                                                        \n\
out vec3 LightPosition0;															\n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    LightPosition0 = (vec4(LightPosition, 1.0)).xyz;                                \n\
    gl_Position = gWVP * vec4(Position, 1.0);                                       \n\
    Position0 = (gWorld*vec4(Position, 1.0)).xyz;                                   \n\
    Position1 = Position;                                                           \n\
    Normal0 = (gWorld * vec4(Normal, 0.0)).xyz;                                   	\n\
    ColorOut = vec4(Color, 1.0);                                       				\n\
    TexCoord0 = TexCoord;                                                           \n\
}";

/*
 * gl_Position 	- это специально построенная переменная, содержащая вершинные координаты (X, Y, Z и W компоненты)
 * gWVP 		- матрица для трансформирования вектора, то есть трансформации вершины
 * 				  World-View-Projection
 */

static const char* pFS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
                                                                                    \n\
                                                                                    \n\
in vec4 ColorOut;                                                                   \n\
in vec2 TexCoord0;                                                                  \n\
in vec3 Normal0;                                                                    \n\
in vec3 Position0;                                                                  \n\
in vec3 Position1;                                                                  \n\
in vec3 Normal_cameraspace;                                                         \n\
in vec3 LightPosition0;																\n\
                                                                                    \n\
                                                                                    \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
uniform sampler2D gSampler;                                                         \n\
uniform int flag;                                                         			\n\
uniform vec3 LightPosition;															\n\
                                                                                    \n\
uniform vec3 StartPos;                                                              \n\
uniform float WidthHeight;                                                          \n\
uniform int HowCell;                                                                \n\
uniform vec3 Color0;                                                              	\n\
uniform vec3 Color1;                                                              	\n\
uniform int flagSquare;                                                         	\n\
                                                                                    \n\
vec3 Procedural()                                                                   \n\
{                                                                                   \n\
    int w, h, k = 0;                                                                \n\
    for (float i = StartPos.x - WidthHeight; i <= WidthHeight + StartPos.x; i+=2*WidthHeight/(float(HowCell)))           \n\
    {                                                                               \n\
        k++;                                                                        \n\
        if ((Position1.x >= i)&&(Position1.x< i + 2*WidthHeight/(float(HowCell))))  \n\
        {                                                                           \n\
            w = int(k)%2;                                                           \n\
            break;                                                                  \n\
        }                                                                           \n\
    }                                                                               \n\
    k = 0;                                                                          \n\
    for (float i = StartPos.z- WidthHeight; i <= WidthHeight + StartPos.z; i+=2*WidthHeight/(float(HowCell)))           	\n\
    {                                                                               \n\
        k++;                                                                        \n\
        if ((Position1.z >= i)&&(Position1.z < i + 2*WidthHeight/(float(HowCell)))) \n\
        {                                                                           \n\
            h = int(k)%2;                                                           \n\
            break;                                                                  \n\
        }                                                                           \n\
    }                                                                               \n\
    if ((w + h ) % 2 == 0)                                                          \n\
    {                                                                               \n\
        if (flagSquare == 0)                                                        \n\
            return Color0;                                                          \n\
        else                                                                        \n\
            return Color1;                                                          \n\
    }                                                                               \n\
    else                                                                            \n\
    {                                                                               \n\
        if (flagSquare == 0)                                                        \n\
            return Color1;                                                          \n\
        else                                                                        \n\
            return Color0;                                                          \n\
    }                                                                               \n\
}                                                                                   \n\
                                                                                    \n\
    vec3 tex_gen(vec2 coord) {													 	\n\
        int w = 0, h = 0;                                                           \n\
        for (float i = 0; i <=1.0; i+=1/float(HowCell))                             \n\
        {                                                                           \n\
            w++;                                                                    \n\
            if ((coord.x >= i)&&(coord.x < i + 1/float(HowCell)))                   \n\
            {                                                                       \n\
                break;                                                              \n\
            }                                                                       \n\
        }                                                                           \n\
                                                                                    \n\
        for (float i = 0; i <=1.0; i+=1/float(HowCell))                             \n\
        {                                                                           \n\
            h++;                                                                    \n\
            if ((coord.y >= i)&&(coord.y < i + 1/float(HowCell)))                   \n\
            {                                                                       \n\
                break;                                                              \n\
            }                                                                       \n\
        }                                                                           \n\
                                                                                    \n\
        if ((w + h ) % 2 == 0)                                                      \n\
        {                                                                           \n\
            if (flagSquare == 0)                                                    \n\
                return Color0;                                                      \n\
            else                                                                    \n\
                return Color1;                                                      \n\
        }                                                                           \n\
        else                                                                        \n\
        {                                                                           \n\
            if (flagSquare == 0)                                                    \n\
                return Color1;                                                      \n\
            else                                                                    \n\
                return Color0;                                                      \n\
        }                                                                           \n\
    }																				\n\
                                                                                    \n\
struct DirectionalLight                                                             \n\
{                                                                                   \n\
    vec3 Color;                                                                     \n\
    float AmbientIntensity;                                                         \n\
    float DiffuseIntensity;                                                         \n\
    vec3 Direction;                                                                 \n\
};                                                                                  \n\
                                                                                    \n\
uniform DirectionalLight gDirectionalLight;                                         \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    vec3 n = normalize( Normal0 );                                       			\n\
    vec3 l = normalize( LightPosition0 );                              				\n\
    float cosTheta = clamp( dot( n,l ), 0.1, 1.0);                                  \n\
                                                                                    \n\
    float distance = length( LightPosition0 - Position0);          					\n\
    vec4 AmbientColor = vec4(gDirectionalLight.Color, 1.0f) *                       \n\
                        gDirectionalLight.AmbientIntensity;                         \n\
                                                                                    \n\
    vec3 LightColor = gDirectionalLight.Color;                                      \n\
    float LightPower = gDirectionalLight.DiffuseIntensity;                          \n\
    switch(flag)                                                                    \n\
    {                                                                               \n\
    case 0:                                                                         \n\
        FragColor = ColorOut * AmbientColor; 										\n\
        break;                                                                      \n\
    case 1:                                                                         \n\
        FragColor.rgb =  															\n\
		texture2D(gSampler, TexCoord0.xy).rgb * (AmbientColor.xyz +					\n\
		LightColor * LightPower * cosTheta / (distance*distance)).rgb;				\n\
        break;                                                                      \n\
    case 2:                                                                         \n\
        FragColor.rgb = Procedural() * (AmbientColor.xyz +							\n\
		LightColor * LightPower * cosTheta / (distance*distance)).rgb;				\n\
        break;                                                                      \n\
    case 3:                                                                         \n\
        FragColor.rgb = tex_gen(TexCoord0) * (AmbientColor.xyz +					\n\
		LightColor * LightPower * cosTheta / (distance*distance)).rgb;				\n\
    }                                                                               \n\
}";

/*
 * FragColor идет на out к растеризатору
 * dot - скалярное произведение
 */

void CalcNormals(const unsigned int* pIndices, unsigned int IndexCount,
		Vector3f* pVertices, Vector3f* pNormal, unsigned int VertexCount)
	{
		if (IndexCount)
			for (unsigned int i = 0; i < IndexCount; i +=3 ){
				unsigned int Index0 = pIndices[i];
				unsigned int Index1 = pIndices[i + 1];
				unsigned int Index2 = pIndices[i + 2];
				Vector3f v1 = pVertices[Index1] - pVertices[Index0];
				Vector3f v2 = pVertices[Index2] - pVertices[Index0];
				Vector3f Normal = v1.Cross(v2);
				Normal.Normalize();
				pNormal[Index0] = Normal;
				pNormal[Index1] = Normal;
				pNormal[Index2] = Normal;
			}
		else
	        for (unsigned int i = 0; i < VertexCount; i +=3 ){
	            unsigned int Index0 = i;
	            unsigned int Index1 = (i + 1 < VertexCount)?(i + 1):1;
	            unsigned int Index2 = (i + 2 < VertexCount)?(i + 2):2;
	            Vector3f v1 = pVertices[Index1] - pVertices[Index0];
	            Vector3f v2 = pVertices[Index2] - pVertices[Index0];
	            Vector3f Normal = v1.Cross(v2);
	            Normal.Normalize();
	            pNormal[Index0] = Normal;
	            pNormal[Index1] = Normal;
	            pNormal[Index2] = Normal;
	        }
        for (unsigned int i = 0; i < VertexCount; i++){
        	pNormal[i].Normalize();
        }
    }

void InitGrid(unsigned int count, float step){
	float c = count*step - step;
	Vector3f buff[count * 4];
	for (int i = 0; i < count; ++i)
	{
		buff[i*4].x = -c/2 + i*step;
		buff[i*4].y = 0.0f;
		buff[i*4].z = -c/2;

		buff[i*4+1].x = -c/2 + i*step;
		buff[i*4+1].y = 0.0f;
		buff[i*4+1].z = c/2;

		buff[i*4+2].x = -c/2;
		buff[i*4+2].y = 0.0f;
		buff[i*4+2].z = -c/2 + i*step;

		buff[i*4+3].x = c/2;
		buff[i*4+3].y = 0.0f;
		buff[i*4+3].z = -c/2 + i*step;
	}
	Vector3f Normals[count * 4];
    CalcNormals(NULL, 0, buff, Normals, count * 4);
	grid.GenBuffers(NULL, 0, buff, NULL, Normals, count * 4);
	grid.PutData(step, count);
}

void InitCube()
{
	Vector3f Vertices[4*6];
	//Back side
    Vertices[0] = Vector3f(-1.0f, -1.0f, -1.0f);	// 	-1 	-1 	0
    Vertices[1] = Vector3f(1.0f, -1.0f, -1.0f);		// 	 1	-1	1
    Vertices[2] = Vector3f(1.0f, 1.0f, -1.0f);		//	 1	 1	2
    Vertices[3] = Vector3f(-1.0f, 1.0f, -1.0f);		//	-1	 1	3
    //Front side
    Vertices[4] = Vector3f(-1.0f, -1.0f, 1.0f);		// 	-1 	-1	4
    Vertices[5] = Vector3f(1.0f, -1.0f, 1.0f);		// 	 1	-1	5
    Vertices[6] = Vector3f(1.0f, 1.0f, 1.0f);		//	 1	 1	6
    Vertices[7] = Vector3f(-1.0f, 1.0f, 1.0f);		//	-1	 1	7
    //Left side
    Vertices[8] = Vector3f(-1.0f, -1.0f, -1.0f);	// 	-1 	-1	0
    Vertices[9] = Vector3f(-1.0f, -1.0f, 1.0f);		// 	-1 	-1	4
    Vertices[10] = Vector3f(-1.0f, 1.0f, 1.0f);		//	 1	 1	7
    Vertices[11] = Vector3f(-1.0f, 1.0f, -1.0f);	//	 1	 1	3
    //Right side
    Vertices[12] = Vector3f(1.0f, -1.0f, -1.0f);	// 	 1	-1	1
    Vertices[13] = Vector3f(1.0f, -1.0f, 1.0f);		// 	 1	-1	5
    Vertices[14] = Vector3f(1.0f, 1.0f, 1.0f);		//	 1	 1	6
    Vertices[15] = Vector3f(1.0f, 1.0f, -1.0f);		//	 1	 1	2
    //Down side
    Vertices[16] = Vector3f(-1.0f, -1.0f, -1.0f);	// 	-1 	-1 	0
    Vertices[17] = Vector3f(-1.0f, -1.0f, 1.0f);	// 	-1 	-1	4
    Vertices[18] = Vector3f(1.0f, -1.0f, 1.0f);		// 	 1	-1	5
    Vertices[19] = Vector3f(1.0f, -1.0f, -1.0f);	// 	 1	-1	1
    //Top side
    Vertices[20] = Vector3f(-1.0f, 1.0f, -1.0f);	//	-1	 1	3
    Vertices[21] = Vector3f(-1.0f, 1.0f, 1.0f);		//	-1	 1	7
    Vertices[22] = Vector3f(1.0f, 1.0f, 1.0f);		//	 1	 1	6
    Vertices[23] = Vector3f(1.0f, 1.0f, -1.0f);		//	 1	 1	2
    //*
	unsigned int Indices[] = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4,
		8, 9, 10, 10, 11, 8,
		12, 13, 14, 14, 15, 12,
		16, 17, 18, 18, 19, 16,
		20, 21, 22, 22, 23, 20
	};
	Vector2f Tex_Vertices[4*6];
	Tex_Vertices[0] = Vector2f(1.0f, 1.0f);
	Tex_Vertices[1] = Vector2f(0.0f, 1.0f);
	Tex_Vertices[2] = Vector2f(0.0f, 0.0f);
	Tex_Vertices[3] = Vector2f(1.0f, 0.0f);

	Tex_Vertices[4] = Vector2f(1.0f, 1.0f);
	Tex_Vertices[5] = Vector2f(0.0f, 1.0f);
	Tex_Vertices[6] = Vector2f(0.0f, 0.0f);
	Tex_Vertices[7] = Vector2f(1.0f, 0.0f);

	Tex_Vertices[8] = Vector2f(1.0f, 1.0f);
	Tex_Vertices[9] = Vector2f(0.0f, 1.0f);
	Tex_Vertices[10] = Vector2f(0.0f, 0.0f);
	Tex_Vertices[11] = Vector2f(1.0f, 0.0f);

	Tex_Vertices[12] = Vector2f(1.0f, 1.0f);
	Tex_Vertices[13] = Vector2f(0.0f, 1.0f);
	Tex_Vertices[14] = Vector2f(0.0f, 0.0f);
	Tex_Vertices[15] = Vector2f(1.0f, 0.0f);

	Tex_Vertices[16] = Vector2f(1.0f, 1.0f);
	Tex_Vertices[17] = Vector2f(0.0f, 1.0f);
	Tex_Vertices[18] = Vector2f(0.0f, 0.0f);
	Tex_Vertices[19] = Vector2f(1.0f, 0.0f);

	Tex_Vertices[20] = Vector2f(1.0f, 1.0f);
	Tex_Vertices[21] = Vector2f(0.0f, 1.0f);
	Tex_Vertices[22] = Vector2f(0.0f, 0.0f);
	Tex_Vertices[23] = Vector2f(1.0f, 0.0f);

	Vector3f Normals[4*6];
    CalcNormals(Indices, 3*2*6, Vertices, Normals, 4*6);

	cube.GenBuffers(Indices, sizeof(Indices), Vertices, Tex_Vertices, Normals, 4*6);
}

void InitCubeLines()
{
	Vector3f Vertices[8];
    Vertices[0] = Vector3f(-1.0f, -1.0f, -1.0f);
    Vertices[1] = Vector3f(1.0f, -1.0f, -1.0f);
    Vertices[2] = Vector3f(-1.0f, 1.0f, -1.0f);
    Vertices[3] = Vector3f(1.0f, 1.0f, -1.0f);
    Vertices[4] = Vector3f(-1.0f, -1.0f, 1.0f);
    Vertices[5] = Vector3f(1.0f, -1.0f, 1.0f);
    Vertices[6] = Vector3f(-1.0f, 1.0f, 1.0f);
    Vertices[7] = Vector3f(1.0f, 1.0f, 1.0f);

	unsigned int Indices[] = {
			0, 1,
			1, 3,
			2, 3,
			2, 0,

			0, 4,
			4, 6,
			6, 2,
			6, 7,

			7, 5,
			5, 4,
			1, 5,
			3, 7
	};
	cubeLines.GenBuffers(Indices, sizeof(Indices), Vertices, NULL, NULL, 8);

}

void InitsquareMass(float radius, int count)
{
	Vector3f Vertices[count*count*6 + count*6];
	for (int i = 0; i < count - 1; i++)
		for (int j = 0; j < count - 1; j++)
		{
			Vertices[i*count*6 + j*6] 		= Vector3f(-radius + 2*i*radius/(count-1), 		0.0f, -radius + 2*j*radius/(count-1));
			Vertices[i*count*6 + j*6 + 1] 	= Vector3f(-radius + 2*(i+1)*radius/(count-1), 	0.0f, -radius + 2*j*radius/(count-1));
			Vertices[i*count*6 + j*6 + 2] 	= Vector3f(-radius + 2*(i+1)*radius/(count-1), 	0.0f, -radius + 2*(j+1)*radius/(count-1));
			Vertices[i*count*6 + j*6 + 3] 	= Vector3f(-radius + 2*(i+1)*radius/(count-1), 	0.0f, -radius + 2*(j+1)*radius/(count-1));
			Vertices[i*count*6 + j*6 + 4] 	= Vector3f(-radius + 2*i*radius/(count-1), 		0.0f, -radius + 2*(j+1)*radius/(count-1));
			Vertices[i*count*6 + j*6 + 5] 	= Vector3f(-radius + 2*i*radius/(count-1), 		0.0f, -radius + 2*j*radius/(count-1));
			//std::cout << Vertices[i*count + j].x << " " << Vertices[i*count + j].y << " " << Vertices[i*count + j].z << std::endl;
		}
	Vector3f Normals[count*count*6 + count*6];
    CalcNormals(NULL, 0, Vertices, Normals, count*count*6 + count*6);
	squareMass.GenBuffers(NULL, NULL, Vertices, NULL, Normals, count*count*6 + count*6);
	squareMass.PutData(radius, count);
}

void InitSquare(float radius, int count)
{
	Vector3f Vertices[4];
	Vertices[0] = Vector3f(-radius , 0.0f, -radius);
	Vertices[1] = Vector3f(radius , 0.0f, -radius);
	Vertices[2] = Vector3f(radius , 0.0f, radius);
	Vertices[3] = Vector3f(-radius , 0.0f, radius);

	unsigned int Indices[] = {
				0, 1, 2,
				0, 2, 3
	};

	Vector2f Tex_Vertices[4];
		Tex_Vertices[0] = Vector2f(1.0f, 1.0f);
		Tex_Vertices[1] = Vector2f(0.0f, 1.0f);
		Tex_Vertices[2] = Vector2f(0.0f, 0.0f);
		Tex_Vertices[3] = Vector2f(1.0f, 0.0f);

	Vector3f Normals[4];
	CalcNormals(Indices, 6, Vertices, Normals, 4);
	square.GenBuffers(Indices, sizeof(Indices), Vertices, Tex_Vertices, Normals, 4);
	square.PutData(radius, count);
}

void InitSphere(float RADIUS, unsigned int POINT_COUNT)
{
	float x, y, z;
	Vector3f buff[POINT_COUNT*POINT_COUNT];
	Vector2f Tex_Vertices[POINT_COUNT*POINT_COUNT];
	int POINT_COUNT_T = POINT_COUNT - 1;
	for (int i = 0; i < POINT_COUNT; ++i)
	{
		y = RADIUS*cos(i*pi/POINT_COUNT_T);
		for (int j = 0; j < POINT_COUNT; ++j)
		{
			x = RADIUS*sin(i*pi/POINT_COUNT_T)*cos(2*j*pi/POINT_COUNT_T);
			z = RADIUS*sin(i*pi/POINT_COUNT_T)*sin(2*j*pi/POINT_COUNT_T);
			buff[POINT_COUNT*i + j].x = x;
			buff[POINT_COUNT*i + j].y = y;
			buff[POINT_COUNT*i + j].z = z;
			Tex_Vertices[POINT_COUNT*i + j].x = j/(float)POINT_COUNT_T;
			Tex_Vertices[POINT_COUNT*i + j].y = i/(float)POINT_COUNT_T;
		}
	}
	unsigned int tmpsize = POINT_COUNT*POINT_COUNT*6 + POINT_COUNT*6 + 6;
	unsigned int Indices[tmpsize];
	for (int i = 0; i < POINT_COUNT; ++i)
	{
		for (int j = 0; j < POINT_COUNT; ++j)
		{
			Indices[i*POINT_COUNT*6 + j*6] = i*POINT_COUNT + j;
			Indices[i*POINT_COUNT*6 + j*6 + 1] = i*POINT_COUNT +j + 1;
			Indices[i*POINT_COUNT*6 + j*6 + 2] = (i+1)*POINT_COUNT + j;
			Indices[i*POINT_COUNT*6 + j*6 + 3] = i*POINT_COUNT +j + 1;
			Indices[i*POINT_COUNT*6 + j*6 + 4] = (i+1)*POINT_COUNT + j;
			Indices[i*POINT_COUNT*6 + j*6 + 5] = (i+1)*POINT_COUNT + j + 1;
		}
	}
	Vector3f Normals[tmpsize];
    CalcNormals(Indices, POINT_COUNT*POINT_COUNT*6, buff, Normals, POINT_COUNT*POINT_COUNT);
	sphere.GenBuffers(Indices, sizeof(Indices), buff, Tex_Vertices, Normals, POINT_COUNT*POINT_COUNT);
	sphere.PutData(RADIUS, POINT_COUNT);
}

float tmp = 0.0f;

void SetDirectionalLight(const DirectionLight& Light)
{
    glUniform3f(m_dirLightColorLocation, Light.Color.x, Light.Color.y, Light.Color.z);
    glUniform1f(m_dirLightAmbientIntensityLocation, Light.AmbientIntensity);
    Vector3f Direction = Light.Direction;
    Direction.Normalize();
    glUniform3f(m_dirLightDirectionLocation, Direction.x, Direction.y, Direction.z);
    glUniform1f(m_dirLightDiffuseIntensityLocation, Light.DiffuseIntensity);
}

static void RenderSceneCB()
{
    pGameCamera->OnRender();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    InitsquareMass(20, squareCount);
	glUniform1i(ShaderFlagSquare, ShaderFlagSquarei);
    /*
     * это очистка буфера кадра (использует цвет, заданный в glClearColor
     */

    Pipeline p;
    p.SetCamera(pGameCamera->GetPos(), pGameCamera->GetTarget(), pGameCamera->GetUp());
    p.SetPerspectiveProj(60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 100.0f);

    /*
    //Сначала изменять масштаб, потом вращать, потом перемещать!
    p.Rotate(
    		0.0f,	//Повернуть по оси x (в градусах)
    		0.0f,	//Повернуть по оси y (в градусах)
    		0.0f	//Повернуть по оси z (в градусах)
    		);
    p.WorldPos(
    		0.0f, 	//Переместить по x
    		0.0f,  	//Переместить по y
    		0.0f 	//Переместить по z
    		);
    p.Scale(
    		1.0f, 	//Изменить масштаб по х
    		1.0f, 	//Изменить масштаб по y
    		1.0f 	//Изменить масштаб по z
    		);
    */

    glUniformMatrix4fv		(
    						gWVPLocation, 					//Имя Uniform переменной
    						1, 								//Сколько матриц
    						GL_TRUE, 						//TRUE если матрица по хранится построчно
    						(const GLfloat*)p.GetWVPTrans()	//Матрица, которую закидываем
    						);

	SetDirectionalLight(m_directionalLight);

	glUniformMatrix4fv(gCamera, 1, GL_TRUE, (const GLfloat*)p.GetCameraTrans());
	glUniformMatrix4fv(Proj, 1, GL_TRUE, (const GLfloat*)p.GetProjTrans());

    glEnableVertexAttribArray(0);
    //===================================== Grid ==========================================*/
    glEnableVertexAttribArray(2);
	glUniform1i(ShaderFlag, 0);

    glUniform3f(Color, 0.0, 1.0, 0.0);

    glBindBuffer(GL_ARRAY_BUFFER, grid.VBO);
	glVertexAttribPointer	(
							0, 								//Индекс атрибута
							3, 								//Это количество компонентов в атрибуте (3 для X, Y и Z)
							GL_FLOAT, 						//Тип данных для каждого компонента
							GL_FALSE, 						//Хотим ли мы нормализовать атрибуты перед использованием в конвейере.
															//В нашем случае мы хотим, что бы данные передавались не именными
							0, 								//Число байт между 2 экземплярами атрибут
							NULL							//Начальное смещение в структуре, которуе получит наш конвейер
							);

    glBindBuffer(GL_ARRAY_BUFFER, grid.VBO_NORMALS);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glDrawArrays			(								//Не использует индексную отрисовку
							GL_LINES, 						//Что рисуем
							0, 								//С какой позиции бежим по массиву вершин
							2 * (grid.count * 2)			//Сколько точек передаем в буфер
							);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(2);
    //===================================== Cube ==========================================*/
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
	glUniform1i(ShaderFlag, 1);

    p.WorldPos(0.0f, 1.0f, 0.0f);
    glUniform3f(Color, 1.0, 1.0, 0.0);
    glUniformMatrix4fv(gWVPLocation, 1, GL_TRUE, (const GLfloat*)p.GetWVPTrans());
    glUniformMatrix4fv		(
    						gWorldLocation, 					//Имя Uniform переменной
    						1, 									//Сколько матриц
    						GL_TRUE, 							//TRUE если матрица по хранится построчно
    						(const GLfloat*)p.GetWorldTrans()	//Матрица, которую закидываем
    						);
    glBindBuffer(GL_ARRAY_BUFFER, cube.VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, cube.VBO_TEX);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, cube.VBO_NORMALS);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube.IBO);
    pTexture[2]->Bind(GL_TEXTURE0);

    glDrawElements(GL_TRIANGLES, 12*3, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);

    //=================================== CubeLines ========================================*/
    glEnableVertexAttribArray(2);
	glUniform1i(ShaderFlag, 0);

    p.WorldPos(0.0f, 1.0f, 0.0f);
    glUniformMatrix4fv(gWVPLocation, 1, GL_TRUE, (const GLfloat*)p.GetWVPTrans());

    glUniform3f(Color, 1.0, 0.0, 0.0);

    glBindBuffer(GL_ARRAY_BUFFER, cubeLines.VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeLines.IBO);

    glDrawElements(GL_LINES, 12*2, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(2);

    //==================================== Earth ===========================================*/
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glUniform1i(ShaderFlag, 1);

    p.WorldPos(0.0f, 10.0f, 10.0f);
    glUniformMatrix4fv(gWVPLocation, 1, GL_TRUE, (const GLfloat*)p.GetWVPTrans());
    glUniformMatrix4fv		(
    						gWorldLocation, 					//Имя Uniform переменной
    						1, 									//Сколько матриц
    						GL_TRUE, 							//TRUE если матрица по хранится построчно
    						(const GLfloat*)p.GetWorldTrans()	//Матрица, которую закидываем
    						);

    glUniform3f(Color, 1.0, 0.0, 1.0);

    glBindBuffer(GL_ARRAY_BUFFER, sphere.VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, sphere.VBO_TEX);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, sphere.VBO_NORMALS);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere.IBO);
    pTexture[4]->Bind(GL_TEXTURE0);

    glDrawElements(GL_TRIANGLES, sphere.count*sphere.count*6, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);

    //==================================== Moon ===========================================*/
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glUniform1i(ShaderFlag, 1);

    tmp+=1.0f;
    if (tmp == 360.0f)
    	tmp = 0.0f;
    p.Scale(
    		0.5f, 	//Изменить масштаб по х
    		0.5f, 	//Изменить масштаб по y
    		0.5f 	//Изменить масштаб по z
    		);
    p.WorldPos(5.0f*cosf(ToRadian(tmp)), 10.0f, 10.0f + 3.0f*sinf(ToRadian(tmp)));
    glUniformMatrix4fv(gWVPLocation, 1, GL_TRUE, (const GLfloat*)p.GetWVPTrans());
    glUniformMatrix4fv		(
    						gWorldLocation, 					//Имя Uniform переменной
    						1, 									//Сколько матриц
    						GL_TRUE, 							//TRUE если матрица по хранится построчно
    						(const GLfloat*)p.GetWorldTrans()	//Матрица, которую закидываем
    						);

    glUniform3f(Color, 1.0, 0.0, 1.0);

    glBindBuffer(GL_ARRAY_BUFFER, sphere.VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, sphere.VBO_TEX);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, sphere.VBO_NORMALS);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere.IBO);
    pTexture[5]->Bind(GL_TEXTURE0);

    glDrawElements(GL_TRIANGLES, sphere.count*sphere.count*6, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);

    //===================================== Sun ===========================================*/
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glUniform1i(ShaderFlag, 1);
    p.Scale(
    		0.4f, 	//Изменить масштаб по х
    		0.4f, 	//Изменить масштаб по y
    		0.4f 	//Изменить масштаб по z
    		);
    p.WorldPos(LightPos.x, LightPos.y, LightPos.z);
    glUniformMatrix4fv(gWVPLocation, 1, GL_TRUE, (const GLfloat*)p.GetWVPTrans());
    glUniformMatrix4fv		(
    						gWorldLocation, 					//Имя Uniform переменной
    						1, 									//Сколько матриц
    						GL_TRUE, 							//TRUE если матрица по хранится построчно
    						(const GLfloat*)p.GetWorldTrans()	//Матрица, которую закидываем
    						);
	glUniform3f(LightPosition, LightPos.x, LightPos.y, LightPos.z);

    glUniform3f(Color, 1.0, 0.0, 1.0);

    glBindBuffer(GL_ARRAY_BUFFER, sphere.VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, sphere.VBO_TEX);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, sphere.VBO_NORMALS);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere.IBO);
    pTexture[0]->Bind(GL_TEXTURE0);

    glDrawElements(GL_TRIANGLES, sphere.count*sphere.count*6, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);

	//================================= Procedural_0 ==========================================*/

    glEnableVertexAttribArray(2);
	glUniform1i(ShaderFlag, 2);

    p.WorldPos(0.0f, -10.0f, 0.0f);
    glUniform3f(Color0, 1.0, 1.0, 0.0);
    glUniform3f(Color1, 1.0, 0.0, 1.0);
    glUniform1i(HowCell, squareMass.count);
    glUniform1f(WidthHeight, squareMass.radius);
    glUniform3f(StartPos, 0.0f, -10.0f, 0.0f);
    glUniformMatrix4fv(gWVPLocation, 1, GL_TRUE, (const GLfloat*)p.GetWVPTrans());
    glUniformMatrix4fv		(
    						gWorldLocation, 					//Имя Uniform переменной
    						1, 									//Сколько матриц
    						GL_TRUE, 							//TRUE если матрица по хранится построчно
    						(const GLfloat*)p.GetWorldTrans()	//Матрица, которую закидываем
    						);
    glBindBuffer(GL_ARRAY_BUFFER, squareMass.VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, squareMass.VBO_NORMALS);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glDrawArrays			(								//Не использует индексную отрисовку
							GL_TRIANGLES, 					//Что рисуем
							0, 								//С какой позиции бежим по массиву вершин
							squareMass.count*squareMass.count*6 + squareMass.count*6			//Сколько точек передаем в буфер
							);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(2);

	//================================= Procedural_1 ==========================================*/
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
	glUniform1i(ShaderFlag, 3);

    p.WorldPos(-20.0f, 10.0f, 0.0f);
    p.Rotate(0, 90, 90);
    glUniformMatrix4fv(gWVPLocation, 1, GL_TRUE, (const GLfloat*)p.GetWVPTrans());
    glUniformMatrix4fv		(
    						gWorldLocation, 					//Имя Uniform переменной
    						1, 									//Сколько матриц
    						GL_TRUE, 							//TRUE если матрица по хранится построчно
    						(const GLfloat*)p.GetWorldTrans()	//Матрица, которую закидываем
    						);
    glBindBuffer(GL_ARRAY_BUFFER, square.VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, square.VBO_TEX);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, square.VBO_NORMALS);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, square.IBO);


    glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);

    //==================================== End ==============================================*/

    glDisableVertexAttribArray(0);
    glutSwapBuffers();
    /*
     * Просит GLUT поменять фоновый буфер и буфер кадра местами.
     * В следующем вызове рендер будет происходить в текущий буфер кадра, а фоновый буфер будет отображается
     */
}


static void SpecialKeyboardCB(int Key, int x, int y)
{
	if ((Key == GLUT_KEY_UP)||
		(Key == GLUT_KEY_DOWN)||
		(Key == GLUT_KEY_LEFT)||
		(Key == GLUT_KEY_RIGHT))
		pGameCamera->OnSpecialKeyboard(Key, x, y);
	else
	{
		switch(Key)
		{
		case GLUT_KEY_F1:
			ShaderFlagSquarei++;
			ShaderFlagSquarei = ShaderFlagSquarei%2;
			break;
		}
	}
}


static void KeyboardCB(unsigned char Key, int x, int y)
{
	if ((Key == 'w')||(Key == 's')||(Key == 'a')||(Key == 'd'))
		pGameCamera->OnKeyboard(Key, x, y);
	else
		switch(Key)
		{
		case 27:
			exit(0);
		case 'z':
            m_directionalLight.AmbientIntensity -= 0.05f;
            break;
		case 'x':
            m_directionalLight.AmbientIntensity += 0.05f;
            break;
        case 'c':
            m_directionalLight.DiffuseIntensity -= 0.5f;
            break;
        case 'v':
            m_directionalLight.DiffuseIntensity += 0.5f;
            break;
        case '8':
            LightPos.z += 0.3f;
            break;
        case '2':
            LightPos.z -= 0.3f;
            break;
        case '4':
            LightPos.x -= 0.3f;
            break;
        case '6':
            LightPos.x += 0.3f;
            break;
        case '7':
            LightPos.y -= 0.3f;
            break;
        case '9':
            LightPos.y += 0.3f;
            break;
        case '-':
            if (squareCount > 2)
            	squareCount--;
            break;
        case '+':
            if (squareCount <= 100)
            	squareCount++;
            break;
		}
}

static void PassiveMouseCB(int x, int y)
{
    pGameCamera->OnMouse(x, y);
}

static void InitializeGlutCallbacks() 				//Будут обрабатываться в glutMainLoop
{
    //glutDisplayFunc(RenderSceneCB);
    glutIdleFunc(RenderSceneCB);					//Ленивая функция
    glutSpecialFunc(SpecialKeyboardCB);				//Отлавливаем специальные клавиши
    glutPassiveMotionFunc(PassiveMouseCB);			//OnMove
    glutKeyboardFunc(KeyboardCB);					//Отлавливаем остальные клавиши
}

static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
    GLuint ShaderObj = glCreateShader(ShaderType);	//Создать шейдер

    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
    }

    const GLchar* p[1];
    p[0] = pShaderText;
    GLint Lengths[1];
    Lengths[0]= strlen(pShaderText);
    glShaderSource(ShaderObj, 1, p, Lengths);		//Инициализируем код шейдера
    glCompileShader(ShaderObj);
    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }

    glAttachShader(ShaderProgram, ShaderObj); 		//Присоединяем объект шейдера к шейдерной программе
}

static void CompileShaders()
{
    GLuint ShaderProgram = glCreateProgram();		//Создание шейдерной программы, сюда будет все линковаться

    if (ShaderProgram == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    AddShader(ShaderProgram, pVS, GL_VERTEX_SHADER);
    AddShader(ShaderProgram, pFS, GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };

    glLinkProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
	} //Проверяем сможет ли запуститься

    glValidateProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }
    //Проверяем программные ошибки

    glUseProgram(ShaderProgram); 								//Назначаем шейдерную программу для конвейера

    gWVPLocation = glGetUniformLocation(ShaderProgram, "gWVP");	//Инициализировали шейдерную переменную
	if (gWVPLocation == -1){
		cout << "could not bind " << "gWVP" << endl;
		return;
	}
	gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld");	//Инициализировали шейдерную переменную
		if (gWVPLocation == -1){
			cout << "could not bind " << "gWorld" << endl;
			return;
		}
    Color = glGetUniformLocation(ShaderProgram, "Color");
    if (Color == -1){
    	cout << "could not bind " << "Color" << endl;
    	return;
    }
    gSampler = glGetUniformLocation(ShaderProgram, "gSampler");
    if (Color == -1){
    	cout << "could not bind " << "gSampler" << endl;
    	return;
    }
    ShaderFlag = glGetUniformLocation(ShaderProgram, "flag");
    if (Color == -1){
    	cout << "could not bind " << "flag" << endl;
    	return;
    }
    ShaderFlagSquare = glGetUniformLocation(ShaderProgram, "flagSquare");
    if (Color == -1){
    	cout << "could not bind " << "flagSquare" << endl;
    	return;
    }
    m_dirLightColorLocation = glGetUniformLocation(ShaderProgram, "gDirectionalLight.Color");
    if (m_dirLightColorLocation == -1){
    	cout << "could not bind " << "gDirectionalLight.Color" << endl;
    	return;
    }
    m_dirLightAmbientIntensityLocation = glGetUniformLocation(ShaderProgram, "gDirectionalLight.AmbientIntensity");
    if (m_dirLightAmbientIntensityLocation == -1){
    	cout << "could not bind " << "gDirectionalLight.AmbientIntensity" << endl;
    	return;
    }
    m_dirLightDirectionLocation = glGetUniformLocation(ShaderProgram, "gDirectionalLight.Direction");
    m_dirLightDiffuseIntensityLocation = glGetUniformLocation(ShaderProgram, "gDirectionalLight.DiffuseIntensity");
    LightPosition = glGetUniformLocation(ShaderProgram, "LightPosition");
    gCamera = glGetUniformLocation(ShaderProgram, "gCamera");
    if (m_dirLightAmbientIntensityLocation == -1){
    	cout << "could not bind " << "gCamera" << endl;
    	return;
    }

    Proj = glGetUniformLocation(ShaderProgram, "Proj");
    if (m_dirLightAmbientIntensityLocation == -1){
    	cout << "could not bind " << "Proj" << endl;
    	return;
    }

    GLuint StartPos;
    StartPos = glGetUniformLocation(ShaderProgram, "StartPos");
    WidthHeight = glGetUniformLocation(ShaderProgram, "WidthHeight");
    HowCell = glGetUniformLocation(ShaderProgram, "HowCell");
    Color0 = glGetUniformLocation(ShaderProgram, "Color0");
    Color1 = glGetUniformLocation(ShaderProgram, "Color1");

}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    /*
     * Здесь мы инициализируем GLUT.
     */
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    /*
     * Передаем параметры отображения
     * Так настраиваются некоторые опции GLUT.
     * GLUT_DOUBLE включает двойную буферизацию (отрисовка будет происходить в фоновый буфер,
     * в то время как другой буфер отображается) и буфер цвета, непрерывно используемого для конечной цели рендеринга
     * (т.е. на экране). Мы будем использовать как эти 2 опции, так и другие, с которыми познакомимся позже.
     */
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
	glutInitContextVersion(3, 3);							//Версия опенджиель
	glutInitContextProfile(GLUT_CORE_PROFILE);				//Юзаем кор шейдеры
	glewExperimental = GL_TRUE;								//Я знаю, что это плохо и надоы было делать на VBO
    glutCreateWindow("HELLO");
    glutGameModeString("1920x1080@32");
    /*
     * Заметим, что разрешение и количество цветов задаются через строку в функции.
     * 32 бита для пикселя обеспечивают наибольшее количество цветов для рендера.
     */
    glutEnterGameMode();

    InitializeGlutCallbacks();								//Инициализируем функции

    pGameCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT);
    //pos = pGameCamera->GetPos();
    GLenum res = glewInit(); 								//Инициализируем глю, если версия опенджиель старая, то выдаст ошибку
    //Но я узал VAO
    if (res != GLEW_OK) {
      fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
      return 1;
    }
    //std::cout << "!!!" << std::endl;
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    /*
     * glClearColor устанавливает цвет, который будет использован во время очистки буфера кадра
     */

    CompileShaders();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

    glUniform1i(gSampler, 0);
    pTexture[0] = new texture(GL_TEXTURE_2D, "textures/0.jpg");
    pTexture[1] = new texture(GL_TEXTURE_2D, "textures/1.png");
    pTexture[2] = new texture(GL_TEXTURE_2D, "textures/2.jpg");
    pTexture[3] = new texture(GL_TEXTURE_2D, "textures/3.jpg");
    pTexture[4] = new texture(GL_TEXTURE_2D, "textures/4.png");
    pTexture[5] = new texture(GL_TEXTURE_2D, "textures/5.png");
    pTexture[6] = new texture(GL_TEXTURE_2D, "textures/6.bmp");
    pTexture[7] = new texture(GL_TEXTURE_2D, "textures/7.jpg");
    pTexture[8] = new texture(GL_TEXTURE_2D, "textures/8.jpg");
    pTexture[9] = new texture(GL_TEXTURE_2D, "textures/9.jpg");
    pTexture[10] = new texture(GL_TEXTURE_2D, "textures/10.jpg");
    pTexture[11] = new texture(GL_TEXTURE_2D, "textures/11.jpg");
    InitGrid(100, 0.5f);
    InitCube();
    InitCubeLines();
    InitSphere(1.0f, 50);
    InitSquare(20, squareCount);
    for (int i = 0; i < 12; ++i)
    	if (!pTexture[i]->Load()) {
    		std::cout << i << std::endl;
    		return 1;
    	}

    m_directionalLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
    m_directionalLight.AmbientIntensity = 0.5f;
    m_directionalLight.Direction = Vector3f(1.0f, 0.0, 1.0);
    m_directionalLight.DiffuseIntensity = 3.75f;

    glutMainLoop();
    /*
     * Этот вызов передаёт контроль GLUT’у, который теперь начнёт свой собственный цикл.
     * В этом цикле он ждёт событий от оконной системы и передаёт их через функции обратного вызова,
     * которые мы задали ранее
     */

    return 0;
}
