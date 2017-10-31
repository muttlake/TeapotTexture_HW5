// 
//  Timothy Shepard
//  HW5
//  Teapot Texture
//

#include <Windows.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ObjFileLoaderGithub/Source/OBJ_Loader.h"
#include "SOIL.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

//using namespace std;

//const int NUM_VERTICES = 2688;
const int NUM_INDICES = 18960;

#define BUFFER_OFFSET(i) ((char *)NULL + (i))
GLuint shaderProgramID;
GLuint vao = 0;
GLuint vbo;
GLuint vno;
GLuint positionID, texCoordID, normalID;
GLuint indexBufferID;
GLuint positionBuffer, texCoordBuffer, normalBuffer;
GLuint brickTextureID, bumpTextureID;	
GLuint brick_texture, bump_texture;
// The ID of the "texture" variable in the shader

struct Vertex {
	GLfloat x, y, z;
	GLfloat nx, ny, nz;
	GLfloat r, g, b, a;
};

///////////////////////////////////////////////////////////////
// Read and Compile Shaders from tutorial
///////////////////////////////////////////////////////////////
static char* readFile(const char* filename) {
	FILE* fp = fopen(filename, "r");
	fseek(fp, 0, SEEK_END);
	long file_length = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* contents = new char[file_length + 1];
	for (int i = 0; i < file_length + 1; i++) {
		contents[i] = 0;
	}
	fread(contents, 1, file_length, fp);
	contents[file_length + 1] = '\0';  // end of string in C
	fclose(fp);
	return contents;
}

bool compiledStatus(GLint shaderID) {
	GLint compiled = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);
	if (compiled) {
		return true;
	}
	else {
		GLint logLength;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
		char* msgBuffer = new char[logLength];
		glGetShaderInfoLog(shaderID, logLength, NULL, msgBuffer);
		printf("%s\n", msgBuffer);
		delete (msgBuffer);
		return false;
	}
}

void printShaderSource(const char* shaderSource, std::string shaderName) {
	printf("--------------------------------------------------------------\n");
	printf("Printing %s shader:\n", shaderName.c_str());
	printf(shaderSource);
	printf("\n");
}

GLuint makeVertexShader(const char* shaderSource) {
	printShaderSource(shaderSource, "vertex");
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderID, 1, (const GLchar**)&shaderSource, NULL);
	glCompileShader(vertexShaderID);
	bool compiledCorrectly = compiledStatus(vertexShaderID);
	if (compiledCorrectly) {
		return vertexShaderID;
	}
	return -1;
}

GLuint makeFragmentShader(const char* shaderSource) {
	printShaderSource(shaderSource, "fragment");
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderID, 1, (const GLchar**)&shaderSource, NULL);
	glCompileShader(fragmentShaderID);
	bool compiledCorrectly = compiledStatus(fragmentShaderID);
	if (compiledCorrectly) {
		return fragmentShaderID;
	}
	return -1;
}

GLuint makeShaderProgram(GLuint vertexShaderID, GLuint fragmentShaderID) {
	GLuint shaderID = glCreateProgram();
	glAttachShader(shaderID, vertexShaderID);
	glAttachShader(shaderID, fragmentShaderID);
	glLinkProgram(shaderID);
	return shaderID;
}

///////////////////////////////////////////////////////////////
// Read File for Tris.txt
///////////////////////////////////////////////////////////////
int getNumTriangles(std::string filename)
{
	std::ifstream inputFile;
	inputFile.open(filename);
	int numTriangles = 0;
	inputFile >> numTriangles;
	inputFile.close();
	return numTriangles;
}

Vertex* getAllVertices(std::string filename)
{
	std::ifstream inputFile;
	inputFile.open(filename);

	int numTriangles = 0;
	inputFile >> numTriangles;

	Vertex* allVertices;
	allVertices = new Vertex[numTriangles * 3];  // numVertices is numTriangles*3

	int currentIndex = 0;
	while (!inputFile.eof())
	{
		std::string currentLine;
		std::getline(inputFile, currentLine);
		if (currentLine == "") { continue; }
		std::stringstream ss;
		ss << currentLine;
		Vertex newVertex;
		ss >> newVertex.x >> newVertex.y >> newVertex.z;
		ss >> newVertex.nx >> newVertex.ny >> newVertex.nz;
		ss >> newVertex.r >> newVertex.g >> newVertex.b >> newVertex.a;
		allVertices[currentIndex] = newVertex;
		currentIndex++;
	}

	inputFile.close();
	return allVertices;
}

void printAllVertices(Vertex* allVertices, int numTriangles)
{
	int triangleCount = 0;
	std::cout << "Printing all vertices.\n";
	for (int i = 0; i < numTriangles * 3; i++)
	{
		if (i % 3 == 0) { std::cout << "Triangle " << ++triangleCount << std::endl; }
		Vertex v;
		v = allVertices[i];
		std::cout << "Vertex: " << i << " ";
		std::cout << v.x << " " << v.y << " " << v.z << " ";
		std::cout << v.nx << " " << v.ny << " " << v.nz << " ";
		std::cout << v.r << " " << v.b << " " << v.g << " " << v.a << std::endl;
	}
}

glm::vec3* buildPositionsVec3s(Vertex* allVertices, int numVertices)
{
	glm::vec3* allPositions;
	allPositions = new glm::vec3[numVertices];
	for (int i = 0; i < numVertices; i++)
	{
		Vertex v;
		v = allVertices[i];
		allPositions[i] = glm::vec3(v.x, v.y, v.z);
	}
	return allPositions;
}

void printAllPositions(glm::vec3* allPositions, int numVertices)
{
	std::cout << "Printing all position vec3:\n";
	for (int i = 0; i < numVertices; i++)
	{
		if (i % 3 == 0)
		{
			std::cout << "Triangle: " << int(i / 3 + 0.5) << "\n";
		}
		std::cout << "Position For Vertex " << i << ": ";
		std::cout << allPositions[i][0] << " " << allPositions[i][1];
		std::cout << " " << allPositions[i][2] << std::endl;
	}
}

glm::vec3* buildNormalsVec3s(Vertex* allVertices, int numVertices)
{
	glm::vec3* allNormals;
	allNormals = new glm::vec3[numVertices];
	for (int i = 0; i < numVertices; i++)
	{
		Vertex v;
		v = allVertices[i];
		allNormals[i] = glm::vec3(v.nx, v.ny, v.nz);
	}
	return allNormals;
}

void printAllNormals(glm::vec3* allNormals, int numVertices)
{
	std::cout << "Printing all normals vec3:\n";
	for (int i = 0; i < numVertices; i++)
	{
		if (i % 3 == 0)
		{
			std::cout << "Triangle: " << int(i / 3 + 0.5) << "\n";
		}
		std::cout << "Normal For Vertex " << i << ": ";
		std::cout << allNormals[i][0] << " " << allNormals[i][1];
		std::cout << " " << allNormals[i][2] << std::endl;
	}
}

void printAllColors(glm::vec4* allColors, int numVertices)
{
	std::cout << "Printing all color vec4:\n";
	for (int i = 0; i < numVertices; i++)
	{
		if (i % 3 == 0)
		{
			std::cout << "Triangle: " << int(i / 3 + 0.5) << "\n";
		}
		std::cout << "Color For Vertex " << i << ": ";
		std::cout << allColors[i][0] << " " << allColors[i][1] << " ";
		std::cout << allColors[i][2] << " " << allColors[i][3] << std::endl;
	}
}

glm::vec4* buildColorsVec4s(Vertex* allVertices, int numVertices)
{
	glm::vec4* allColors;
	allColors = new glm::vec4[numVertices];
	for (int i = 0; i < numVertices; i++)
	{
		Vertex v;
		v = allVertices[i];
		allColors[i] = glm::vec4(v.r, v.g, v.b, v.a);
	}
	return allColors;
}

GLuint* getTriangleIndicesArray(int numVertices)
{
	GLuint* triangleIndicesList;
	triangleIndicesList = new GLuint[numVertices];
	for (int i = 0; i < numVertices; i++)
	{
		triangleIndicesList[i] = GLuint(i);
	}
	return triangleIndicesList;
}

void printTriangleIndices(GLuint* triangleIndices, int numVertices)
{
	std::cout << "Printing all indices.\n";
	for (int i = 0; i < numVertices; i++)
	{
		std::cout << triangleIndices[i] << "\n";
	}
}

void printObjContents(objl::Loader loader) {
	// Create/Open e1Out.txt
	std::ofstream file("e1Out.txt");

	// Go through each loaded mesh and out its contents
	for (int i = 0; i < loader.LoadedMeshes.size(); i++)
	{
		// Copy one of the loaded meshes to be our current mesh
		objl::Mesh curMesh = loader.LoadedMeshes[i];

		// Print Mesh Name
		file << "Mesh " << i << ": " << curMesh.MeshName << "\n";

		// Print Vertices
		file << "Vertices:\n";

		// Go through each vertex and print its number,
		//  position, normal, and texture coordinate
		for (int j = 0; j < curMesh.Vertices.size(); j++)
		{
			file << "V" << j << ": " <<
				"P(" << curMesh.Vertices[j].Position.X << ", " << curMesh.Vertices[j].Position.Y << ", " << curMesh.Vertices[j].Position.Z << ") " <<
				"N(" << curMesh.Vertices[j].Normal.X << ", " << curMesh.Vertices[j].Normal.Y << ", " << curMesh.Vertices[j].Normal.Z << ") " <<
				"TC(" << curMesh.Vertices[j].TextureCoordinate.X << ", " << curMesh.Vertices[j].TextureCoordinate.Y << ")\n";
		}

		// Print Indices
		file << "Indices:\n";

		// Go through every 3rd index and print the
		//      triangle that these indices represent
		for (int j = 0; j < curMesh.Indices.size(); j += 3)
		{
			file << "T" << j / 3 << ": " << curMesh.Indices[j] << ", " << curMesh.Indices[j + 1] << ", " << curMesh.Indices[j + 2] << "\n";
		}

		// Print Material
		file << "Material: " << curMesh.MeshMaterial.name << "\n";
		file << "Ambient Color: " << curMesh.MeshMaterial.Ka.X << ", " << curMesh.MeshMaterial.Ka.Y << ", " << curMesh.MeshMaterial.Ka.Z << "\n";
		file << "Diffuse Color: " << curMesh.MeshMaterial.Kd.X << ", " << curMesh.MeshMaterial.Kd.Y << ", " << curMesh.MeshMaterial.Kd.Z << "\n";
		file << "Specular Color: " << curMesh.MeshMaterial.Ks.X << ", " << curMesh.MeshMaterial.Ks.Y << ", " << curMesh.MeshMaterial.Ks.Z << "\n";
		file << "Specular Exponent: " << curMesh.MeshMaterial.Ns << "\n";
		file << "Optical Density: " << curMesh.MeshMaterial.Ni << "\n";
		file << "Dissolve: " << curMesh.MeshMaterial.d << "\n";
		file << "Illumination: " << curMesh.MeshMaterial.illum << "\n";
		file << "Ambient Texture Map: " << curMesh.MeshMaterial.map_Ka << "\n";
		file << "Diffuse Texture Map: " << curMesh.MeshMaterial.map_Kd << "\n";
		file << "Specular Texture Map: " << curMesh.MeshMaterial.map_Ks << "\n";
		file << "Alpha Texture Map: " << curMesh.MeshMaterial.map_d << "\n";
		file << "Bump Map: " << curMesh.MeshMaterial.map_bump << "\n";

		// Leave a space to separate from the next mesh
		file << "\n";
	}

	// Close File
	file.close();
}

GLuint* buildIndicesListFromObj(objl::Loader loader)
{
	GLuint* triangleIndicesList;
	objl::Mesh curMesh = loader.LoadedMeshes[0];
	triangleIndicesList = new GLuint[curMesh.Indices.size()];
	//      triangle that these indices represent
	for (int j = 0; j < curMesh.Indices.size(); j++)
	{
		triangleIndicesList[j] = curMesh.Indices[j];
	}
	return triangleIndicesList;
}

glm::vec3* buildPositionsVec3sFromObj(objl::Loader loader)
{
	glm::vec3* allPositions;
	objl::Mesh curMesh = loader.LoadedMeshes[0];
	allPositions = new glm::vec3[curMesh.Vertices.size()];
	for (int j = 0; j < curMesh.Vertices.size(); j++)
	{
		allPositions[j] = glm::vec3(curMesh.Vertices[j].Position.X, curMesh.Vertices[j].Position.Y, curMesh.Vertices[j].Position.Z);
	}
	return allPositions;
}

glm::vec3* buildNormalsVec3sFromObj(objl::Loader loader)
{
	glm::vec3* allNormals;
	objl::Mesh curMesh = loader.LoadedMeshes[0];
	allNormals = new glm::vec3[curMesh.Vertices.size()];
	for (int j = 0; j < curMesh.Vertices.size(); j++)
	{
		allNormals[j] = glm::vec3(curMesh.Vertices[j].Normal.X, curMesh.Vertices[j].Normal.Y, curMesh.Vertices[j].Normal.Z);
	}
	return allNormals;
}

glm::vec2* buildTexCoordVec2sFromObj(objl::Loader loader)
{
	glm::vec2* allTexCoords;
	objl::Mesh curMesh = loader.LoadedMeshes[0];
	allTexCoords = new glm::vec2[curMesh.Vertices.size()];
	for (int j = 0; j < curMesh.Vertices.size(); j++)
	{
		allTexCoords[j] = glm::vec2(curMesh.Vertices[j].TextureCoordinate.X, curMesh.Vertices[j].TextureCoordinate.Y);
	}
	return allTexCoords;
}

void printAllTexCoords(glm::vec2* allTexCoords, int numVertices)
{
	std::cout << "Printing all texture coordinates vec2:\n";
	for (int i = 0; i < numVertices; i++)
	{
		std::cout << "Texture Coordinate For Vertex " << i << ": ";
		std::cout << allTexCoords[i][0] << " " << allTexCoords[i][1];
		std::cout << std::endl;
	}
}

///////////////////////////////////////////////////////////////
// Main Part of program
///////////////////////////////////////////////////////////////

// changeViewport
void changeViewport(int w, int h) {
	glViewport(0, 0, w, h);
}

// render
void render() {
	glEnable(GL_DEPTH_TEST);	// Turn on depth culling
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	//When rendering an objectwith this program.




	//glActiveTexture(GL_TEXTURE0);

	// Set the preferences
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glDrawElements(GL_TRIANGLES, NUM_INDICES, GL_UNSIGNED_INT, NULL);




	glDrawArrays(GL_TRIANGLES, 0, NUM_INDICES);



	//glActiveTexture(GL_TEXTURE0);



	glutSwapBuffers();
}

void switchMVP(unsigned char key, int xmouse, int ymouse)
{
	GLuint MVPID = glGetUniformLocation(shaderProgramID, "MVP");
	GLuint MVID = glGetUniformLocation(shaderProgramID, "MV");
	GLuint shaderCVID = glGetUniformLocation(shaderProgramID, "shaderChooserVec");

	//set shadingType boolean, Tried int but could not pass uniform
	glm::vec3 shaderChooserVec = glm::vec3(0.0f, 0.0f, 0.0f);

	glm::mat4 M = glm::mat4(1.0f);
	glm::mat4 P = glm::mat4(1.0f);
	glm::mat4 V = glm::mat4(1.0f);
	glm::mat4 MV = V*M;
	glm::mat4 MVP = P*V*M;

	switch (key) {
	case '1':
		//#1 Model Transformation, Orthographic Projection, Camera Transformation
		M = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		P = glm::ortho(-2.4f, 2.4f, -1.8f, 1.8f, 1.0f, 50.0f);
		V = glm::lookAt(glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		MV = V*M;
		MVP = P*V*M;
		shaderChooserVec = glm::vec3(1.0f, 1.0f, 1.0f);
		break;

	case '2':
		//#2 Model Transformation, Perspective Projection, Camera Transformation
		M = glm::rotate(M, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		M = glm::translate(M, glm::vec3(0.0f, -1.0f, 0.0f));
		P = glm::perspective(glm::radians(50.0f), 4.0f / 3.0f, 1.0f, 50.0f);
		V = glm::lookAt(glm::vec3(3.0f, 3.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		MV = V*M;
		MVP = P*V*M;
		shaderChooserVec = glm::vec3(1.0f, 1.0f, 1.0f);
		break;

	case '3':
		//#3 Model Transformation, Orthographic Projection, Camera Transformation, Same as #1
		M = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		P = glm::ortho(-2.4f, 2.4f, -1.8f, 1.8f, 1.0f, 50.0f);
		V = glm::lookAt(glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		MV = V*M;
		MVP = P*V*M;
		shaderChooserVec = glm::vec3(2.0f, 2.0f, 2.0f);
		break;

	case '4':
		//#4 Model Transformation, Perspective Projection, Camera Transformation, Same as #2
		M = glm::rotate(M, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		M = glm::translate(M, glm::vec3(0.0f, -1.0f, 0.0f));
		P = glm::perspective(glm::radians(50.0f), 4.0f / 3.0f, 1.0f, 50.0f);
		V = glm::lookAt(glm::vec3(3.0f, 3.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		MV = V*M;
		MVP = P*V*M;
		shaderChooserVec = glm::vec3(2.0f, 2.0f, 2.0f);
		break;

	default:
		//Default copy number 1
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		break;
	}

	glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(MVID, 1, GL_FALSE, &MV[0][0]);
	glUniform3fv(shaderCVID, 1, glm::value_ptr(shaderChooserVec));
	glutPostRedisplay(); //request display() call ASAP
}

// main program
int main(int argc, char** argv) {

	// Get vertices from Tris.txt file
	//std::string filename = "Tris1.txt";
	//int numTriangles = getNumTriangles(filename);

	//Read obj file
	objl::Loader loader;
	loader.LoadFile("teapot2.obj");
	printf("\nLoaded teapot2.obj\n\n");
	printObjContents(loader);

	int numIndices = loader.LoadedMeshes[0].Indices.size();
	GLuint* vindices;
	vindices = buildIndicesListFromObj(loader);
	//printTriangleIndices(vindices, numVertices);

	int numVertices = loader.LoadedMeshes[0].Vertices.size();
	glm::vec3* vpositions;
	vpositions = buildPositionsVec3sFromObj(loader);
	//printAllPositions(vpositions, numVertices);

	printf("--------------------------------------------------------------\n");
	std::cout << "The real number of indices is: " << numIndices << "\n";
	std::cout << "The real number of vertices is: " << numVertices << "\n";
	printf("--------------------------------------------------------------\n");

	glm::vec3* vnormals;
	vnormals = buildNormalsVec3sFromObj(loader);
	//printAllNormals(vnormals, numVertices);

	glm::vec2* vtex_coords;
	vtex_coords = buildTexCoordVec2sFromObj(loader);
	//printAllTexCoords(vtex_coords, numVertices);


	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Timothy_Shepard_HW5.zip");
	glutReshapeFunc(changeViewport);
	glutDisplayFunc(render);
	glewInit();  //glewInit() for Windows only
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GL Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n\n";
	printf("--------------------------------------------------------------\n");

	///////////////////////// Start Texture /////////////////////////
	//Copied this from SOIL documentation
	brick_texture = SOIL_load_OGL_texture
	(
		"brick.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	if (0 == brick_texture)
	{
		printf("SOIL loading error: '%s'\n", SOIL_last_result());
	}
	else {
		printf("SOIL loaded file brick.png\n");
	}

	bump_texture = SOIL_load_OGL_texture
	(
		"bump.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	if (0 == bump_texture)
	{
		printf("SOIL loading error: '%s'\n", SOIL_last_result());
	}
	else {
		printf("SOIL loaded file bump.jpg\n");
	}
	
	///////////////////////// End   Texture /////////////////////////


	// Make a shader
	char* vertexShaderSourceCode = readFile("vertexShader.vsh");
	char* fragmentShaderSourceCode = readFile("fragmentShader.vsh");
	GLuint vertShaderID = makeVertexShader(vertexShaderSourceCode);
	GLuint fragShaderID = makeFragmentShader(fragmentShaderSourceCode);
	shaderProgramID = makeShaderProgram(vertShaderID, fragShaderID);

	// Make MVP and MV transformation matrixes
	glm::mat4 M = glm::mat4(1.0f);
	M = glm::rotate(M, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	M = glm::translate(M, glm::vec3(0.0f, -10.0f, 0.0f));
	glm::mat4 P = glm::ortho(-24.0f, 24.0f, -18.0f, 18.0f, 1.0f, 500.0f);
	glm::mat4 V = glm::lookAt(glm::vec3(10.0f, 50.0f, 100.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 MV = V*M;
	glm::mat4 MVP = P*V*M;

	// Change for APPLE version
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	//glGenVertexArraysAPPLE(1, &vao);
	//glBindVertexArrayAPPLE(vao);

	//Make buffers for vertex attributes and assign them to shader attributes
	glGenBuffers(1, &positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(glm::vec3), vpositions, GL_STATIC_DRAW);

	glGenBuffers(1, &texCoordBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(glm::vec2), vtex_coords, GL_STATIC_DRAW);

	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(glm::vec3), vnormals, GL_STATIC_DRAW);

	positionID = glGetAttribLocation(shaderProgramID, "s_vPosition");
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	texCoordID = glGetAttribLocation(shaderProgramID, "s_vTexCoord");
	glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
	glVertexAttribPointer(texCoordID, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);

	normalID = glGetAttribLocation(shaderProgramID, "s_vNormal");
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glVertexAttribPointer(normalID, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	///////////////////////// Start Texture /////////////////////////

	//bump texture





	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, bump_texture);
	//tex2ID = glGetUniformLocation(shaderProgramID, "bumpTexture");
	//glActiveTexture(GL_TEXTURE1);				// Turn on texture unit 1
	//glUniform1i(tex2ID, 1);

	////brick texture
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, brick_texture);
	//texID = glGetUniformLocation(shaderProgramID, "brickTexture");
	//glActiveTexture(GL_TEXTURE0);				// Turn on texture unit 0
	//glUniform1i(texID, 0);

	// Turn on texture unit 0
	//glActiveTexture(GL_TEXTURE0);


	//brickTextureID = glGetUniformLocation(shaderProgramID, "brickTexture");
	//bumpTextureID = glGetUniformLocation(shaderProgramID, "bumpTexture");
	////glUseProgram(program);
	//glUniform1i(brickTextureID, 0); //Texture unit 0 is for base images.
	//glUniform1i(bumpTextureID, 1); //Texture unit 2 is for normal maps.

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, brick_texture);

	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, bump_texture);

	//glActiveTexture(GL_TEXTURE0);

	//// Load textures
	//GLuint textures[2];
	//glGenTextures(2, textures);

	//int width, height;
	//unsigned char* image;

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, textures[0]);
	//	image = SOIL_load_image("brick.png", &width, &height, 0, SOIL_LOAD_RGB);
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	//	SOIL_free_image_data(image);
	//glUniform1i(glGetUniformLocation(shaderProgramID, "brickTexture"), 0);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, textures[1]);
	//	image = SOIL_load_image("bump.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	//	SOIL_free_image_data(image);
	//glUniform1i(glGetUniformLocation(shaderProgramID, "bumpTexture"), 1);

	////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glActiveTexture(GL_TEXTURE0);

	///////////////////////// End   Texture /////////////////////////


	// Put indices in Index Buffer
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint), vindices, GL_STATIC_DRAW);


	//use shader program
	glUseProgram(shaderProgramID);

	brickTextureID = glGetUniformLocation(shaderProgramID, "brickTexture");
	glUniform1i(brickTextureID, 0);

	bumpTextureID = glGetUniformLocation(shaderProgramID, "bumpTexture");
	glUniform1i(bumpTextureID, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, brick_texture);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bump_texture);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);





	GLuint MVPID = glGetUniformLocation(shaderProgramID, "MVP");
	glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVP[0][0]);

	GLuint MVID = glGetUniformLocation(shaderProgramID, "MV");
	glUniformMatrix4fv(MVID, 1, GL_FALSE, &MV[0][0]);

	//Directional Lights
	glm::vec3 light1direction = glm::vec3(0.0f, -1.0f, 0.0f);
	glm::vec3 light1color = glm::vec3(1.0f, 1.0f, 1.0f);
	GLuint light1d = glGetUniformLocation(shaderProgramID, "light1direction");
	glUniform3fv(light1d, 1, glm::value_ptr(light1direction));
	GLuint light1c = glGetUniformLocation(shaderProgramID, "light1color");
	glUniform3fv(light1c, 1, glm::value_ptr(light1color));

	glm::vec3 light2direction = glm::vec3(-1.0f, 0.0f, 0.0f);
	glm::vec3 light2color = glm::vec3(1.0f, 1.0f, 1.0f);
	GLuint light2d = glGetUniformLocation(shaderProgramID, "light2direction");
	glUniform3fv(light2d, 1, glm::value_ptr(light2direction));
	GLuint light2c = glGetUniformLocation(shaderProgramID, "light2color");
	glUniform3fv(light2c, 1, glm::value_ptr(light2color));

	glm::vec3 light3direction = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 light3color = glm::vec3(1.0f, 1.0f, 1.0f);
	GLuint light3d = glGetUniformLocation(shaderProgramID, "light3direction");
	glUniform3fv(light3d, 1, glm::value_ptr(light3direction));
	GLuint light3c = glGetUniformLocation(shaderProgramID, "light3color");
	glUniform3fv(light3c, 1, glm::value_ptr(light3color));

	//glutKeyboardFunc(switchMVP);
	glEnableVertexAttribArray(positionID);
	glEnableVertexAttribArray(texCoordID);
	glEnableVertexAttribArray(normalID);

	glutMainLoop();

	// Delete dynamic arrays
	delete[] vindices;
	delete[] vpositions;
	delete[] vnormals;
	delete[] vtex_coords;

	return 0;
}