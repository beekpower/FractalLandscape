#define PI 3.14159265
#include <math.h>

class Point3{
public:
    float x,y,z;
    void set(float dx, float dy, float dz){x = dx; y = dy; z = dz;}
    void set(Point3& p){x = p.x; y = p.y; z = p.z;}
    Point3(float xx,     float yy, float zz){x = xx; y = yy; z = zz;}
    Point3(){x = y = z = 0;}
    void build4tuple(float v[])
    {// load 4-tuple with this color: v[3] = 1 for homogeneous
        v[0] = x; v[1] = y; v[2] = z; v[3] = 1.0f;
    }
};
//@@@@@@@@@@@@@@@@@@ Vector3 class @@@@@@@@@@@@@@@@
class Vector3{
public:
    float x,y,z;
    void set(float dx, float dy, float dz){ x = dx; y = dy; z = dz;}
    void set(Vector3& v){ x = v.x; y = v.y; z = v.z;}
    void flip(){x = -x; y = -y; z = -z;} // reverse this vector
    void setDiff(Point3& a, Point3& b)//set to difference a - b
    { x = a.x - b.x; y = a.y - b.y; z = a.z - b.z;}
    void normalize()//adjust this vector to unit length
    {
        double sizeSq = x * x + y * y + z * z;
        if(sizeSq < 0.0000001)
        {
            //cerr << "\nnormalize() sees vector (0,0,0)!";
            return; // does nothing to zero vectors;
        }
        float scaleFactor = 1.0/(float)sqrt(sizeSq);
        x *= scaleFactor; y *= scaleFactor; z *= scaleFactor;
    }
    Vector3(float xx, float yy, float zz){x = xx; y = yy; z = zz;}
    Vector3(Vector3& v){x = v.x; y = v.y; z = v.z;}
    Vector3(){x = y = z = 0;} //default constructor
    Vector3 cross(Vector3 b) //return this cross b
    {
        Vector3 c(y*b.z - z*b.y, z*b.x - x*b.z, x*b.y - y*b.x);
        return c;
    }
    float dot(Vector3 b) // return this dotted with b
    {return x * b.x + y * b.y + z * b.z;}
};

class Camera
{
private:
    Point3 eye;
    Vector3 u,v,n;
    double viewAngle, aspect, nearDist, farDist; // view volume shape
    void setModelViewMatrix(); // tell OpenGL where the camera is
    
public:
    Camera(void){}; // default constructor
    void set(Point3 eye, Point3 look, Vector3 up); // like gluLookAt()
    void roll(float angle); // roll it
    void pitch(float angle); // increase pitch
    void yaw(float angle); // yaw it
    void slide(float delU, float delV, float delN); // slide it
    void rotate (Vector3 axis, float angle);
    void setShape(float vAng, float asp, float nearD, float farD);
};

void Camera :: setShape(float vAngle, float asp, float nr, float fr)
{
    viewAngle = vAngle;
    aspect = asp;
    nearDist = nr;
    farDist = fr;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(viewAngle, aspect, nearDist, farDist);
    glMatrixMode(GL_MODELVIEW);
}

void Camera :: setModelViewMatrix(void)
{ // load modelview matrix with existing camera values
    float m[16];
    Vector3 eVec(eye.x, eye.y, eye.z); // a vector version of eye
    m[0] =  u.x; m[4] =  u.y; m[8]  =  u.z;  m[12] = -eVec.dot(u);
    m[1] =  v.x; m[5] =  v.y; m[9]  =  v.z;  m[13] = -eVec.dot(v);
    m[2] =  n.x; m[6] =  n.y; m[10] =  n.z;  m[14] = -eVec.dot(n);
    m[3] =  0;   m[7] =  0;   m[11] =  0;    m[15] = 1.0;
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(m); // load OpenGL’s modelview matrix
}

void Camera:: set(Point3 Eye, Point3 look, Vector3 up)
{	// create a modelview matrix and send it to OpenGL
    eye.set(Eye); // store the given eye position
    n.set(eye.x - look.x, eye.y - look.y, eye.z - look.z); // make n
    u.set(up.cross(n).x, up.cross(n).y, up.cross(n).z); // make u = up X n
    n.normalize(); u.normalize(); // make them unit length
    v.set(n.cross(u).x, n.cross(u).y, n.cross(u).z);  // make v =  n X u
    setModelViewMatrix(); // tell OpenGL
}

void Camera:: slide(float delU, float delV, float delN)
{
    eye.x += delU * u.x + delV * v.x + delN * n.x;
    eye.y += delU * u.y + delV * v.y + delN * n.y;
    eye.z += delU * u.z + delV * v.z + delN * n.z;
    setModelViewMatrix();
}

void Camera:: roll(float angle)
{
    float cs=cos(PI/180.0 * angle);
    float sn=sin(PI/180.0 * angle);
    Vector3 t = u;
    u.set(cs*t.x - sn*v.x, cs*t.y - sn*v.y, cs*t.z - sn*v.z);
    v.set(sn*t.x + cs*v.x, sn*t.y + cs*v.y, sn*t.z + cs*v.z);
    setModelViewMatrix();
}

void Camera :: pitch (float angle)
{ // pitch the camera through angle degrees around U
    float cs = cos(3.14159265/180 * angle);
    float sn = sin(3.14159265/180 * angle);
    Vector3 t(v); // remember old v
    v.set(cs*t.x - sn*n.x, cs*t.y - sn*n.y, cs*t.z - sn*n.z);
    n.set(sn*t.x + cs*n.x, sn*t.y + cs*n.y, sn*t.z + cs*n.z);
    setModelViewMatrix();
}

void Camera :: yaw (float angle)
{ // yaw the camera through angle degrees around V
    float cs = cos(3.14159265/180 * angle);
    float sn = sin(3.14159265/180 * angle);
    Vector3 t(n); // remember old v
    n.set(cs*t.x - sn*u.x, cs*t.y - sn*u.y, cs*t.z - sn*u.z);
    u.set(sn*t.x + cs*u.x, sn*t.y + cs*u.y, sn*t.z + cs*u.z);
    setModelViewMatrix();
}

void Camera :: rotate (Vector3 axis, float angle)
{ // rotate the camera through angle degrees around axis
    float cs = cos(3.14159265/180 * angle);
    float sn = sin(3.14159265/180 * angle);
    Vector3 oldn(n); // remember old n
    Vector3 oldu(u); // remember old u
    n.x=((cs+(1-cs)*u.x*u.x)*oldn.x+(1-cs)*u.y*u.x -sn*u.z)
    *oldu.x+((1-cs)*u.z*u.x+sn*u.y)*v.x;
    n.y=((cs+(1-cs)*u.x*u.x)*oldn.y+(1-cs)*u.y*u.x -sn*u.z)
    *oldu.y + ((1-cs)*u.z*u.x+sn*u.y)*v.y;
    n.z=((cs +(1-cs)*u.x*u.x)*oldn.z +(1-cs)*u.y*u.x -sn*u.z)
    *oldu.z+((1-cs)*u.z*u.x+sn*u.y)*v.z;
    u.x=((1-cs)*u.y*u.x +sn*u.z)*oldn.x +(cs +(1-cs)*u.y*u.y)*
    oldu.x+((1-cs)*u.z*u.x - sn*u.y)*v.x;
    u.y=((1-cs)*u.y*u.x +sn*u.z)*oldn.y +(cs +(1-cs)*u.y*u.y)*
    oldu.y+((1-cs)*u.z*u.x - sn*u.y)*v.y;
    u.z=((1-cs)*u.y*u.x +sn*u.z)*oldn.z +(cs +(1-cs)*u.y*u.y)*
    oldu.z +((1-cs)*u.z*u.x - sn*u.y)*v.z;
    v.x=((1-cs)*u.x*u.z -sn*u.y)*oldn.x +((1-cs)*u.y*u.z+sn*u.y)
    *oldu.x +(cs +(1-cs)*u.z*u.z)*v.x;
    v.y=((1-cs)*u.x*u.z -sn*u.y)*oldn.y +((1-cs)*u.y*u.z+sn*u.y)
    *oldu.y +(cs +(1-cs)*u.z*u.z)*v.y;
    v.z=((1-cs)*u.x*u.z -sn*u.y)*oldn.z +((1-cs)*u.y*u.z+sn*u.y)
    *oldu.z +(cs +(1-cs)*u.z*u.z)*v.z;
    
    setModelViewMatrix();
}



