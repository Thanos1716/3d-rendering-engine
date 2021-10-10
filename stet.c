#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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

    double focal_dist = 1 / (2 * tan(camera.fov / 2)); // 1 is screen width in the simulation, may need change

    printf("focal_dist: %f", focal_dist);
    Vec3 screen_dir = newVec3(focal_dist * cos(camera.yaw), 0, focal_dist * sin(camera.yaw));
    Vec3 screen_pos = addV3(newVec3(camera.x, camera.y, camera.z), screen_dir);

    printf("\nvector screen_dir = (%f, %f, %f)\n", screen_dir.x, screen_dir.y, screen_dir.z);
    printf("\nvector screen_pos = (%f, %f, %f)\n", screen_pos.x, screen_pos.y, screen_pos.z);

    Vec3 b = addV3(screen_pos, normaliseV3(crossV3(newVec3(1, 0, 0), screen_pos)));  // Normalising vector b (slow, look into Quake III fast inverse sqrt)

    Vec3 a = addV3(screen_pos, normaliseV3(crossV3(screen_pos, b)));

    printf("\nvector a = (%f, %f, %f)\n", a.x, a.y, a.z);
    printf("vector b = (%f, %f, %f)\n", b.x, b.y, b.z);

    double mu = ((point.x - camera.x) * (a.y - screen_dir.y) - (point.y - screen_dir.y) * (a.x - screen_dir.x)) / ((b.y - screen_dir.y) * (a.x - screen_dir.x) - (b.x - screen_dir.x) * (a.y - screen_dir.y));
    double lambda = ((point.z - camera.z) - (screen_dir.z - camera.z) - mu * (b.z - screen_dir.z)) / (a.z - screen_dir.z);

    printf("lambda: %f, mu: %f\n", lambda, mu);

    printf("%f\n", a.x-screen_pos.x);

    return newVec2(lambda, mu);
}

int main(int argc, char *argv[]) {

    Camera cam;
    cam.x     = 0.0;
    cam.y     = 0.0;
    cam.z     = 0.0;
    cam.pitch = 0.0;
    cam.yaw   = 0.0000000001;
    cam.roll  = 0.0;
    cam.fov   = 3.141592653589 / 2;

    Vec2 vec = project3D(cam, newVec3(2.0, 1.0, 0.0));

    printf("%f, %f", vec.x, vec.y);

    vec = project3D(cam, newVec3(2.0, 0.0, 1.0));

    printf("%f, %f", vec.x, vec.y);

}
