//
//  main.cpp
//  视觉demo
//

#include "GLTools.h"
#include "GLMatrixStack.h"
#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLGeometryTransform.h"

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

////设置角色帧，作为相机
GLFrame             viewFrame;
//使用GLFrustum类来设置透视投影
GLFrustum           viewFrustum;
GLTriangleBatch     torusBatch;
GLMatrixStack       modelViewMatix;
GLMatrixStack       projectionMatrix;
GLGeometryTransform transformPipeline;
GLShaderManager     shaderManager;

//标记：背面剔除、深度测试
int iCull = 0;
int iDepth = 0;

//窗口大小改变时接受新的宽度和高度，其中0，0代表窗口中视图的左下角坐标，w，h代表像素
void ChangeSize(int w,int h)
{
    glViewport(0, 0, w, h);
    
    viewFrustum.SetPerspective(35, float(w)/float(h), 1.0, 100);
    
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    transformPipeline.SetMatrixStacks(modelViewMatix, projectionMatrix);
}

//为程序做一次性的设置
void SetupRC()
{
    //设置背景色
    glClearColor(0.5f,0.5f,0.5f,1.0f);
    
    //初始化着色管理器
    shaderManager.InitializeStockShaders();
    
    viewFrame.MoveForward(7.0);
    
    gltMakeTorus(torusBatch, 1.0f, 0.3f, 52, 26);
    
    glPointSize(4.0);
    
}

//开始渲染
void RenderScene(void) {
    //清楚一个或一组特定的缓冲区
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    
    if (iCull) {
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
    } else {
        glDisable(GL_CULL_FACE);
    }
    
    if (iDepth) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
    
    modelViewMatix.PushMatrix(viewFrame);
    
    //设置一组浮点数来表示红色
    GLfloat vRed[] = {1.0f,0.0f,0.0f,1.0f};
    

//     shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vRed);
    
    //使用默认光源着色器
    //通过光源、阴影效果跟提现立体效果
    //参数1：GLT_SHADER_DEFAULT_LIGHT 默认光源着色器
    //参数2：模型视图矩阵
    //参数3：投影矩阵
    //参数4：基本颜色值
    shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, transformPipeline.GetModelViewMatrix(), transformPipeline.GetProjectionMatrix(),vRed);
    
    torusBatch.Draw();
    
    modelViewMatix.PopMatrix();
    
    //将在后台缓冲区进行渲染，然后在结束时交换到前台
    glutSwapBuffers();
}

void SpecialKeys(int key, int x, int y) {
    //1.判断方向
    if(key == GLUT_KEY_UP)
        //2.根据方向调整观察者位置
        viewFrame.RotateWorld(m3dDegToRad(-5.0), 1.0f, 0.0f, 0.0f);
    
    if(key == GLUT_KEY_DOWN)
        viewFrame.RotateWorld(m3dDegToRad(5.0), 1.0f, 0.0f, 0.0f);
    
    if(key == GLUT_KEY_LEFT)
        viewFrame.RotateWorld(m3dDegToRad(-5.0), 0.0f, 1.0f, 0.0f);
    
    if(key == GLUT_KEY_RIGHT)
        viewFrame.RotateWorld(m3dDegToRad(5.0), 0.0f, 1.0f, 0.0f);
    
    //3.重新刷新
    glutPostRedisplay();
}

void ProcessMenu(int value)
{
    switch(value)
    {
        case 1:
            iDepth = !iDepth;
            break;
            
        case 2:
            iCull = !iCull;
            break;
            
        case 3:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
            
        case 4:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
            
        case 5:
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            break;
    }
    
    glutPostRedisplay();
}

int main(int argc,char* argv[]) {

    //设置当前工作目录，针对MAC OS X
    gltSetWorkingDirectory(argv[0]);
    
    //初始化GLUT库
    glutInit(&argc, argv);
    /*初始化双缓冲窗口，其中标志GLUT_DOUBLE、GLUT_RGBA、GLUT_DEPTH、GLUT_STENCIL分别指
     双缓冲窗口、RGBA颜色模式、深度测试、模板缓冲区
     */
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH|GLUT_STENCIL);
    
    //GLUT窗口大小
    glutInitWindowSize(800,600);
    glutCreateWindow("Geometry Test Program");
    
    //注册回调函数
    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);
    glutSpecialFunc(SpecialKeys);
    
    //添加右击菜单栏
    // Create the Menu
    glutCreateMenu(ProcessMenu);
    glutAddMenuEntry("Toggle depth test",1);
    glutAddMenuEntry("Toggle cull backface",2);
    glutAddMenuEntry("Set Fill Mode", 3);
    glutAddMenuEntry("Set Line Mode", 4);
    glutAddMenuEntry("Set Point Mode", 5);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    //驱动程序的初始化中没有出现任何问题
    GLenum err = glewInit();
    if(GLEW_OK != err) {
        fprintf(stderr,"glew error:%s\n",glewGetErrorString(err));
        return 1;
    }

    //调用SetupRC
    SetupRC();
    glutMainLoop();
    return 0;
}
