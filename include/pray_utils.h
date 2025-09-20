
#ifndef PRAY_UTILS_H
#define PRAY_UTILS_H

#include "raylib.h"

#define VEC2(X, Y) ((Vector2) {(X), (Y)})
#define VEC2_ZERO (Vector2){0, 0}
#define VEC2_EQ(V1, V2) (((V1).x == (V2).x) && ((V1).y == (V2).y))
#define VEC2_ADD(V1, V2) ((Vector2) {(V1).x + (V2).x, (V1).y + (V2).y})
#define VEC2_ADD_VALUE(V1, V) ((Vector2) {(V1).x + (V), (V1).y + (V)})

// Delete this in th future
typedef struct
{
    int x;
    int y;
} IntVector2;

#define IVEC2(X, Y) ((IntVector2) {.x = (X), .y = (Y)})

Color prayColorDesaturate(Color color, float desaturate);
Vector2 prayVector2MoveTowards(Vector2 current, Vector2 target, float maxDistanceDelta);
Vector2 prayVector2Add(Vector2 v1, Vector2 v2);
float prayVector2CalcSlope(Vector2 v1, Vector2 v2);
float prayVector2Distance(Vector2 p1, Vector2 p2);
bool prayVector2Equals(Vector2 v1, Vector2 v2);
void prayCalcTriangle(Vector2 origin, float roatationDegrees, float radius, Vector2 points[3]);
float prayCalcAngle(Vector2 p1, Vector2 p2);
Vector2 prayCalcPointOnCircle(Vector2 origin, float radians, float radius);
float prayCalcSlope(Vector2 p1, Vector2 p2);
void prayDrawPolygon(const Vector2 *points, int pointsCount, Vector2 offset, Color color);

#endif // PRAY_UTILS_H
