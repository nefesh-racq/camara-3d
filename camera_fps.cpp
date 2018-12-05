/*
* programa que hace el manejo de una camara para moverse por el mundo 3d
* el programa no detecta colisiones
* para rotar la camara haga click en el mundo 3d
* para moverse use las teclas (a w d s )
* esc para terminar el programa
*
*
* a = izquierda
* w = adelanate
* d = derecha
* s = atras
*
* compilar usando make y ejecutar ./a.out
*
* o $ g++ -lGL -lGLU -lSDL2 camera_fps.cpp
*
*/
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <cstdio>
#include <cmath>
#include <iostream>

int mape;
SDL_Window* window;
float camX = 0.0, camY = 0.0, camZ = 0.0;
float camYaw = 0.0;
float camPitch = 0.0;
bool mousein = false;


struct Coordinate {
    float x, y, z;
    Coordinate(float a, float b, float c) : x(a), y(b), z(c) {};
};

struct Face {
    int facenum;
    bool four;
    int faces[4];
    Face(int facen, int f1, int f2, int f3) : facenum(facen) {
        faces[0] = f1;
        faces[1] = f2;
        faces[2] = f3;
        four = false;
    }
    Face(int facen, int f1, int f2, int f3, int f4) : facenum(facen) {
        faces[0] = f1;
        faces[1] = f2;
        faces[2] = f3;
        faces[3] = f4;
        four = true;
    }
};

int loadObject(const char *filename) {
    std::vector<std::string*> coord;
    std::vector<Coordinate*> vertex;
    std::vector<Face*> faces;
    std::vector<Coordinate*> normals;
    std::ifstream in(filename);

    if (!in.is_open()) {
        std::cout << "no puede abrirse" << std::endl;
        return -1;
    }

    char buf[256];

    while (!in.eof()) {
        in.getline(buf, 256);
        coord.push_back(new std::string(buf));
    }

    for (int i = 0; i < coord.size(); i++) {
        if ((*coord[i])[0] == '#')
            continue;
        else if ((*coord[i])[0] == 'v' && (*coord[i])[1] == ' ') {
            char tmp;
            float tmpx, tmpy, tmpz;
            sscanf(coord[i]->c_str(), "%c %f %f %f", &tmp, &tmpx, &tmpy, &tmpz);
            vertex.push_back(new Coordinate(tmpx, tmpy, tmpz));
        }
        else if ((*coord[i])[0] == 'v' && (*coord[i])[1] == 'n') {
            float tmpx, tmpy, tmpz;
            sscanf(coord[i]->c_str(), "vn %f %f %f", &tmpx, &tmpy, &tmpz);
            normals.push_back(new Coordinate(tmpx, tmpy, tmpz));
        }
        else if ((*coord[i])[0] == 'f') {
            int a, b, c, d, e;
            if (count(coord[i]->begin(), coord[i]->end(), ' ') == 4) {
                sscanf(coord[i]->c_str(), "f %d//%d %d//%d %d//%d %d//%d",
                       &a, &b, &c, &b, &d, &b, &e, &b);
                faces.push_back(new Face(b, a, c, d, e));
            }
            else {
                sscanf(coord[i]->c_str(), "f %d//%d %d//%d %d//%d",
                       &a, &b, &c, &b, &d, &b);
                faces.push_back(new Face(b, a, c, d));
            }
        }
    }

    // draw
    int num;

    num = glGenLists(1);
    glNewList(num, GL_COMPILE);

    for (int i = 0; i < faces.size(); i++) {
        if (faces[i]->four) {
            glBegin(GL_QUADS);
                glNormal3f(normals[faces[i]->facenum-1]->x,
                           normals[faces[i]->facenum-1]->y,
                           normals[faces[i]->facenum-1]->z
                           );
                glVertex3f(vertex[faces[i]->faces[0]-1]->x,
                           vertex[faces[i]->faces[0]-1]->y,
                           vertex[faces[i]->faces[0]-1]->z
                           );
                glVertex3f(vertex[faces[i]->faces[1]-1]->x,
                           vertex[faces[i]->faces[1]-1]->y,
                           vertex[faces[i]->faces[1]-1]->z
                           );
                glVertex3f(vertex[faces[i]->faces[2]-1]->x,
                           vertex[faces[i]->faces[2]-1]->y,
                           vertex[faces[i]->faces[2]-1]->z
                           );
                glVertex3f(vertex[faces[i]->faces[3]-1]->x,
                           vertex[faces[i]->faces[3]-1]->y,
                           vertex[faces[i]->faces[3]-1]->z
                           );
            glEnd();
        }
        else {
            glBegin(GL_TRIANGLES);
                glNormal3f(normals[faces[i]->facenum-1]->x,
                           normals[faces[i]->facenum-1]->y,
                           normals[faces[i]->facenum-1]->z
                           );
                glVertex3f(vertex[faces[i]->faces[0]-1]->x,
                           vertex[faces[i]->faces[0]-1]->y,
                           vertex[faces[i]->faces[0]-1]->z
                           );
                glVertex3f(vertex[faces[i]->faces[1]-1]->x,
                           vertex[faces[i]->faces[1]-1]->y,
                           vertex[faces[i]->faces[1]-1]->z
                           );
                glVertex3f(vertex[faces[i]->faces[2]-1]->x,
                           vertex[faces[i]->faces[2]-1]->y,
                           vertex[faces[i]->faces[2]-1]->z
                           );
            glEnd();
        }
    }
    glEndList();

    for (int i = 0; i < coord.size(); i++)
        delete coord[i];
    for (int i = 0; i < faces.size(); i++)
        delete faces[i];
    for (int i = 0; i < normals.size(); i++)
        delete normals[i];
    for (int i = 0; i < vertex.size(); i++)
        delete vertex[i];

    return num;
}

void lockCamera() {
    if(camPitch > 90)
        camPitch = 90;
    if(camPitch < -90)
        camPitch = -90;
    if(camYaw < 0.0)
        camYaw += 360.0;
    if(camYaw > 360.0)
        camYaw -= 360.0;
}

void moveCamera(float dist, float dir) {
    float rad = (camYaw + dir) * M_PI / 180.0;

    camX -= sin(rad) * dist;
    camZ -= cos(rad) * dist;
}

void moveCameraUp(float dist, float dir) {
    float rad = (camPitch + dir) * M_PI / 180.0;

    camY += sin(rad) * dist;
}

void control(float movevel, float mousevel, bool mi) {
    if (mi) {
        int MidX = 320;
        int MidY = 240;
        SDL_ShowCursor(SDL_DISABLE);

        int tmpx, tmpy;
        SDL_GetMouseState(&tmpx, &tmpy);
        camYaw += mousevel * (MidX - tmpx);
        camPitch += mousevel * (MidY - tmpy);

        lockCamera();
        SDL_WarpMouseInWindow(window, MidX, MidY);
        SDL_PumpEvents();
        const Uint8* state = SDL_GetKeyboardState(NULL);

        if (state[SDLK_w] == 1) {
            //if (camPitch != 90 && camPitch != -90)
                moveCamera(movevel, 0.0);

            moveCameraUp(movevel, 0.0);
        }
        else if (state[SDLK_s] == 1) {
            //if (camPitch != 90 && camPitch != -90)
                moveCamera(movevel, 180.0);

            moveCameraUp(movevel, 180.0);
        }
        if (state[SDLK_a] == 1)
            moveCamera(movevel, 90.0);
        else if (state[SDLK_d] == 1)
            moveCamera(movevel, 270.0);
    }

    //std::cout << "Mouse movido.. " << camPitch << std::endl;
    glRotatef(-camPitch, 1.0, 0.0, 0.0);
    glRotatef(-camYaw, 0.0, 1.0, 0.0);
}

void updateCamera() {
    glTranslatef(-camX, -camY, -camZ);
}

/*

   aqui se carga el mapa


*/
void init() {
    glClearColor(0.5, 0.5, 0.5, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 640.0 / 480.0, 1.0, 400.0);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
/*
    glEnable(GL_CCW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glEnable(GL_COLOR_MATERIAL);
*/
    //mape = loadObject("varios1.obj");
    //mape = loadObject("sherry.obj");
    //mape = loadObject("leon.obj");
    mape = loadObject("Escenario.obj");
}

float a = 0.1;

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    control(0.2, 0.2, mousein);
    updateCamera();

    /*
      rotamos la camara en el eje y
      y trasladamos la camara en las coordenadas x, y, z
    */
    glRotatef(-35, 0.0, 1.0, 0.0);
    glTranslatef(-3.0, -2.0, -8.0);

    glPushMatrix();
        //glRotatef(a, 0.0, 1.0, 0.0);FLISOL021.obj
        a+= 0.005;
        glCallList(mape);
    glPopMatrix();
}

int main(int argc, char** argv) {
    //SDL_Window*
    window = SDL_CreateWindow("move camera fps", 20, 20, 640, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    bool running = true;

    SDL_GL_CreateContext(window);
    init();

    while (running) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: running = false; break;
                case SDL_MOUSEBUTTONDOWN:
                    mousein = true;
                    SDL_ShowCursor(SDL_DISABLE);
                    break;
                case SDL_KEYDOWN:
                case SDL_MOUSEMOTION:
                    if (event.key.keysym.sym == SDLK_p) {
                        mousein = false;
                        SDL_ShowCursor(SDL_ENABLE);
                        break;
                    }
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        running = false;
                        break;
                    }

                    if (event.key.keysym.sym == SDLK_w)
                        moveCamera(0.2, 0.0);
                    if (event.key.keysym.sym == SDLK_s)
                        moveCamera(0.2, 180);
                    if (event.key.keysym.sym == SDLK_a)
                        moveCamera(0.2, 90);
                    if (event.key.keysym.sym == SDLK_d)
                        moveCamera(0.2, 270);
                    break;
            }
        }

        display();
        SDL_GL_SwapWindow(window);
        SDL_Delay(100);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
