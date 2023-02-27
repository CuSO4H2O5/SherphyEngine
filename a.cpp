class Light{
    Color color;

    void emit(out Ray ray);
};

class Ray{
    Vector3 point;
    Vector3 dir;
    Color color;
};

class Color{
    union 
    {
        char clr[3];
        char r, g, b;
    };
};

class Scene{

};

class Object{

};