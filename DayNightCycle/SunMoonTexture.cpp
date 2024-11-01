#include <windows.h>        //	Utilizarea functiilor de sistem Windows (crearea de ferestre, manipularea fisierelor si directoarelor);
#include <stdlib.h>         //  Biblioteci necesare pentru citirea shaderelor;
#include <stdio.h>
#include <GL/glew.h>        //  Define?te prototipurile functiilor OpenGL si constantele necesare pentru programarea OpenGL moderna; 
#include <GL/freeglut.h>    //	Include functii pentru: 
//	- gestionarea ferestrelor si evenimentelor de tastatura si mouse, 
//  - desenarea de primitive grafice precum dreptunghiuri, cercuri sau linii, 
//  - crearea de meniuri si submeniuri;
#include "loadShaders.h"	//	Fisierul care face legatura intre program si shadere;
#include "glm/glm.hpp"		//	Bibloteci utilizate pentru transformari grafice;
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include "SOIL.h"			//	Biblioteca pentru texturare;
#include "SunMoonTexture.h"

GLuint
VaoId,
VboId,
EboId,
ColorBufferId,
ProgramId,
myMatrixLocation,
matrRotLocation,
codColLocation,
sunScreenPosLocation,
moonScreenPosLocation,
windowSizeLocation,
sunTextureID,
moonTextureID,
texCoordsBufferID,
textureLocation;

#define PI 3.14159265359

GLfloat winWidth = 900.0, winHeight = 900.0; // Window Size

glm::mat4 myMatrix, resizeMatrix, matrTransl, matrScale, matrTrans, matrRot;

int codCol;

float
xMin = -500, xMax = 500, yMin = -500, yMax = 500; // World Coordinates

float angularVelocity = 0.01f;

void LoadSunTexture(const char* texturePath)
{
	glGenTextures(1, &sunTextureID);
	glBindTexture(GL_TEXTURE_2D, sunTextureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//Load texture and transfer data in texture object
	int width, height;
	unsigned char* image = SOIL_load_image(texturePath, &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	//Free memory
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void LoadMoonTexture(const char* texturePath)
{
	glGenTextures(1, &moonTextureID);
	glBindTexture(GL_TEXTURE_2D, moonTextureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//Load texture and transfer data in texture object
	int width, height;
	unsigned char* image = SOIL_load_image(texturePath, &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	//Free memory
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void CreateShaders(void)
{
	ProgramId = LoadShaders("SunAndMoon.vert", "SunAndMoon.frag");
	glUseProgram(ProgramId);
}

void CreateVBO(void)
{

	//	Atributele varfurilor -  COORDONATE, CULORI, COORDONATE DE TEXTURARE;
	GLfloat Vertices[] = {
		//	Coordonate;					Culori;				Coordonate de texturare;
		  -100.0f, -100.0f, 0.0f, 1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 0.0f,	// Stanga jos;
		   100.0f, -100.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f,	1.0f, 0.0f, // Dreapta jos;
		   100.0f,  100.0f, 0.0f, 1.0f,   1.0f, 1.0f, 0.0f,	1.0f, 1.0f,	// Dreapta sus;
		  -100.0f,  100.0f, 0.0f, 1.0f,   0.0f, 1.0f, 1.0f,	0.0f, 1.0f  // Stanga sus;
	};

	//	Indicii care determina ordinea de parcurgere a varfurilor;
	GLuint Indices[] = {
	  0, 1, 2,  //	Primul triunghi;
	  0, 2, 3	//  Al doilea triunghi;
	};

	//  Transmiterea datelor prin buffere;

	//  Se creeaza / se leaga un VAO (Vertex Array Object);
	glGenVertexArrays(1, &VaoId);                                                   //  Generarea VAO si indexarea acestuia catre variabila VaoId;
	glBindVertexArray(VaoId);

	//  Se creeaza un buffer pentru VARFURI - COORDONATE, CULORI si TEXTURARE;
	glGenBuffers(1, &VboId);													//  Generarea bufferului si indexarea acestuia catre variabila VboId;
	glBindBuffer(GL_ARRAY_BUFFER, VboId);										//  Setarea tipului de buffer - atributele varfurilor;
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	//	Se creeaza un buffer pentru INDICI;
	glGenBuffers(1, &EboId);														//  Generarea bufferului si indexarea acestuia catre variabila EboId;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);									//  Setarea tipului de buffer - atributele varfurilor;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	//	Se activeaza lucrul cu atribute;
	//  Se asociaza atributul (0 = coordonate) pentru shader;
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
	//  Se asociaza atributul (1 =  culoare) pentru shader;
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
	//  Se asociaza atributul (2 =  texturare) pentru shader;
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));
}


void DestroyVBO(void)
{
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	//  Stergerea bufferelor pentru VARFURI (Coordonate, Culori, Textura), INDICI;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VboId);
	glDeleteBuffers(1, &EboId);

	//  Eliberaea obiectelor de tip VAO;
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);
}

void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
}

void Initialize(void)
{
	glm::vec3 lightSkyColor(0.69f, 0.87f, 0.9f); // Lighter sky blue
	glClearColor(lightSkyColor.r, lightSkyColor.g, lightSkyColor.b, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	CreateVBO();
	
	LoadSunTexture("SunGL.png");
	LoadMoonTexture("MoonGL.png");
	
	CreateShaders();

	codColLocation = glGetUniformLocation(ProgramId, "codCol");
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	sunScreenPosLocation = glGetUniformLocation(ProgramId, "sunScreenPos");
	moonScreenPosLocation = glGetUniformLocation(ProgramId, "moonScreenPos");
	windowSizeLocation = glGetUniformLocation(ProgramId, "windowSize");

}

void RenderFunction(void) {
	glClear(GL_COLOR_BUFFER_BIT);

	static float angle = 0.0f;
	angle -= angularVelocity / 60.f;
	float orbitRadius = 350.0f;

	resizeMatrix = glm::ortho(xMin, xMax, yMin, yMax);
	matrRot = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0, 0.0, 1.0));
	matrTransl = glm::translate(glm::mat4(1.0f), glm::vec3(orbitRadius * cos(angle), orbitRadius * sin(angle), 0.0f));

	// Render Green Square
	codCol = 0;
	glUniform1i(codColLocation, codCol);
	matrScale = glm::scale(glm::mat4(1.0f), glm::vec3(2.f, 2.f, 1.f));
	myMatrix = resizeMatrix * matrScale;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glDrawElements(GL_TRIANGLE_FAN, 6, GL_UNSIGNED_INT, 0);

	// Render Sun
	myMatrix = resizeMatrix * matrTransl;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	glm::vec3 sunScreenPos3D = glm::project(glm::vec3(orbitRadius * cos(angle), orbitRadius * sin(angle), 0.0f),
		glm::mat4(1.0f), resizeMatrix, glm::vec4(0, 0, winWidth, winHeight));
	glm::vec2 sunScreenPos = glm::vec2(sunScreenPos3D.x, sunScreenPos3D.y);

	glUniform2f(sunScreenPosLocation, sunScreenPos.x, sunScreenPos.y);
	glUniform2f(windowSizeLocation, winWidth, winHeight);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sunTextureID);
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);
	codCol = 1;
	glUniform1i(codColLocation, codCol);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Render Moon
	float angleMoon = angle + PI;
	glm::mat4 moonTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(orbitRadius * cos(angleMoon), orbitRadius * sin(angleMoon), 0.0f));
	myMatrix = resizeMatrix * moonTranslation;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	glm::vec3 moonScreenPos3D = glm::project(glm::vec3(orbitRadius * cos(angleMoon), orbitRadius * sin(angleMoon), 0.0f),
		glm::mat4(1.0f), resizeMatrix, glm::vec4(0, 0, winWidth, winHeight));
	glm::vec2 moonScreenPos = glm::vec2(moonScreenPos3D.x, moonScreenPos3D.y);

	glUniform2f(moonScreenPosLocation, moonScreenPos.x, moonScreenPos.y);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, moonTextureID);
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);
	codCol = 2;
	glUniform1i(codColLocation, codCol);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glutSwapBuffers();
	glFlush();
}




void Cleanup(void)
{
	DestroyShaders();
	DestroyVBO();
	glDeleteTextures(1, &sunTextureID);
}

void IdleFunction(void)
{
	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(100, 100); // pozitia initiala a ferestrei
	glutInitWindowSize(winWidth, winHeight); //dimensiunile ferestrei
	glutCreateWindow("Sun and Moon"); // titlul ferestrei
	glewInit(); // nu uitati de initializare glew; trebuie initializat inainte de a a initializa desenarea
	Initialize();
	glutDisplayFunc(RenderFunction);
	glutIdleFunc(IdleFunction);
	glutCloseFunc(Cleanup);
	glutMainLoop();
}

