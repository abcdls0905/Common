
#pragma once

struct Vector2
{
    float x;
    float y;
    Vector2()
    {
        this->x = 0;
        this->y = 0;
    }
    Vector2(float x, float y)
    {
        this->x = x;
        this->y = y;
    }
};

struct Vector3
{
    float x;
    float y;
    float z;
    Vector3()
    {
        this->x = 0;
        this->y = 0;
        this->z = 0;
    }
    Vector3(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }
};

struct Matrix
{
    float m[16];
};