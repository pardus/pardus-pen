#pragma once

#include <stdio.h>
#include <cmath>
#include <qt6/QtCore/qtypes.h>
#include <qabstractitemmodel.h>
#include <QPointF>
#include <QVector>
#include <QMap>
#include <ranges>

struct EndpointCandidate
{
    int startPoint;

    int endPoint1;
    int endPoint2;

    int startSegmentPoint1;
    int startSegmentPoint2;
};

#define RESAMPLE_POINTS 64
#define MIN_CHANGE_DEGREE 10.0f
#define STRONG_REGION_TURN 60.0f
#define ANGLE_TRESHOLD 45.0f

extern float points[RESAMPLE_POINTS];
extern float Theta[RESAMPLE_POINTS - 1];
extern float DeltaTheta[RESAMPLE_POINTS - 2];
extern int IdealCornerIndex[RESAMPLE_POINTS];

extern float turnRegionSum[62];
extern int turnRegionCount;

extern float idealCornerX[RESAMPLE_POINTS];
extern float idealCornerY[RESAMPLE_POINTS];

extern float cuttedPoint_x[RESAMPLE_POINTS];
extern float cuttedPoint_y[RESAMPLE_POINTS];
extern float cuttedTheta[RESAMPLE_POINTS - 1];
extern float cuttedDeltaTheta[RESAMPLE_POINTS - 2];

extern float distanceBetweenPoints;
extern float point_x[RESAMPLE_POINTS];
extern float point_y[RESAMPLE_POINTS];
extern float lineLength;
extern float pointDistance;

extern int CornerIndex[RESAMPLE_POINTS];
extern int CornerCount;
extern int Fixed_CornerIndex[RESAMPLE_POINTS];
extern int Fixed_CornerCount;
extern int Decision;

extern float circle_sum_x;
extern float circle_sum_y;
extern  float Center_x;
extern float Center_y;

extern float Radius_distance[RESAMPLE_POINTS];
extern float Radius_sum;
extern float Radius_avg;

extern float angle;
extern float edgeLength1;
extern float edgeLength2;
extern float edgeSlope;

void CalculateDeltaTheta(int POINT_LENGTH);

float TotalTurnDegree(int POINT_LENGTH);

float TotalAbsTurnDegree(int POINT_LENGTH);

float TotalPathLength(int POINT_LENGTH);

float StraightnessScore(float totalLength, int POINT_LENGTH);

int DirectionChangeCount(int POINT_LENGTH);

float CalculateShapeFitSquare(int turnRegionCount, int POINT_LENGTH, float totalTurnDegree);

float CalculateShapeFitTriangle(int turnRegionCount, int POINT_LENGTH, float totalTurnDegree);

bool CreateIdealShape(int newturnRegionStart[], int newturnRegionEnd[], int newturnRegionCount, int POINT_LENGTH);

float CalculateShapeFitError(
    int shapeType,
    const float idealCornerX[],
    const float idealCornerY[],
    const int regionStart[],
    const int regionEnd[],
    int POINT_LENGTH);

float FindLength(
    float x1,
    float y1,
    float x2,
    float y2);

int FixPointArray(
    int startPoint,
    int endPoint,
    bool hasIntersection,
    float intersectionX,
    float intersectionY);

float LineScore(float totalTurnDegree, float straightnessScore, int directionChangeCount, int turnRegionCount);

void stroke_recognition(const QMap<long long, QPointF>& points);

void resample(const QMap<long long, QPointF>& points);

void print_out();


#define STANDART_TRESHOLD 0.15f
#define TRIANGLE_TRESHOLD 0.20f
#define SQUARE_TRESHOLD 0.80f
#define CIRCLE_TRESHOLD 1.05f
#define DISTANCETRESHOLD 30

#define RECOG_LINE 1
#define RECOG_CIRCLE 2
#define RECOG_TRIANGLE 3
#define RECOG_SQUARE 10
#define RECOG_UNKNOWN 5
#define RECOG_DISTANCE_ERROR 6
#define RECOG_LENGTH_ERROR 7
#define RECOG_RECTANGLE 67



struct StrokeFeatures
{
    float pathLength = 0.0f;
    float startEndDistance = 0.0f;
    float straightness = 0.0f;

    float totalTurnDegree = 0.0f;
    float totalAbsTurnDegree = 0.0f;
    float meanAbsTurnDegree = 0.0f;
    float maxAbsTurnDegree = 0.0f;

    float turnCancelation = 0.0f;
    float turnConsistency = 0.0f;

    int directionChangeCount = 0;
    int turnRegionCount = 0;

    float straightnessScore = 0.0f;
    float lineScore = 0.0f;
    float totalLength = 0.0f;
    float noise;
};


enum ClosureType
{
    NO_CLOSURE,
    DIRECT_CLOSURE,
    INTERSECTION_CLOSURE,
    PROJECTION_CLOSURE
};

struct ClosureCandidate
{
    ClosureType type;
    float score;
    float pointX;
    float pointY;
    int startCutIndex;
    int endCutIndex;
};


