#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include "window.c"
#define NENDS 6           /* number of end "points" to draw */

GLFWwindow* window;           /* window desciptor/handle */
GLint width = 640;
GLint height = 480;         /* initial window width and height */
int fbwidth, fbheight;    /* framebuffer width and height (Retina display) */

GLfloat ends[NENDS][3];   /* array of 2D points */

typedef struct Camera {
    double x;
    double y;
    double z;
    double pitch;
    double yaw;
    double roll;
    double fov;
} Camera;

typedef struct Vec2 {
    double x;
    double y;
} Vec2;

typedef struct Vec3 {
    double x;
    double y;
    double z;
} Vec3;

typedef struct Mat2x2 {
    double ix;
    double iy;
    double jx;
    double jy;
} Mat2x2;


Camera cam;

/* function where all the actual drawing happens */
// void disp() {
//     int i;

//     /* color buffer must be cleared each time */
//     glClear(GL_COLOR_BUFFER_BIT);

//     /* draw the line using the preset color and line thickness  */
//     glBegin(GL_LINES);
//     for (i = 0; i < NENDS; ++i) {
//         glVertex2fv(ends[i]);
//     }
//     glEnd();

//     /* GLFW is ALWAYS double buffered; will call glFlush() */
//     glfwSwapBuffers(window);

//     return;
// }

/* Callback functions for GLFW */

// void init_data() {
    /* initialize line's end points to be fractions of window dimensions */
    // ends[0][0] = (float) 0.25*fbwidth;  /* (0,0) is the lower left corner */
    // ends[0][1] = (float) 0.75*fbheight;
    // ends[1][0] = (float) 0.75*fbwidth;
    // ends[1][1] = (float) 0.25*fbheight;

    // return;
// }

/* Called when framebuffer is resized, e.g., when window is resized
 * OR when the same size buffer is moved across Retina and non-Retina displays
 * when running Mac OS X.
 * NOT called automatically when window is first created.
 * Called by GLFW BEFORE reshape().
*/
void fbreshape(GLFWwindow *window, int w, int h) {
    /* save new framebuffer dimensions */
    fbwidth = w;
    fbheight = h;

    /* do an orthographic parallel projection with the view volume
       set to first quadrant, fixed to the initial window dimension */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (float)fbwidth, 0.0, (float)fbheight, -1.f, 1.f);

    /* Tell OpenGL to use the whole window for drawing.
       Note that we don't resize the view volume, so
       the viewport will show the whole view volume
       shrunk/stretched to fit the current view port. */
    glViewport(0, 0, (GLsizei) fbwidth, (GLsizei) fbheight);

    // init_data();
    // disp();

    return;
}

/* Called when window is resized.
 * NOT called automatically when window is first created.
 * Called by GLFW AFTER fbreshape().
*/
void reshape(GLFWwindow *window, int w, int h) {
    /* save new screen dimensions */
    width = w;
    height = h;

    return;
}

void kbd(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_RELEASE) { // function is called first on GLFW_PRESS.
        return;
    }

    switch (key) {
    case GLFW_KEY_ESCAPE:
        quit(window);
        break;
    case GLFW_KEY_LEFT:
        cam.yaw -= 0.1;
        break;
    case GLFW_KEY_RIGHT:
        cam.yaw += 0.1;
        // disp();
        break;
    default:
        break;
    }

    return;
}

void charhd(GLFWwindow* window, unsigned int key) {
    switch (key) {
    case 'q':
        quit(window);
        break;
    default:
        break;
    }

    return;
}

void err(int errcode, const char* desc) {
    fprintf(stderr, "%d: %s\n", errcode, desc);

    return;
}

Vec3 newVec3(double x, double y, double z) {
    Vec3 vec;
    vec.x = x;
    vec.y = y;
    vec.z = z;
    return vec;
}

Vec2 newVec2(double x, double y) {
    Vec2 vec;
    vec.x = x;
    vec.y = y;
    return vec;
}

Mat2x2 newMat2x2(double ix, double iy, double jx, double jy) {
    Mat2x2 mat;
    mat.ix = ix;
    mat.iy = iy;
    mat.jx = jx;
    mat.jy = jy;
    return mat;
}

double detM2x2(Mat2x2 u) {
    return u.ix * u.jy - u.iy * u.jx;
}

Vec3 crossV3(Vec3 u, Vec3 v) {
    //https://www.analyzemath.com/stepbystep_mathworksheets/vectors/cross_product.html
    return newVec3(detM2x2(newMat2x2(u.y, v.y, u.z, v.z)), detM2x2(newMat2x2(u.x, v.x, u.z, v.z)), detM2x2(newMat2x2(u.x, v.x, u.y, v.y)));
}

double absV3(Vec3 u) {
    return sqrt(pow(u.x, 2) + pow(u.y, 2) + pow(u.z, 2));
}

double dotV3(Vec3 u, Vec3 v) {
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

void initgl() {
    /* clear color buffer to white */
    glClearColor(0.0, 0.0, 0.0, 0.0);

    /* set the line color to black and thickness to 3 pixels */
    glColor3f(1.0, 0.0, 0.0);
    glLineWidth(2.0);

    /* no transformation */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    return;
}

Vec3 subV3(Vec3 u, Vec3 v) {
    return newVec3(u.x - v.x, u.y - v.y, u.z - v.z);
}

Vec3 addV3(Vec3 u, Vec3 v) {
    return newVec3(u.x + v.x, u.y + v.y, u.z + v.z);
}

Vec3 normaliseV3(Vec3 u) {
    return newVec3(u.x / absV3(u), u.y / absV3(u), u.z / absV3(u));
}

Vec2 project3D(Camera camera, Vec3 point) {

    double focal_dist = 10 / (2 * tan(camera.fov / 2)); // 1 is screen width in the simulation, may need change

    printf("focal_dist: %f", focal_dist);
    Vec3 screen_dir = newVec3(focal_dist * cos(camera.yaw), 0, focal_dist * sin(camera.yaw));
    Vec3 screen_pos = addV3(newVec3(camera.x, camera.y, camera.z), screen_dir);

    printf("\nvector screen_dir = (%f, %f, %f)\n", screen_dir.x, screen_dir.y, screen_dir.z);
    printf("\nvector screen_pos = (%f, %f, %f)\n", screen_pos.x, screen_pos.y, screen_pos.z);

    Vec3 a = newVec3((-sqrt(pow((screen_dir.z - camera.z), 2) * (pow(camera.z, 2) - 2 * screen_dir.z * camera.z + pow(camera.x, 2) - 2 * screen_dir.x * camera.x + pow(screen_dir.x, 2) + pow(screen_dir.z, 2))) + screen_dir.x * pow(camera.z, 2) - 2 * screen_dir.z * screen_dir.x * camera.z + screen_dir.x * pow(camera.x, 2) - 2 * pow(screen_dir.x, 2) * camera.x + pow(screen_dir.x, 3) + pow(screen_dir.z, 2) * screen_dir.x) / (pow(camera.z, 2) - 2 * screen_dir.z * camera.z + pow(camera.x, 2) - 2 * screen_dir.x * camera.x + pow(screen_dir.x, 2) + pow(screen_dir.z, 2)),
                     0,
                     (-screen_dir.z * pow(camera.z, 3) - camera.x * sqrt(pow((screen_dir.z - camera.z), 2) * (pow(camera.z, 2) - 2 * screen_dir.z * camera.z + pow(camera.x, 2) - 2 * screen_dir.x * camera.x + pow(screen_dir.x, 2) + pow(screen_dir.z, 2))) + screen_dir.x * sqrt(pow((screen_dir.z - camera.z), 2) * (pow(camera.z, 2) - 2 * screen_dir.z * camera.z + pow(camera.x, 2) - 2 * screen_dir.x * camera.x + pow(screen_dir.x, 2) + pow(screen_dir.z, 2))) + 3 * pow(screen_dir.z, 2) * pow(camera.z, 2) - screen_dir.z * camera.z * pow(camera.x, 2) + 2 * screen_dir.z * screen_dir.x * camera.z * camera.x - screen_dir.z * pow(screen_dir.x, 2) * camera.z - 3 * pow(screen_dir.z, 3) * camera.z + pow(screen_dir.z, 2) * pow(camera.x, 2) - 2 * pow(screen_dir.z, 2) * screen_dir.x * camera.x + pow(screen_dir.z, 2) * pow(screen_dir.x, 2) + pow(screen_dir.z, 4)) / ((screen_dir.z - camera.z) * (pow(camera.z, 2) - 2 * screen_dir.z * camera.z + pow(camera.x, 2) - 2 * screen_dir.x * camera.x + pow(screen_dir.x, 2) + pow(screen_dir.z, 2))));

    printf("\nvector a = (%f, %f, %f)\n", a.x, a.y, a.z);

    Vec3 b = addV3(screen_pos, normaliseV3(crossV3(screen_pos, a)));  // Normalising vector b (slow, look into Quake III fast inverse sqrt)

    printf("vector b = (%f, %f, %f)\n", b.x, b.y, b.z);

    double mu = ((point.x - camera.x) * (a.y - screen_dir.y) - (point.y - screen_dir.y) * (a.x - screen_dir.x)) / ((b.y - screen_dir.y) * (a.x - screen_dir.x) - (b.x - screen_dir.x) * (a.y - screen_dir.y));
    double lambda = ((point.x - camera.x) - (screen_dir.x - camera.x) - mu * (b.x - screen_dir.x)) / (a.x - screen_dir.x);

    printf("%f, %f\n", lambda, mu);


    return newVec2(lambda * fbwidth + (fbwidth / 2), mu * fbheight + (fbheight / 2));
}


int main(int argc, char *argv[]) {
    glfwSetErrorCallback(err);
    if (!glfwInit()) {
        exit(1);
    }

    /* create the window and its associated OpenGL context */
    window = glfwCreateWindow(width, height, "3D Engine", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(1);
    }
    /* make the window's context the current context */
    glfwMakeContextCurrent(window);

    /* With Retina display on Mac OS X, GLFW's FramebufferSize
         is different from WindowSize */
    glfwGetFramebufferSize(window, &fbwidth, &fbheight);

    /* shape view port */
    fbreshape(window, fbwidth, fbheight);

    /* --- register callbacks with GLFW --- */

    /* register function to handle window resizes */
    /* With Retina display on Mac OS X GLFW's FramebufferSize
     is different from WindowSize */
    glfwSetFramebufferSizeCallback(window, fbreshape);
    glfwSetWindowSizeCallback(window, reshape);

    /* register function to handle window close */
    glfwSetWindowCloseCallback(window, quit);

    /* register function to handle keyboard input */
    glfwSetKeyCallback(window, kbd);      // general keyboard input
    glfwSetCharCallback(window, charhd);  // simpler specific character handling

    /* --- draw line --- */

    // initgl();
    // disp();

    // Camera cam;
    cam.x     = 0.0;
    cam.y     = 0.0;
    cam.z     = 0.0;
    cam.pitch = 0.0;
    cam.yaw   = 0.00000000001;
    cam.roll  = 0.0;
    cam.fov   = 3.14159265358979/2;

    const int point_count = 4;
    double points[NENDS][3] = {
        {3.0, 0.0, -1.0},
        {3.0, 2.0, -1.0},
        {3.0, 1.0, -1.0},
        {3.0, 1.0, 0.0},
        {3.0, 0.0, 0.0},
        {3.0, 2.0, 0.0}
    };


    // {
    //     {2.0, 0.0, 0.0},
    //     {0.0, 0.0, 0.0},
    //     {4.0, 3.0, 1.0},
    //     {3.0, 4.0, 2.0}
    // };

    initgl();
    glColor3f(1.0, 1.0, 1.0);

    while (!glfwWindowShouldClose(window)) {

        glClear(GL_COLOR_BUFFER_BIT);


        /* draw the line using the preset color and line thickness  */
        glBegin(GL_LINES);

        for (int i = 0; i < NENDS; ++i) {
            Vec2 vec = project3D(cam, newVec3(points[i][0], points[i][1], points[i][2]));
            ends[i][0] = vec.x;  // * fbwidth;
            ends[i][1] = vec.y;  // * fbheight;
        }
        printf("yaw: %f", cam.yaw);

        // for (int i = 0; i < 3; ++i) {
        //     printf("%f, %f\n", ends[i][0], ends[i][1]);
        // }
        // printf("\n");

        glColor3f(1.0, 1.0, 1.0);
        for (int i = 0; i < NENDS; ++i) {
            glVertex2fv(ends[i]);
        }
        glEnd();

        /* GLFW is ALWAYS double buffered; will call glFlush() */
        glfwSwapBuffers(window);


        glfwWaitEvents();
    }

    exit(0);
}
