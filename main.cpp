#define GL_TEST_MAIN main

#include "Game.h"
#include "ServiceLocator.h"
#include "KeyboardHandler.h"
#include "MouseHandler.h"
#include "Window.h"

#include <iostream>
#include <vector>
#include <cassert>
#include "GL/glew.h"
#include "GL/freeglut.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "IL/ilut.h"

#include "TestCallbacks.h"

/*enum ATTRIB_INDEX
{
	A_POSITION,		// vertex location, point in space
	A_BLWT,			// irrelevant for now (bl. wt)
	A_NORMAL,		// normal vector
	A_COLOR0,		// colour
	A_COLOR1,		// alt colour
	A_FOG,			// irrelevant for now (fog)
	A_PTSZ,			// point size
	A_BLIND,		// irrelevant for now (bl. wt)
	A_TEXCOORD0,	// texcoord
	A_TEXCOORD1,	// "
	A_TEXCOORD2,	// "
	A_TEXCOORD3,	// "
	A_TEXCOORD4,	// "
	A_TEXCOORD5,	// "
	A_TEXCOORD6,	// "
	A_TEXCOORD7,	// "

	ATTRIB_COUNT	// 16
};*/

/*
// Fuck it, global variable
Camera* gActiveSceneCamera;
//Erm
GameObject* gSceneObjects;
void resize(int width, int height) {
	glViewport(0, 0, width, height);
	gActiveSceneCamera->resize(width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//gSceneGeometry[0].render(gActiveSceneCamera);
	//gSceneGeometry[1].render(gActiveSceneCamera);
	glutSwapBuffers();
}

void resize(int width, int height) {
	Game::getInstance().resize(width, height);

void timer(int value) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (gSceneObjects) {
		gSceneObjects[0].render(gActiveSceneCamera);
		gSceneObjects[1].render(gActiveSceneCamera);
		gSceneObjects[2].render(gActiveSceneCamera);
	}
	glutSwapBuffers();
	glutTimerFunc(10, timer, 0);
}

void static_resize(Camera* c, int width, int height) {
	static Camera* camera = NULL;
	if (c)
		camera = c;
	camera->resize(width, height);
}
void resize_wrapper(int width, int height) {
	static_resize(NULL, width, height);
}

std::ostream& operator<<(std::ostream& out, glm::mat4 matrix) {
	out << *(mat4*)glm::value_ptr(matrix);
	return out;
}
*/

void game_resize_wrapper(int width, int height) {
	Game::getInstance().resize(width, height);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Game::getInstance().render(0.0f);
	glutSwapBuffers();
}

void game_keydown_wrapper(unsigned char key, int mouse_x, int mouse_y) {
	// Temporary hard break
	if (key == 27)
		glutLeaveMainLoop();
	else
		KeyboardHandler::getInstance().handlePress(key, mouse_x, mouse_y);
}

void game_keyup_wrapper(unsigned char key, int mouse_x, int mouse_y) {
	KeyboardHandler::getInstance().handleRelease(key, mouse_x, mouse_y);
}

void game_mouse_wrapper(int button, int edge, int mouse_x, int mouse_y) {
	MouseHandler::getInstance().handleButton((MouseButton)button, edge, mouse_x, mouse_y);
}

void game_movement_wrapper(int mouse_x, int mouse_y) {
	MouseHandler::getInstance().handleMove(mouse_x, mouse_y);
}

GLint64 getCurrentTime() {
	GLint64 time;
	glGetInteger64v(GL_TIMESTAMP, &time);
	return time;
}

void gameLoop(int value) {
	static GLint64 current, prev = getCurrentTime();
	static float msPerFrame = 10.0f, lag = 0.0f;
	glutTimerFunc((int)msPerFrame, gameLoop, 0);
	current = getCurrentTime();
	GLint64 elapsed = current - prev;
	prev = current;
	lag += elapsed / 1000000.0f;
	while (lag > msPerFrame) {
		KeyboardHandler::getInstance().dispatchAll();	// Why are these here, instead of in Game::update? I must have had a reason, right?
		MouseHandler::getInstance().dispatchAll();
		Game::getInstance().update(msPerFrame / 1000.0f);
		lag -= msPerFrame;
	}
	glClear(GL_DEPTH_BUFFER_BIT);
	Game::getInstance().render(lag);
	glutSwapBuffers();
}

int GL_TEST_MAIN(int argc, char* argv[]) {
#pragma region initGL
	glutInit(&argc, argv);

#pragma region initEKH
	ServiceLocator::provideLoggingService(new ConsoleLoggingService);
	Game::getInstance().init();
/*
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 2.0f));
	view = glm::rotate(view, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	view = glm::rotate(view, glm::radians(-25.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	glm::mat4 newview = glm::lookAt(glm::vec3(1.5f, 1.5f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 thirdview = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	thirdview = glm::translate(thirdview, glm::vec3(0.0f, 0.0f, 4.0f));

	//PerspCamera camera(view, 800, 600, 75.0f);
	OrthoCamera camera(800, 600);
	static_resize(&camera, 800, 600);
	std::cout << view << std::endl << newview << std::endl;// << thirdview << std::endl;
	gActiveSceneCamera = &camera;
	Window display(camera.getWidth(), camera.getHeight(), "Test");
*/
#pragma endregion

	int version[2];
	glGetIntegerv(GL_MAJOR_VERSION, version);
	glGetIntegerv(GL_MINOR_VERSION, version + 1);
	ServiceLocator::getLoggingService().log("Version: " + std::to_string(version[0]) + "." + std::to_string(version[1]));

#pragma region initglew
	if (glewInit() == GLEW_OK) {
		glGetIntegerv(GL_MAJOR_VERSION, version);
		glGetIntegerv(GL_MINOR_VERSION, version + 1);
		ServiceLocator::getLoggingService().log("Version: " + std::to_string(version[0]) + "." + std::to_string(version[1]));

#pragma region callbacks
		glutDisplayFunc(testDisplay);
		glutTimerFunc(42, gameLoop, 0);
		glutCloseFunc(testCloseWindow);
		glutReshapeFunc(game_resize_wrapper);
		glutPositionFunc(testPositionWindow);
		glutKeyboardFunc(game_keydown_wrapper);
		glutKeyboardUpFunc(game_keyup_wrapper);
		glutMouseFunc(game_mouse_wrapper);
		glutMotionFunc(game_movement_wrapper);
		glutPassiveMotionFunc(testMouseMove);
		glutEntryFunc(testMouseEntry);
#pragma endregion
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_MULTISAMPLE);
		//glEnable(GL_FRAMEBUFFER_SRGB);
		//Not so sure about this one
		glActiveTexture(GL_TEXTURE0);
		//glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
	}
	else {
		ServiceLocator::getLoggingService().error("Error initiating glew", "");
		return 1;
	}
#pragma endregion
#pragma endregion
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
	/*
#pragma region testInterleavedVBO
#pragma region generate
	//Interleaved VBO
	float* interleavedData = new float[tempcube_numverts * 3 * 2];
	for (unsigned int i = 0; i < tempcube_numverts; i++) {
		for (int j = 0; j < 3; j++) {
			interleavedData[i * 6 + j] = tempcube_vertices[i * 3 + j];
			interleavedData[i * 6 + j + 3] = tempcube_colors[i * 3 + j];
		}
	}
	float* square_interleaved = new float[square_numverts * 3 * 2];
	for (unsigned int i = 0; i < square_numverts; i++) {
		for (int j = 0; j < 3; j++) {
			square_interleaved[i * 6 + j] = square_vertices[i * 3 + j];
			square_interleaved[i * 6 + j + 3] = square_colors[i * 3 + j];
		}
	}
#pragma endregion
	// Old-model everything
	/*gSceneGeometry = new Geometry[2]{ Geometry(numverts, interleavedData, numtris, tris, { A_POSITION,A_COLOR0 }), Geometry(numverts, interleavedData, numtris, tris, { A_POSITION,A_COLOR0 }) };
	gSceneGeometry[1].scale(glm::vec3(0.5f, 0.5f, 0.5f));
	gSceneGeometry[1].translate(glm::vec3(0.0f, 0.0f, 2.0f));
	gSceneGeometry[0].transfer();
	gSceneGeometry[1].transfer();
	delete[] interleavedData;
	gSceneGeometry[0].cleanup();
	gSceneGeometry[1].cleanup();
	Program program(new Shader("mvp.vs", GL_VERTEX_SHADER), new Shader("test.fs", GL_FRAGMENT_SHADER));
	gSceneGeometry[0].setDisplay(&program);
	gSceneGeometry[1].setDisplay(&program);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glUniform4f(2, 1.0f, 1.0f, 1.0f, 1.0f);
	//float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	//glUniform4fv(2, 1, color);
	gSceneGeometry[0].render(gActiveSceneCamera);
	gSceneGeometry[1].render(gActiveSceneCamera);*/
	/*
	float buttonverts[12] = { 100.0f,100.0f,0.0f, 700.0f,100.0f,0.0f, 100.0f,200.0f,0.0f, 700.0f,200.0f,0.0f };
	unsigned int buttontris[6] = { 0,2,1,1,2,3 };

	// Improved object-oriented everything
	gSceneObjects = new GameObject[3]{
	GameObject(
		new Geometry(tempcube_numverts, interleavedData, tempcube_numfaces, tempcube_faces, { A_POSITION, A_COLOR0 }),
		new Program(
			new Shader("glsl/mvp_color.vert", GL_VERTEX_SHADER),
			new Shader("glsl/test.frag", GL_FRAGMENT_SHADER)
			),
		new PhysicsComponent(glm::mat4(1.0f)),
		NULL
		),
		GameObject(
			new Geometry(numverts, interleavedData, numtris, tris, {A_POSITION, A_COLOR0}),
			new Program(
				new Shader("mvp.vert", GL_VERTEX_SHADER),
				new Shader("test.frag", GL_FRAGMENT_SHADER)
			),
			new PhysicsComponent(glm::mat4(1.0f)),
			NULL
		),
		GameObject(
			new Geometry(square_numverts, buttonverts, square_numfaces, buttontris, {A_POSITION}),
			new Program(
				new Shader("mvp.vert", GL_VERTEX_SHADER),
				new Shader("test.frag", GL_FRAGMENT_SHADER)
			),
			new PhysicsComponent(glm::mat4()),
			NULL
		)
	};
	// Transformations go here
	delete[] interleavedData;
	delete[] square_interleaved;
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#pragma endregion
*/
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glutSwapBuffers();
	Game::getInstance().load();
	glutMainLoop();
	ServiceLocator::getLoggingService().log("After the main loop");
	system("pause");
	return 0;
}

int MATRIX_TEST_MAIN(int argc, char* argv[]) {
/*	sizeof(vec4);
	sizeof(mat4);
	vec4 myvector(1.0, 1.0, 1.0, 1.0);
	myvector[1] = 2.0;
	mat4 mytranslation = makeTranslationMatrix(1.0, 2.0, 3.0);
	vec4 result = mytranslation * myvector;
	std::cout << myvector << std::endl << mytranslation << std::endl << result << std::endl;
	std::cout << mytranslation[2] << std::endl;
	system("pause");*/
	return 0;
}

int SHADER_TEST_MAIN(int argc, char* argv[]) {
	glutInit(&argc, argv);
	//Window display(800, 600, "Test");
	assert(glewInit() == GLEW_OK);
	Shader vs("test.vs", GL_VERTEX_SHADER);
	Shader fs("test.fs", GL_FRAGMENT_SHADER);
	Program program(&vs, &fs);
	program.link();
	program.validate();
	system("pause");
	return 0;
}

struct Vortex {
	int x, y, z;
};

int OBJ_TEST_MAIN(int argc, char* argv[]) {
	ServiceLocator::provideLoggingService(new ConsoleLoggingService);
	FileService& file = ServiceLocator::getFileService("obj/spaceship.obj");
	char buffer[100];
	char* t1 = new char;
	file.extract("\\Sd", &t1);
	for (int i = 0; i < 25; i++)
		file.in.getline(buffer, 100);
	char* target = new char;
	file.extract("\\S/", &target);
	file.close();
	Geometry test("obj/spaceship.obj");
	system("pause");
	return 0;
}