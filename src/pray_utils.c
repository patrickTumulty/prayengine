
#include "pray_utils.h"
#include "common_utils.h"
#include <math.h>

Color prayColorDesaturate(Color color, float desaturate)
{
    float r = color.r;
    float g = color.g;
    float b = color.b;
    float gray = (r + g + b) / 3.0f;
    return (Color) {
        .r = (u8) ((r * (1 - desaturate)) + (gray * desaturate)),
        .g = (u8) ((g * (1 - desaturate)) + (gray * desaturate)),
        .b = (u8) ((b * (1 - desaturate)) + (gray * desaturate)),
        .a = color.a,
    };
}

Vector2 prayVector2MoveTowards(Vector2 current, Vector2 target, float maxDistanceDelta)
{
    float deltaX = target.x - current.x;
    float deltaY = target.y - current.y;

    float sqDist = (deltaX * deltaX) + (deltaY * deltaY);

    if (feq(sqDist, 0.0f) || (maxDistanceDelta >= 0 && sqDist <= maxDistanceDelta * maxDistanceDelta))
    {
        return target;
    }

    float dist = sqrtf(sqDist);
    return (Vector2) {
        .x = current.x + (deltaX / dist * maxDistanceDelta),
        .y = current.y + (deltaY / dist * maxDistanceDelta),
    };
}

Vector2 prayVector2Add(Vector2 v1, Vector2 v2)
{
    v1.x += v2.x;
    v1.y += v2.y;
    return v1;
}

bool prayVector2Equals(Vector2 v1, Vector2 v2)
{
    return feq(v1.x, v2.x) && feq(v1.y, v2.y);
}

/**
 * @brief Calculate distance between 2 vector2s 
 *
 * @param[[in]] p1 point 1 
 * @param[[in]] p2 point 2 
 *
 * @return distance
 */
float prayVector2Distance(Vector2 p1, Vector2 p2)
{
    float deltaX = p2.x - p1.x;
    float deltaY = p2.y - p1.y;
    return sqrtf((deltaX * deltaX) + (deltaY * deltaY));
}

/**
 * @brief calculate points for an equal lateral triangle 
 *
 * @param[[in]] origin center point 
 * @param[[in]] roatationDegrees triangle rotation around origin 
 * @param[[in]] radius radius 
 */
void prayCalcTriangle(Vector2 origin, float roatationDegrees, float radius, Vector2 points[3])
{
    points[0] = prayCalcPointOnCircle(origin, DEG2RAD * roatationDegrees, radius);
    points[1] = prayCalcPointOnCircle(origin, DEG2RAD * (120 + roatationDegrees), radius);
    points[2] = prayCalcPointOnCircle(origin, DEG2RAD * (240 + roatationDegrees), radius);
}

/**
 * @brief Calculate angle between 2 points 
 *
 * @param[[in]] p1 point 1 
 * @param[[in]] p2 point 2 
 *
 * @return angle in degrees
 */
float prayCalcAngle(Vector2 p1, Vector2 p2)
{
    float deltaX = p1.x - p2.x;
    float deltaY = p1.y - p2.y;
    return RAD2DEG * atan2f(deltaY, deltaX);
}

/**
 * @brief calculate a point on a circle around a point 
 *
 * @param[[in]] origin origin point  
 * @param[[in]] radians degrees in radians 
 * @param[[in]] radius radius 
 *
 * @return point on circle 
 */
Vector2 prayCalcPointOnCircle(Vector2 origin, float radians, float radius)
{
    float x = radius * cosf(radians);
    float y = radius * sinf(radians);
    return (Vector2) {
        .x = x + origin.x,
        .y = y + origin.y,
    };
}

float prayCalcSlope(Vector2 p1, Vector2 p2)
{
    float deltaX = p2.x - p1.x;
    float deltaY = p2.y - p1.y;
    return deltaX == 0 ? 1.0f : deltaY / deltaX;
}

void prayDrawPolygon(const Vector2 *points, int pointsCount, Vector2 offset, Color color)
{
    Vector2 prev = points[0];
    prev.x += offset.x;
    prev.y += offset.y;
    for (int i = 0; i < pointsCount; i++)
    {
        Vector2 p = points[i];
        p.x += offset.x;
        p.y += offset.y;
        DrawLineEx(p, prev, 1, color);
        prev = p;
        DrawCircleV(p, 2, color);
    }
    Vector2 p = points[0];
    p.x += offset.x;
    p.y += offset.y;
    DrawLineEx(prev, p, 1, color);
}
