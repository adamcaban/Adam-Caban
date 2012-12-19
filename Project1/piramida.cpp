#include "stdafx.h"
#include <GLTools.h>
#include <windows.h>
#include <GLFrustum.h>
#include <GLFrame.h>
#include <GLFrustum.h>
#include <GLFrame.h>
#include <StopWatch.h>
#include <GLBatch.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>

#ifdef __APPLE__
#include <glut/glut.h>          // OS X version of GLUT
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif
GLuint shader;
GLuint MVMatrixLocation;
GLFrame cameraFrame;
GLFrustum viewFrustum;
CStopWatch timer;
GLMatrixStack modelView;
GLMatrixStack projection;
GLGeometryTransform geometryPipeline;
GLMatrixStack matrixStack;

void ChangeSize(int w, int h) {
    glViewport(0, 0, w, h);
    viewFrustum.SetPerspective(50.0f, w/h, 0.1f, 1000.0f);
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
    shader = gltLoadShaderPairWithAttributes("pass_thru_shader.vp", "pass_thru_shader.fp", 2, GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_COLOR, "vColor");
    fprintf(stdout, "GLT_ATTRIBUTE_VERTEX : %d\nGLT_ATTRIBUTE_COLOR : %d \n", GLT_ATTRIBUTE_VERTEX, GLT_ATTRIBUTE_COLOR);
    MVMatrixLocation=glGetUniformLocation(shader,"MVMatrix");
    if(MVMatrixLocation==-1){
        fprintf(stderr,"uniform MVMatrix could not be found\n");
    }
    geometryPipeline.SetMatrixStacks(modelView, projection);
    M3DVector3f eye = {1.0f, 1.0f, 10.0f};
    M3DVector3f at = {0.0f, 0.0f, 0.0f};
    M3DVector3f up = {0.0f, 0.0f, 0.5f};
    LookAt(cameraFrame, eye, at, up);
}

void rysujPiramidke(){
		glBegin(GL_QUADS);
    glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 0.73f, 0.73f, 0.73f);
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, 1.0f, 0.0f);
    glEnd();
		glBegin(GL_TRIANGLES);
    glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 2.0f);
    glVertex3f(-1.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glEnd();
		glBegin(GL_TRIANGLES);
    glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 2.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, 0.0f);
    glEnd();
		glBegin(GL_TRIANGLES);
    glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 2.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, 1.0f, 0.0f);
    glEnd();
		 glBegin(GL_TRIANGLES);
    glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 2.0f);
    glVertex3f(1.0f, -1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glEnd();
}

void szachownica(){
    glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 0.0f, 0.0f, 0.0f);
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
void RenderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glFrontFace(GL_CW);
    glUseProgram(shader);
    M3DVector3f at={0.0f, 0.0f, 0.0f};
    M3DVector3f up={0.0f, 0.0f, 1.0f};
    M3DVector3f eye;
    float angle = timer.GetElapsedSeconds()*3.14159f/8;
    eye[0]= 6.8f * cos(angle);
    eye[1]= 6.0f * sin(angle);
    eye[2]= 25.0f;
    LookAt(cameraFrame,eye,at,up);

    projection.LoadMatrix(viewFrustum.GetProjectionMatrix());
    modelView.PushMatrix();
    M3DMatrix44f mCamera;
    cameraFrame.GetCameraMatrix(mCamera);

    modelView.LoadMatrix(mCamera);
    modelView.PushMatrix();
	glUniformMatrix4fv(MVMatrixLocation,1,GL_FALSE,geometryPipeline.GetModelViewProjectionMatrix());
		szachownica();
    modelView.Translate(0.0f, 0.0f, 0.0f);
    glUniformMatrix4fv(MVMatrixLocation,1,GL_FALSE,geometryPipeline.GetModelViewProjectionMatrix());
		rysujPiramidke();
    modelView.PopMatrix();
    modelView.PushMatrix();
    modelView.Translate(5.0f, 0.0f, 0.0f);
    glUniformMatrix4fv(MVMatrixLocation,1,GL_FALSE,geometryPipeline.GetModelViewProjectionMatrix());
		rysujPiramidke();
    modelView.PopMatrix();
    modelView.PushMatrix();
    modelView.Translate(-5.0f, 0.0f, 0.0f);
    modelView.Scale(2.0f, 2.0f, 2.0f);
    glUniformMatrix4fv(MVMatrixLocation,1,GL_FALSE,geometryPipeline.GetModelViewProjectionMatrix());
		rysujPiramidke();
    modelView.PopMatrix();
    modelView.PopMatrix();

	glutSwapBuffers();
    glutPostRedisplay();
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Pir");
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