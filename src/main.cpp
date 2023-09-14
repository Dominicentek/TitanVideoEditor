#include <iostream>
#include <GL/glut.h>

#define WINDOW_WIDTH 1270
#define WINDOW_HEIGHT 768

void update() {
    
}
void render() {
    update();
    glClear(GL_COLOR_BUFFER_BIT);
    
    glFlush();
}
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(GLUT_SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2, GLUT_SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2);
    glutCreateWindow("Titan");
    glutDisplayFunc(render);
    glutMainLoop();
    return 0;
}