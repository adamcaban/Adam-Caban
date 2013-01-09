#include "stdafx.h"
#include <windows.h>
#include <GLTools.h>
#include <GL/glew.h>
#define FREEGLUT_STATIC
#include <GL/freeglut.h>
#pragma comment(lib, "gltools.lib")
#include <GLFrustum.h>
#include <GLFrame.h>
#include <StopWatch.h>
#include <GLBatch.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>



GLuint shader;
GLuint MVMatrixLocation;
GLFrame cameraFrame;
GLFrustum viewFrustum;
CStopWatch timer;
GLMatrixStack modelView;
GLMatrixStack projection;
GLGeometryTransform geometryPipeline;
GLMatrixStack matrixStack;
GLuint MVPMatrixLocation;
GLuint normalMatrixLocation;
GLuint ambientLightLocation;
GLuint shaderPositionLocation;
GLuint shaderColorLocation;
GLuint shaderAngleLocation;
GLuint shaderAttenuation0Location;
GLuint shaderAttenuation1Location;
GLuint shaderAttenuation2Location;
GLuint shaderAmbientColorLocation;
GLuint shaderDiffuseColorLocation;
GLuint shaderSpecularColorLocation;
GLuint shaderSpecularExponentLocation;
CStopWatch timer;
GLuint textureID[1];

float angle;
// float vertices_tab[3 * 12] = {
      // 0., 0., -0.9510565162951536,
      // 0., 0., 0.9510565162951536,
      // -0.85065080835204, 0., -0.42532540417601994,
      // 0.85065080835204, 0., 0.42532540417601994,
      // 0.6881909602355868, -0.5, -0.42532540417601994,
      // 0.6881909602355868, 0.5, -0.42532540417601994,
      // -0.6881909602355868, -0.5, 0.42532540417601994,
      // -0.6881909602355868, 0.5, 0.42532540417601994,
      // -0.2628655560595668, -0.8090169943749475, -0.42532540417601994,
      // -0.2628655560595668, 0.8090169943749475, -0.42532540417601994,
      // 0.2628655560595668, -0.8090169943749475, 0.42532540417601994,
      // 0.2628655560595668, 0.8090169943749475, 0.42532540417601994
      // };
// int faces_tab[3*20]={
      // 1 ,			 11 ,			 7 ,
      // 1 ,			 7 ,			 6 ,
      // 1 ,			 6 ,			 10 ,
      // 1 ,			 10 ,			 3 ,
      // 1 ,			 3 ,			 11 ,
      // 4 ,			 8 ,			 0 ,
      // 5 ,			 4 ,			 0 ,
      // 9 ,			 5 ,			 0 ,
      // 2 ,			 9 ,			 0 ,
      // 8 ,			 2 ,			 0 ,
      // 11 ,			 9 ,			 7 ,
      // 7 ,			 2 ,			 6 ,
      // 6 ,			 8 ,			 10 ,
      // 10 ,			 4 ,			 3 ,
      // 3 ,			 5 ,			 11 ,
      // 4 ,			 10 ,			 8 ,
      // 5 ,			 3 ,			 4 ,
      // 9 ,			 11 ,			 5 ,
      // 2 ,			 7 ,			 9 ,
      // 8 ,			 6 ,			 2 };


// void ChangeSize(int width, int height) {
    // glViewport(0, 0, width, height);
    // viewFrustum.SetPerspective(60.0f, width/height, 0.1f, 1000.0f);
// }

void Texture2f(float s, float t) {
    glVertexAttrib2f(GLT_ATTRIBUTE_TEXTURE0, s, t);
}

void SetUpFrame(GLFrame &frame, const M3DVector3f origin, const M3DVector3f forward, const M3DVector3f up) {
frame.SetOrigin(origin);
frame.SetForwardVector(forward);
    M3DVector3f side,oUp;
    m3dCrossProduct3(side,forward,up);
    m3dCrossProduct3(oUp,side,forward);
    frame.SetUpVector(oUp);
    frame.Normalize();
}

void LookAt(GLFrame &frame, const M3DVector3f eye, const M3DVector3f at, const M3DVector3f up) {
    M3DVector3f forward;
    m3dSubtractVectors3(forward, at, eye);
    SetUpFrame(frame, eye, forward, up);
}

void SetupRC() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    shader = gltLoadShaderPairWithAttributes("pass_thru_shader.vp", "pass_thru_shader.fp", 2, GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_NORMAL, "vNormal");
    fprintf(stdout, "GLT_ATTRIBUTE_VERTEX : %d\nGLT_ATTRIBUTE_COLOR : %d \n", GLT_ATTRIBUTE_VERTEX, GLT_ATTRIBUTE_COLOR);

shaderColorLocation = glGetUniformLocation(shader, "light1.color");
shaderAngleLocation = glGetUniformLocation(shader, "light1.angle");
shaderAttenuation0Location = glGetUniformLocation(shader, "light1.attenuation0");
shaderAttenuation1Location = glGetUniformLocation(shader, "light1.attenuation1");
shaderAttenuation2Location = glGetUniformLocation(shader, "light1.attenuation2");

    MVMatrixLocation = glGetUniformLocation(shader,"MVMatrix");
    MVPMatrixLocation = glGetUniformLocation(shader,"MVPMatrix");
    normalMatrixLocation = glGetUniformLocation(shader,"normalMatrix");
    ambientLightLocation = glGetUniformLocation(shader,"ambientLight");
    shaderPositionLocation = glGetUniformLocation(shader, "light1.position");

shaderAmbientColorLocation = glGetUniformLocation(shader, "material.ambientColor");
    shaderDiffuseColorLocation = glGetUniformLocation(shader, "material.diffuseColor");
    shaderSpecularColorLocation = glGetUniformLocation(shader, "material.specularColor");
    shaderSpecularExponentLocation = glGetUniformLocation(shader, "material.specularExponent");
if(MVMatrixLocation==-1){
        fprintf(stderr,"uniform MVMatrix could not be found\n");
    }
    
geometryPipeline.SetMatrixStacks(modelView, projection);
    M3DVector3f eye = {1.0f, 1.0f, 1.0f};
    M3DVector3f at = {5.0f, 5.0f, 5.0f};
    M3DVector3f up = {1.0f, 1.0f, 1.0f};
    LookAt(cameraFrame, eye, at, up);
}

void TriangleFace(M3DVector3f a, M3DVector3f b, M3DVector3f c) {
M3DVector3f normal, bMa, cMa;
m3dSubtractVectors3(bMa, b, a);
m3dSubtractVectors3(cMa, c, a);
m3dCrossProduct3(normal, bMa, cMa);
m3dNormalizeVector3(normal);
glVertexAttrib3fv(GLT_ATTRIBUTE_NORMAL, normal);
Texture2f(1, 1);
glVertex3fv(c);
Texture2f(0, 0);
glVertex3fv(b);
Texture2f(1, 0);
glVertex3fv(a);
}

void piramida(){

	modelView.PushMatrix();
        
    glUniformMatrix4fv(MVPMatrixLocation, 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());
    glUniformMatrix4fv(MVMatrixLocation, 1, GL_FALSE, transformPipeline.GetModelViewMatrix());
    glUniformMatrix3fv(normalMatrixLocation, 1, GL_FALSE, transformPipeline.GetNormalMatrix());
	glBindTexture(GL_TEXTURE_2D, textureID[0]);
	
    M3DVector3f a = {-1.0f, -1.0f, 0.0f};
    M3DVector3f b = {1.0f, -1.0f, 0.0f};
    M3DVector3f c = {1.0f, 1.0f, 0.0f};
    M3DVector3f d = {-1.0f, 1.0f, 0.0f};
    M3DVector3f e = {0.0f, 0.0f, 2.0f};

    glBegin(GL_QUADS);
	glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 0.6f, 0.0f, 0.0f);
        glVertex3fv(a);
        glVertex3fv(b);
        glVertex3fv(c);
        glVertex3fv(d);
    glEnd();
	glBegin(GL_TRIANGLES);
	glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 1.0f, 0.6f, 0.0f);
        TriangleFace(e, a, b);
    glEnd();
    glBegin(GL_TRIANGLES);
	glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 0.75, 0.15f, 0.5f);
        TriangleFace(e, b, c);
    glEnd();
    glBegin(GL_TRIANGLES);
	glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 0.0f, 0.55f, 0.75f);
        TriangleFace(e, c, d);
    glEnd();
    glBegin(GL_TRIANGLES);
	glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 0.0f, 1.0f, 0.0f);
        TriangleFace(e, d, a);
    glEnd();
}

void grid(){
    glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    for(int i=-10;i<11;i++){
        for(int j=-10; j<11;j++){
            glVertex3f(i, j, 0.0f);
            glVertex3f(-i, j, 0.0f);
            glVertex3f(i, -j, 0.0f);
            glVertex3f(i, j, 0.0f);
        }
    }
    glEnd();
}

bool LoadTGATexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode) {
    GLbyte *pBits;
    int nWidth, nHeight, nComponents;
    GLenum eFormat;

    pBits = gltReadTGABits(szFileName, &nWidth, &nHeight, &nComponents, &eFormat);
    if (pBits == NULL)
    return false;
	
    fprintf(stderr, "Reading from: %s %dx%d\n", szFileName, nWidth, nHeight);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nWidth, nHeight, 0,eFormat, GL_UNSIGNED_BYTE, pBits);
    free(pBits);
    return true;
}

void ChangeSize(int w, int h) {
    glViewport(0, 0, w, h);
	viewFrustum.SetPerspective(40.0f,w/h,4.5f,300.0f);
}

void SetupRC() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
	
    shader = gltLoadShaderPairWithAttributes("pass_thru_shader.vp", "pass_thru_shader.fp",4,GLT_ATTRIBUTE_VERTEX, "vVertex",GLT_ATTRIBUTE_COLOR, "vColor",GLT_ATTRIBUTE_TEXTURE0, "texCoord0",GLT_ATTRIBUTE_NORMAL, "vNormal");
	
    fprintf(stdout, "GLT_ATTRIBUTE_VERTEX : %d\nGLT_ATTRIBUTE_COLOR : %d \nGLT_ATTRIBUTE_TEXTURE0 : %d\n",GLT_ATTRIBUTE_VERTEX, GLT_ATTRIBUTE_COLOR,GLT_ATTRIBUTE_TEXTURE0);

    glGenTextures(1, textureID);
    glBindTexture(GL_TEXTURE_2D, textureID[0]);
    if (!LoadTGATexture("texture.tga", GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE)) {
        fprintf(stderr, "Error while loading\n");
    }

	MVPMatrixLocation = glGetUniformLocation(shader, "MVPMatrix");
    shaderTextureLocation = glGetUniformLocation(shader, "texture0");
    shaderPositionLocation = glGetUniformLocation(shader, "lightPosition");
    shaderDiffuseColorLocation = glGetUniformLocation(shader, "diffuseColor");
    shaderAmbientColorLocation = glGetUniformLocation(shader, "ambientColor");
    shaderSpecularColorLocation = glGetUniformLocation(shader, "specularColor");
    MVMatrixLocation = glGetUniformLocation(shader, "MVMatrix");
    normalMatrixLocation = glGetUniformLocation(shader, "normalMatrix");
    alphaLocation = glGetUniformLocation(shader, "alpha");

	transformPipeline.SetMatrixStacks(modelView, projectionMatrix);

    M3DVector3f eye = {12.0f, -30.0f, 20.0f};
    M3DVector3f at = {0.0f, 0.0f, 0.0f};
    M3DVector3f up = {0.0f, 0.0f, 1.0f};

    LookAt(cameraFrame, eye, at, up);

    M3DMatrix44f mCamera;

    cameraFrame.GetCameraMatrix(mCamera);
}

void ShutdownRC()

void RenderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, textureID[0]);
    glUseProgram(shader);
	M3DVector3f position={10.0f, 10.0f, 3.0f};

    M3DMatrix44f mCamera;
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
	modelView.PushMatrix();
    cameraFrame.GetCameraMatrix(mCamera);
    modelView.LoadMatrix(mCamera);
	modelView.PushMatrix();

    glUniform4f(shaderDiffuseColorLocation, 1.0f, 1.0f, 1.0f, 1.0f);
    glUniform4f(shaderAmbientColorLocation, 0.3f, 0.30f, 0.30f, 1.0f);
	glUniform4f(shaderSpecularColorLocation, 0.4f, 0.4f, 0.4f, 1.0f);
	glUniform3fv(shaderPositionLocation, 1, position);
    glUniform1i(shaderTextureLocation, 0);

    glPolygonOffset(1.0f, 1.0f);
	grid();
	glEnable(GL_POLYGON_OFFSET_FILL);
	grid();
	glDisable(GL_POLYGON_OFFSET_FILL);

    modelView.PushMatrix();
	modelView.Scale(2.0f, 2.0f, 2.0f);
	DrawPyramid();
	modelView.PopMatrix();
	modelView.PushMatrix();

	modelView.Translate(5.0f, -5.0f, 0.0f);
	piramida();

	modelView.PopMatrix();
    glutSwapBuffers();
}

int main(int argc, char* argv[]) {


    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Texture");
    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }

    SetupRC();

    glutMainLoop();
    return 0;
}