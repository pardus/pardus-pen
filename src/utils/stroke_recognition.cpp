#include "utils/stroke_recognition.h"
#include "widgets/DrawingWidget.h"

float points[RESAMPLE_POINTS];
float Theta[RESAMPLE_POINTS - 1];
float DeltaTheta[RESAMPLE_POINTS - 2];

int turnRegionStart[RESAMPLE_POINTS - 2];
int turnRegionEnd[RESAMPLE_POINTS - 2];
float turnRegionSum[RESAMPLE_POINTS - 2];
int turnRegionCount;

float distanceBetweenPoints;
float point_x[RESAMPLE_POINTS];
float point_y[RESAMPLE_POINTS];
float lineLength;
float pointDistance;

int CornerIndex[RESAMPLE_POINTS];
int CornerCount;
int Fixed_CornerIndex[RESAMPLE_POINTS];
int Fixed_CornerCount;
int Decision;

float circle_sum_x;
float circle_sum_y;
float Center_x;
float Center_y;

float Radius_distance[RESAMPLE_POINTS];
float Radius_sum;
float Radius_avg;

float angle;
float edgeLength1;
float edgeLength2;
float edgeSlope;

float idealCornerX[RESAMPLE_POINTS];
float idealCornerY[RESAMPLE_POINTS];
int IdealCornerIndex[RESAMPLE_POINTS];

float angleAtPoint(float x1, float y1,
                   float x2, float y2,
                   float x3, float y3);

static float distanceAtIndex(int first, int second)
{
    float dx = point_x[first] - point_x[second];
    float dy = point_y[first] - point_y[second];
    return std::sqrt(dx * dx + dy * dy);
}

bool resample(const QMap<long long, QPointF> &points)
{
    int totalPoints = points.size();

    if (totalPoints < 100)
    {
        Decision = RECOG_LENGTH_ERROR;
        return false;
    }

    QVector<QPointF> sampledPoints;

    sampledPoints.reserve(totalPoints);

    for (auto it = points.constBegin(); it != points.constEnd(); ++it)
    {
        sampledPoints.append(it.value());
    }

    float step = static_cast<float>(totalPoints - 1) / 63.0f;

    for (int i = 0; i < RESAMPLE_POINTS; i++)
    {
        int index = static_cast<int>(i * step);

        point_x[i] = sampledPoints[index].x();
        point_y[i] = sampledPoints[index].y();
    }

    float totalDistance = 0.0f;

    for (int i = 0; i < 63; i++)
    {
        float dx = point_x[i + 1] - point_x[i];
        float dy = point_y[i + 1] - point_y[i];

        totalDistance += std::sqrt(dx * dx + dy * dy);
    }

    distanceBetweenPoints = totalDistance / 63.0f;
    return true;
}

float clamp01(float value)
{
    if (value < 0.0f)
        return 0.0f;

    if (value > 1.0f)
        return 1.0f;

    return value;
}

void CalculateDeltaTheta(int POINT_LENGTH)
{
    // it calculates the angle between consecutive segments of the stroke and stores them in DeltaTheta array.
    for (int i = 0; i < POINT_LENGTH - 1; i++)
    {
        float dx = point_x[i + 1] - point_x[i];
        float dy = point_y[i + 1] - point_y[i];

        Theta[i] = std::atan2(dy, dx) * 180.0f / M_PI;
    }

    for (int i = 0; i < POINT_LENGTH - 2; i++)
    {
        DeltaTheta[i] = Theta[i + 1] - Theta[i];

        if (DeltaTheta[i] > 180)
            DeltaTheta[i] -= 360;

        if (DeltaTheta[i] < -180)
            DeltaTheta[i] += 360;
    }
}

float TotalTurnDegree(int POINT_LENGTH)
{
    // it calculates the total turn degree of the stroke by summing up the angles in DeltaTheta array.
    // it stores positive and negative angles so that it can be used to calculate the total turn degree and total absolute turn degree separately.
    // with help of this we can calculate lines circles and squares which has different turn degree values.
    float totalTurnDegree = 0;
    for (int i = 0; i < POINT_LENGTH - 2; i++)
    {
        totalTurnDegree += DeltaTheta[i];
    }
    return totalTurnDegree;
}

float TotalAbsTurnDegree(int POINT_LENGTH)
{
    // it calculates the total absolute turn degree of the stroke by summing up the absolute values of angles in DeltaTheta array.
    // with help of this we can see how much noise or zigzag this function has
    float totalAbsTurnDegree = 0;
    for (int i = 0; i < POINT_LENGTH - 2; i++)
    {
        totalAbsTurnDegree += std::abs(DeltaTheta[i]);
    }
    return totalAbsTurnDegree;
}

float TotalPathLength(int POINT_LENGTH)
{
    // it calculates the total path of the stroke by summing up the distances between consecutive points in the point_x and point_y arrays.
    // we use this function to see how long the stroke is
    // with the help of this function and the start and end point distance we can calculate the straightness score of the stroke
    float totalLength = 0.0f;

    for (int i = 0; i < POINT_LENGTH - 1; i++)
    {
        float dx = point_x[i + 1] - point_x[i];
        float dy = point_y[i + 1] - point_y[i];

        totalLength += std::sqrt(dx * dx + dy * dy);
    }

    return totalLength;
}

void findTurnRegions(int POINT_LENGTH)
{
    // it identifies regions of the stroke where the turn degree exceeds a certain threshold (MIN_CHANGE_DEGREE).
    // this function stores the start and end of the regions and the sum of the turn degrees in those regions.
    // if the sum is greater than STRONG_REGION_TURN then we can say that this is a strong turn region
    turnRegionCount = 0;
    bool inRegion = false;
    int gapCount = 0;
    const int MAX_GAP = 1;

    for (int i = 0; i < POINT_LENGTH - 2; i++)
    {
        float turn = std::abs(DeltaTheta[i]);

        if (turn > MIN_CHANGE_DEGREE)
        {
            if (!inRegion)
            {
                turnRegionStart[turnRegionCount] = i;
                turnRegionSum[turnRegionCount] = DeltaTheta[i];
                inRegion = true;
            }
            else
            {
                turnRegionSum[turnRegionCount] += DeltaTheta[i];
            }

            gapCount = 0;
        }
        // we forgive the small gaps in the turn regions, if the gap count is more then MAX_GAP(i set it to 1) it ends the turn region
        else if (inRegion)
        {
            gapCount++;

            if (gapCount > MAX_GAP)
            {
                turnRegionEnd[turnRegionCount] = i - gapCount;
                turnRegionCount++;

                inRegion = false;
                gapCount = 0;
            }
        }
    }

    // if the stroke ends while still in a turn region, we close that region at the end of the stroke
    // i might change thet in the next updates
    if (inRegion)
    {
        turnRegionEnd[turnRegionCount] =
            POINT_LENGTH - 3 - gapCount;

        turnRegionCount++;
    }

    int validRegionCount = 0;
    // we filter out the turn regions that do not meet the STRONG_REGION_TURN threshold, keeping only the valid ones
    // in later versions i can substract the last delta theta if its a gap
    for (int i = 0; i < turnRegionCount; i++)
    {
        if (std::abs(turnRegionSum[i]) >= STRONG_REGION_TURN)
        {
            turnRegionStart[validRegionCount] = turnRegionStart[i];
            turnRegionEnd[validRegionCount] = turnRegionEnd[i];
            turnRegionSum[validRegionCount] = turnRegionSum[i];

            validRegionCount++;
        }
    }

    turnRegionCount = validRegionCount;
}

float StraightnessScore(float totalLength, int POINT_LENGTH)
{
    float dx =
        point_x[POINT_LENGTH - 1] - point_x[0];

    float dy =
        point_y[POINT_LENGTH - 1] - point_y[0];

    float startEndDistance =
        std::sqrt(dx * dx + dy * dy);

    if (totalLength <= 0.0f)
        return 0.0f;

    float straightness =
        startEndDistance / totalLength;

    if (straightness > 1.0f)
        straightness = 1.0f;

    if (straightness < 0.0f)
        straightness = 0.0f;

    return straightness;
}

int DirectionChangeCount(int POINT_LENGTH)
{
    int directionChangeCount = 0;

    for (int i = 1; i < POINT_LENGTH - 2; i++)
    {
        if (std::abs(DeltaTheta[i]) < MIN_CHANGE_DEGREE ||
            std::abs(DeltaTheta[i - 1]) < MIN_CHANGE_DEGREE)
            continue;

        if (DeltaTheta[i] * DeltaTheta[i - 1] < 0)
            directionChangeCount++;
    }

    return directionChangeCount;
}

float LineScore(float totalTurnDegree, float straightnessScore, int directionChangeCount, int turnRegionCount, int totalAbsTurnDegree)
{

    /*
        Turn amount puanı:

        Dönüş 90 derece ise 0 puan.
        Dönüş 0 derece ise +20 puan.
        Dönüş 180 derece ise -20 puan.
    */
    float turnScore =
        20.0f -
        (totalTurnDegree / 180.0f) * 40.0f;

    if (turnScore > 20.0f)
    {
        turnScore = 20.0f;
    }

    if (turnScore < -20.0f)
    {
        turnScore = -20.0f;
    }

    /*
        Region puanı:

        0 region varsa +20.
        1 veya daha fazla region varsa -10.
    */
    float regionScore;

    if (turnRegionCount == 0)
    {
        regionScore = 20.0f;
    }
    else
    {
        regionScore = -10.0f * turnRegionCount;
    }
    float degreePenalty = 0;
    if (totalAbsTurnDegree > 180.0f)
    float degreePenalty = -20;
    /*
        Her yön değişiminde 2 puan ceza.
        En fazla 60 puan ceza.
    */
    float zigzagPenalty =
        directionChangeCount * 2.0f;

    if (zigzagPenalty > 60.0f)
        zigzagPenalty = 60.0f;

    float strScore = 0;
    if(straightnessScore >= 0.90)
        strScore = straightnessScore * 60.0f;

    float score = strScore;
    score += turnScore;
    score += regionScore;
    score -= zigzagPenalty;
    score += degreePenalty;

    return score;
}

float TriangleScore(
    float shapeFitScore,
    float closureScore)
{
    float score = 0.0f;

    score += shapeFitScore * 0.80f;
    score += closureScore * 0.20f;

    return score;
}

float CalculateShapeFitError(
    int shapeType,
    const float idealCornerX[],
    const float idealCornerY[],
    const int regionStart[],
    const int regionEnd[],
    int POINT_LENGTH)
{
    // check it again later
    int cornerCount = 0;

    if (shapeType == RECOG_TRIANGLE)
        cornerCount = 3;
    else if (shapeType == RECOG_SQUARE)
        cornerCount = 4;
    else
        return 0.0f;

    // İdeal şeklin ortalama kenar uzunluğu
    float averageEdgeLength = 0.0f;

    for (int i = 0; i < cornerCount; i++)
    {
        int next = (i + 1) % cornerCount;

        float dx = idealCornerX[next] - idealCornerX[i];
        float dy = idealCornerY[next] - idealCornerY[i];

        averageEdgeLength += std::sqrt(dx * dx + dy * dy);
    }

    averageEdgeLength /= cornerCount;

    if (averageEdgeLength <= 0.0f)
        return 0.0f;

    float totalNormalizedError = 0.0f;
    int comparedPointCount = 0;

    for (int corner = 0; corner < cornerCount; corner++)
    {
        int start = regionStart[corner];
        int end = regionEnd[corner];

        /*
            Sanal başlangıç-bitiş region'u:

            start = 0
            end   = 63

            Bütün stroke köşe değildir.
            Yalnızca ilk ve son noktayı karşılaştırıyoruz.
        */
        bool isVirtualClosure =
            (start == 0 && end == POINT_LENGTH - 1) ||
            (start == POINT_LENGTH - 1 && end == 0);

        if (isVirtualClosure)
        {
            float dx1 = point_x[0] - idealCornerX[corner];
            float dy1 = point_y[0] - idealCornerY[corner];

            float dx2 =
                point_x[POINT_LENGTH - 1] -
                idealCornerX[corner];

            float dy2 =
                point_y[POINT_LENGTH - 1] -
                idealCornerY[corner];

            float distance1 = std::sqrt(dx1 * dx1 + dy1 * dy1);
            float distance2 = std::sqrt(dx2 * dx2 + dy2 * dy2);

            totalNormalizedError +=
                distance1 / averageEdgeLength;

            totalNormalizedError +=
                distance2 / averageEdgeLength;

            comparedPointCount += 2;
            continue;
        }

        /*
            DeltaTheta[i], point[i+1] üzerindeki dönüşü temsil eder.
            Bu yüzden pointIndex = i + 1.
        */
        for (int i = start; i <= end; i++)
        {
            int pointIndex = i + 1;

            if (pointIndex < 0 ||
                pointIndex >= POINT_LENGTH)
            {
                continue;
            }

            float dx =
                point_x[pointIndex] -
                idealCornerX[corner];

            float dy =
                point_y[pointIndex] -
                idealCornerY[corner];

            float distance = std::sqrt(dx * dx + dy * dy);

            totalNormalizedError +=
                distance / averageEdgeLength;

            comparedPointCount++;
        }
    }

    if (comparedPointCount == 0)
        return 0.0f;

    float averageError =
        totalNormalizedError / comparedPointCount;

    /*
        Ortalama hata, kenar uzunluğunun %30'una ulaşınca
        skor sıfıra yaklaşır.

        Bu değeri testlerden sonra ayarlarız.
    */
    const float MAX_ACCEPTABLE_ERROR = 0.30f;

    float score =
        1.0f - averageError / MAX_ACCEPTABLE_ERROR;

    score = clamp01(score);

    return score * 100.0f;
}

bool FindLineIntersection(
    float x1,
    float y1,
    float theta1,

    float x2,
    float y2,
    float theta2,

    float *cornerX,
    float *cornerY)
{
    if (cornerX == nullptr || cornerY == nullptr)
        return false;

    float theta1Radians = theta1 * M_PI / 180.0f;
    float theta2Radians = theta2 * M_PI / 180.0f;

    float directionX1 = std::cos(theta1Radians);
    float directionY1 = std::sin(theta1Radians);

    float directionX2 = std::cos(theta2Radians);
    float directionY2 = std::sin(theta2Radians);

    /*
        Birinci doğru:
            P1 + t * D1

        İkinci doğru:
            P2 + u * D2

        Kesişim için:
            P1 + t * D1 = P2 + u * D2
    */

    float denominator =
        directionX1 * directionY2 -
        directionY1 * directionX2;

    /*
        denominator sıfıra yakınsa doğrular paralel
        veya neredeyse paraleldir.
    */
    const float PARALLEL_EPSILON = 0.0001f;

    if (std::fabs(denominator) < PARALLEL_EPSILON)
        return false;

    float differenceX = x2 - x1;
    float differenceY = y2 - y1;

    float t =
        (differenceX * directionY2 -
         differenceY * directionX2) /
        denominator;

    *cornerX = x1 + t * directionX1;
    *cornerY = y1 + t * directionY1;

    return true;
}
bool FindLineToEdge(
    int startIndex,
    int endIndex,
    int POINT_LENGTH,
    float *centerX,
    float *centerY,
    float *averageTheta)
{
    if (POINT_LENGTH < 2)
        return false;

    if (startIndex < 0)
        startIndex = 0;

    if (startIndex >= POINT_LENGTH)
        startIndex = POINT_LENGTH - 1;

    if (endIndex < 0)
        endIndex = 0;

    if (endIndex >= POINT_LENGTH)
        endIndex = POINT_LENGTH - 1;

    float sumX = 0.0f;
    float sumY = 0.0f;

    float cosSum = 0.0f;
    float sinSum = 0.0f;

    int pointCount = 0;
    int thetaCount = 0;

    /*
        Normal aralık:
            start = 10, end = 20
            10, 11, ... 20

        Wrap aralık:
            start = 55, end = 8
            55, 56, ... 63, 0, 1, ... 8
    */
    int numberOfPoints =
        ((endIndex - startIndex + POINT_LENGTH) % POINT_LENGTH) + 1;

    for (int offset = 0; offset < numberOfPoints; offset++)
    {
        int pointIndex =
            (startIndex + offset) % POINT_LENGTH;

        sumX += point_x[pointIndex];
        sumY += point_y[pointIndex];
        pointCount++;

        /*
            Theta[i], point[i] -> point[i+1] segmentini temsil ediyor.

            Son noktaya ait Theta yok:
            Theta dizisi 0 ... POINT_LENGTH-2 aralığında.
        */
        if (pointIndex < POINT_LENGTH - 1)
        {
            float radians =
                Theta[pointIndex] * M_PI / 180.0f;

            cosSum += std::cos(radians);
            sinSum += std::sin(radians);

            thetaCount++;
        }
    }

    if (pointCount == 0 || thetaCount == 0)
        return false;

    *centerX = sumX / pointCount;
    *centerY = sumY / pointCount;

    *averageTheta =
        std::atan2(sinSum, cosSum) * 180.0f / M_PI;

    return true;
}

float LineAngleDifference(float angle1, float angle2)
{
    float difference =
        std::fmod(std::abs(angle1 - angle2), 180.0f);

    if (difference > 90.0f)
        difference = 180.0f - difference;

    return difference;
}

float AverageParallelAngle(float angle1, float angle2)
{
    float radians1 =
        angle1 * 2.0f * M_PI / 180.0f;

    float radians2 =
        angle2 * 2.0f * M_PI / 180.0f;

    float averageRadians = std::atan2(
        std::sin(radians1) + std::sin(radians2),
        std::cos(radians1) + std::cos(radians2));

    return averageRadians * 0.5f * 180.0f / M_PI;
}

bool MakeOppositeEdgesParallel(float edgeTheta[4])
{
    const float PARALLEL_TOLERANCE = 20.0f;

    if (LineAngleDifference(edgeTheta[0], edgeTheta[2]) > PARALLEL_TOLERANCE)
    {
        return false;
    }

    if (LineAngleDifference(edgeTheta[1], edgeTheta[3]) > PARALLEL_TOLERANCE)
    {
        return false;
    }

    float averageTheta02 =
        AverageParallelAngle(edgeTheta[0], edgeTheta[2]);

    float averageTheta13 =
        AverageParallelAngle(edgeTheta[1], edgeTheta[3]);

    edgeTheta[0] = averageTheta02;
    edgeTheta[2] = averageTheta02;

    edgeTheta[1] = averageTheta13;
    edgeTheta[3] = averageTheta13;

    return true;
}

bool CreateIdealShape(
    int regionStart[],
    int regionEnd[],
    int regionCount,
    int POINT_LENGTH)
{
    float avarageThetaForLine[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float avarageXForLine[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float avarageYForLine[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float cornerX = 0.0f;
    float cornerY = 0.0f;
    float avgTheta = 0.0f;

    for (int i = 0; i < regionCount; i++)
    {
        int nextRegion =
            (i + 1) % regionCount;

        int edgeStart =
            (regionEnd[i] + 1) % POINT_LENGTH;

        int edgeEnd =
            regionStart[nextRegion];

        bool doesEdgeWork = FindLineToEdge(
            edgeStart,
            edgeEnd,
            POINT_LENGTH,
            &cornerX,
            &cornerY,
            &avgTheta);

        if (!doesEdgeWork)
            return false;

        avarageThetaForLine[i] = avgTheta;
        avarageXForLine[i] = cornerX;
        avarageYForLine[i] = cornerY;
    }

    int firstLine[4] = {0, 1, 2, 3};
    int secondLine[4] = {2, 0, 1, 2};

    if (regionCount == 4)
        secondLine[0] = 3;

    if (regionCount == 4)
    {
        bool canCreateParallelShape =
            MakeOppositeEdgesParallel(avarageThetaForLine);

        if (!canCreateParallelShape)
            return false;
    }

    float idealCornerXValue = 0.0f;
    float idealCornerYValue = 0.0f;

    for (int i = 0; i < regionCount; i++)
    {
        bool intersectionFound = FindLineIntersection(
            avarageXForLine[firstLine[i]],
            avarageYForLine[firstLine[i]],
            avarageThetaForLine[firstLine[i]],
            avarageXForLine[secondLine[i]],
            avarageYForLine[secondLine[i]],
            avarageThetaForLine[secondLine[i]],
            &idealCornerXValue,
            &idealCornerYValue);

        if (!intersectionFound)
            return false;

        idealCornerX[i] = idealCornerXValue;
        idealCornerY[i] = idealCornerYValue;
    }

    return true;
}

float CalculateShapeFitTriangle(int turnRegionCount, int POINT_LENGTH, float totalTurnDegree)
{
    // i will check that and look for bugs
    float score = 0.0f;
    int newturnRegionCount = turnRegionCount;
    int newturnRegionStart[5];
    int newturnRegionEnd[5];
    if (turnRegionCount == 2)
    {
        newturnRegionCount += 1;
        newturnRegionStart[0] = POINT_LENGTH - 1;
        newturnRegionEnd[0] = 0;
        for (int i = 0; i < 2; i++)
        {
            newturnRegionStart[i + 1] = turnRegionStart[i];
            newturnRegionEnd[i + 1] = turnRegionEnd[i];
        }
        // add 0 and 63 as the first region
    }
    else if (turnRegionCount == 3 && std::abs(360 - std::abs(totalTurnDegree)) < ANGLE_TRESHOLD)
    {
        for (int i = 0; i < 3; i++)
        {
            newturnRegionStart[i] = turnRegionStart[i];
            newturnRegionEnd[i] = turnRegionEnd[i];
        }
    }
    else
    {
        return 0.0f;
    }
    if (newturnRegionCount == 3)
    {
        bool doesfunctionwork = CreateIdealShape(newturnRegionStart, newturnRegionEnd, newturnRegionCount, POINT_LENGTH);
        if (!doesfunctionwork)
            return 0;
        score = CalculateShapeFitError(
            RECOG_TRIANGLE,
            idealCornerX,
            idealCornerY,
            newturnRegionStart,
            newturnRegionEnd,
            POINT_LENGTH);
        return score;
    }
    else
    {
        score = 0.0f;
    }

    return score;
}

float CalculateShapeFitSquare(int turnRegionCount, int POINT_LENGTH, float totalTurnDegree)
{
    // i will check that and look for bugs
    float score = 0.0f;
    int newturnRegionCount = turnRegionCount;
    int newturnRegionStart[5];
    int newturnRegionEnd[5];
    if (turnRegionCount == 3 && std::abs(270 - std::abs(totalTurnDegree)) < ANGLE_TRESHOLD)
    {
        newturnRegionCount += 1;
        newturnRegionStart[0] = POINT_LENGTH - 1;
        newturnRegionEnd[0] = 0;
        for (int i = 0; i < 3; i++)
        {
            newturnRegionStart[i + 1] = turnRegionStart[i];
            newturnRegionEnd[i + 1] = turnRegionEnd[i];
        }
        // add 0 and 63 as the first region
    }
    else if (turnRegionCount == 4)
    {
        for (int i = 0; i < 4; i++)
        {
            newturnRegionStart[i] = turnRegionStart[i];
            newturnRegionEnd[i] = turnRegionEnd[i];
        }
    }
    else
    {
        return 0.0f;
    }
    if (newturnRegionCount == 4)
    {
        bool doesfunctionwork = CreateIdealShape(newturnRegionStart, newturnRegionEnd, newturnRegionCount, POINT_LENGTH);
        if (!doesfunctionwork)
            return 0;
        score = CalculateShapeFitError(
            RECOG_SQUARE,
            idealCornerX,
            idealCornerY,
            newturnRegionStart,
            newturnRegionEnd,
            POINT_LENGTH);
        return score;
    }
    else
    {
        score = 0.0f;
    }

    return score;
}

int FindRegionCornerPointIndex(int regionStart, int regionEnd, int POINT_LENGTH)
{
    int strongestDeltaIndex = regionStart;
    float strongestTurn = 0.0f;

    for (int i = regionStart; i <= regionEnd; i++)
    {
        if (i < 0 || i >= POINT_LENGTH - 2)
            continue;

        float turn = std::abs(DeltaTheta[i]);

        if (turn > strongestTurn)
        {
            strongestTurn = turn;
            strongestDeltaIndex = i;
        }
    }

    // DeltaTheta[i], point[i + 1] üzerindeki dönüşü temsil eder.
    int pointIndex = strongestDeltaIndex + 1;

    if (pointIndex < 0)
        pointIndex = 0;

    if (pointIndex >= POINT_LENGTH)
        pointIndex = POINT_LENGTH - 1;

    return pointIndex;
}

void CalculateGroupSize(int pointLength, int *groupSize)
{
    int baseSize = pointLength / TOTAL_GROUP;
    int remainder = pointLength % TOTAL_GROUP;

    for (int i = 0; i < TOTAL_GROUP; i++)
    {
        groupSize[i] = baseSize;

        if (remainder > 0)
        {
            groupSize[i]++;
            remainder--;
        }
    }
}

float DistanceBetweenPoints(
    float x1,
    float y1,
    float x2,
    float y2)
{
    float dx = x1 - x2;
    float dy = y1 - y2;

    return std::sqrt(dx * dx + dy * dy);
}

float FindCenter(int pointLength, const float *valueArray)
{
    if (pointLength <= 0)
        return 0.0f;

    float sum = 0.0f;

    for (int i = 0; i < pointLength; i++)
    {
        sum += valueArray[i];
    }

    return sum / static_cast<float>(pointLength);
}

float CalculateCircleScore(
    const float *radiusDiffPoint,
    const float *radiusAvgDiffPoint)
{
    float localDiffAvg = 0.0f;
    float globalDiffAvg = 0.0f;

    for (int i = 0; i < TOTAL_GROUP; i++)
    {
        localDiffAvg += radiusDiffPoint[i];
        globalDiffAvg += radiusAvgDiffPoint[i];
    }

    localDiffAvg /= static_cast<float>(TOTAL_GROUP);
    globalDiffAvg /= static_cast<float>(TOTAL_GROUP);

    float localScore = std::clamp(
        1.0f - localDiffAvg * ERROR_SCALE,
        0.0f,
        1.0f);

    float globalScore = std::clamp(
        1.0f - globalDiffAvg * ERROR_SCALE,
        0.0f,
        1.0f);

    printf(
        "circle localError: %.3f globalError: %.3f "
        "localScore: %.3f globalScore: %.3f final: %.3f\n",
        localDiffAvg,
        globalDiffAvg,
        localScore,
        globalScore,
        localScore * globalScore);

    return localScore * globalScore;
}

float CalculateCircleRadiusDiff(int pointLength)
{
    if (pointLength <= 0 || pointLength > RESAMPLE_POINTS)
        return 0.0f;

    int groupSize[TOTAL_GROUP];

    float radius[RESAMPLE_POINTS];
    float radiusDiffPoint[TOTAL_GROUP];
    float radiusAvgDiffPoint[TOTAL_GROUP];

    CalculateGroupSize(pointLength, groupSize);

    float centerX = FindCenter(pointLength, point_x);
    float centerY = FindCenter(pointLength, point_y);

    Center_x = centerX;
    Center_y = centerY;

    float totalRadiusSum = 0.0f;

    for (int i = 0; i < pointLength; i++)
    {
        radius[i] = DistanceBetweenPoints(
            point_x[i],
            point_y[i],
            centerX,
            centerY);

        totalRadiusSum += radius[i];
    }

    float totalRadius =
        totalRadiusSum / static_cast<float>(pointLength);
        Radius_avg = totalRadius;

    if (totalRadius <= 0.0001f)
        return 0.0f;

    int startIndex = 0;

    for (int i = 0; i < TOTAL_GROUP; i++)
    {
        if (groupSize[i] <= 0)
        {
            radiusDiffPoint[i] = 0.0f;
            radiusAvgDiffPoint[i] = 0.0f;
            continue;
        }

        float groupRadiusSum = 0.0f;

        for (int j = 0; j < groupSize[i]; j++)
        {
            groupRadiusSum += radius[startIndex + j];
        }

        float groupRadiusAvg =
            groupRadiusSum /
            static_cast<float>(groupSize[i]);

        float groupRadiusDiff = 0.0f;

        if (groupRadiusAvg > 0.0001f)
        {
            for (int j = 0; j < groupSize[i]; j++)
            {
                groupRadiusDiff += std::abs(
                                       groupRadiusAvg -
                                       radius[startIndex + j]) /
                                   groupRadiusAvg;
            }

            radiusDiffPoint[i] =
                groupRadiusDiff /
                static_cast<float>(groupSize[i]);
        }
        else
        {
            radiusDiffPoint[i] = 0.0f;
        }

        radiusAvgDiffPoint[i] =
            std::abs(groupRadiusAvg - totalRadius) /
            totalRadius;

        startIndex += groupSize[i];
    }

    return CalculateCircleScore(
        radiusDiffPoint,
        radiusAvgDiffPoint);
}

float ClosureScore(float totalPathLength, int POINT_LENGTH)
{
    if (totalPathLength <= 0.0f)
        return 0.0f;

    float dx = point_x[POINT_LENGTH - 1] - point_x[0];
    float dy = point_y[POINT_LENGTH - 1] - point_y[0];

    float closureDistance = std::sqrt(dx * dx + dy * dy);

    float closureRatio = closureDistance / totalPathLength;

    /*
        ratio = 0.00  -> 100 puan
        ratio = 0.20  -> 0 puan
    */
    const float MAX_CLOSURE_RATIO = 0.20f;

    float score =
        1.0f - closureRatio / MAX_CLOSURE_RATIO;

    return clamp01(score) * 100.0f;
}

float noiseScore(float TotalTurnDegree, float totalAbsTurnDegree)
{

    // the minimum is -20 and the maximum is + 20 and the zero point starts at 200 diff
    float noiseValue = totalAbsTurnDegree - std::abs(TotalTurnDegree);

    if (noiseValue <= 400)
        return (200 - noiseValue) / 10;
    else
        return -20.0f;
}

float SquareScore(float shapeFitScore,
                  float closureScore)
{
    float score = 0.0f;

    score += shapeFitScore * 0.80f;
    score += closureScore * 0.20f;

    return score;
}

float CircleScore(float circleShapeFit, float TotalTurnDegree)
{
    float score = 0;

    score += circleShapeFit * 0.80;

    float diff = std::abs(std::abs(TotalTurnDegree) - 360);

    if (diff < CIRCLE_ANGLE_TRESHOLD)
        score += 20.0;
    else
        score -= 20.0;

    return score;
}

void CreateDistanceMatrix(float endpointDistanceMatrix[])
{
    const int endpointCount = RESAMPLE_POINTS / 8;
    float x1, y1, x2, y2;
    for (int i = 0; i < endpointCount; i++)
    {
        x1 = point_x[i];
        y1 = point_y[i];
        for (int j = 0; j < endpointCount; j++)
        {
            x2 = point_x[RESAMPLE_POINTS - j - 1];
            y2 = point_y[RESAMPLE_POINTS - j - 1];
            endpointDistanceMatrix[i * endpointCount + j] = FindLength(x1, y1, x2, y2);
        }
    }
}

void FindClosestPointsPerRow(float endpointDistanceMatrix[], int closestEndpointPairs[])
{
    // Stores flat indices inside endpointDistanceMatrix.
    // Local endpoint index = flatIndex % endpointCount.
    const int endpointCount = RESAMPLE_POINTS / 8;
    for (int i = 0; i < endpointCount; i++)
    {
        int rowStart = i * endpointCount;
        int smallestDistanceIndex1 = rowStart;
        int smallestDistanceIndex2 = rowStart + 1;

        if (endpointDistanceMatrix[rowStart] <
            endpointDistanceMatrix[rowStart + 1])
        {
            smallestDistanceIndex1 = rowStart;
            smallestDistanceIndex2 = rowStart + 1;
        }
        else
        {
            smallestDistanceIndex1 = rowStart + 1;
            smallestDistanceIndex2 = rowStart;
        }

        for (int j = 2; j < endpointCount; j++)
        {
            int currentIndex = rowStart + j;

            if (endpointDistanceMatrix[currentIndex] <
                endpointDistanceMatrix[smallestDistanceIndex2])
            {
                if (endpointDistanceMatrix[currentIndex] <
                    endpointDistanceMatrix[smallestDistanceIndex1])
                {
                    smallestDistanceIndex2 = smallestDistanceIndex1;
                    smallestDistanceIndex1 = currentIndex;
                }
                else
                {
                    smallestDistanceIndex2 = currentIndex;
                }
            }
        }
        closestEndpointPairs[i * 2] = smallestDistanceIndex1;     // it holds numbers thru 0 to RESAMPLE_POINTS
        closestEndpointPairs[i * 2 + 1] = smallestDistanceIndex2; // if you want to find which point is the smallest just do %8 and you will find the answe
    }
}

int FindBestEndpointPair(
    const float endpointDistanceMatrix[],
    const int closestEndpointPairs[])
{
    const int endpointCount = RESAMPLE_POINTS / 8;

    float minDistance = 999999.0f;
    int bestPairIndex = -1;

    for (int i = 0; i < endpointCount * 2; i += 2)
    {
        float currentDistance =
            endpointDistanceMatrix[closestEndpointPairs[i]] +
            endpointDistanceMatrix[closestEndpointPairs[i + 1]];

        if (currentDistance < minDistance)
        {
            minDistance = currentDistance;
            bestPairIndex = i;
        }
    }

    return bestPairIndex;
}

bool CreateEndpointCandidate(
    int bestPairIndex,
    const int closestEndpointPairs[],
    int *startPoint,
    int *endPoint1,
    int *endPoint2)
{
    const int endpointCount = RESAMPLE_POINTS / 8;

    if (bestPairIndex < 0)
        return false;

    *startPoint = bestPairIndex / 2;

    int endLocal1 =
        closestEndpointPairs[bestPairIndex] % endpointCount;

    int endLocal2 =
        closestEndpointPairs[bestPairIndex + 1] % endpointCount;

    *endPoint1 =
        RESAMPLE_POINTS - 1 - endLocal1;

    *endPoint2 =
        RESAMPLE_POINTS - 1 - endLocal2;

    if (std::abs(*endPoint1 - *endPoint2) != 1)
        return false;

    return true;
}

bool FindBestStartSegment(
    int startPoint,
    int endPoint1,
    int endPoint2,
    int *startSegmentPoint1,
    int *startSegmentPoint2)
{
    const int endpointCount = RESAMPLE_POINTS / 8;

    if (startPoint < 0 || startPoint >= endpointCount)
        return false;

    *startSegmentPoint1 = startPoint;

    if (startPoint == 0)
    {
        *startSegmentPoint2 = startPoint + 1;
        return true;
    }

    if (startPoint == endpointCount - 1)
    {
        *startSegmentPoint2 = startPoint - 1;
        return true;
    }

    float leftScore =
        FindLength(
            point_x[startPoint - 1],
            point_y[startPoint - 1],
            point_x[endPoint1],
            point_y[endPoint1]) +
        FindLength(
            point_x[startPoint - 1],
            point_y[startPoint - 1],
            point_x[endPoint2],
            point_y[endPoint2]);

    float rightScore =
        FindLength(
            point_x[startPoint + 1],
            point_y[startPoint + 1],
            point_x[endPoint1],
            point_y[endPoint1]) +
        FindLength(
            point_x[startPoint + 1],
            point_y[startPoint + 1],
            point_x[endPoint2],
            point_y[endPoint2]);

    if (leftScore < rightScore)
        *startSegmentPoint2 = startPoint - 1;
    else
        *startSegmentPoint2 = startPoint + 1;

    return true;
}

bool PointIntersection(
    float x1, float x2, float y1, float y2,
    float x3, float x4, float y3, float y4,
    float *intersectionX,
    float *intersectionY)
{
    float d1 =
        (x2 - x1) * (y3 - y1) -
        (y2 - y1) * (x3 - x1);

    float d2 =
        (x2 - x1) * (y4 - y1) -
        (y2 - y1) * (x4 - x1);

    float d3 =
        (x4 - x3) * (y1 - y3) -
        (y4 - y3) * (x1 - x3);

    float d4 =
        (x4 - x3) * (y2 - y3) -
        (y4 - y3) * (x2 - x3);

    bool doesIntersect =
        d1 * d2 <= 0.0f &&
        d3 * d4 <= 0.0f;

    if (!doesIntersect)
        return false;

    float denominator =
        (x1 - x2) * (y3 - y4) -
        (y1 - y2) * (x3 - x4);

    if (std::abs(denominator) < 0.0001f)
        return false;

    float determinant1 =
        x1 * y2 - y1 * x2;

    float determinant2 =
        x3 * y4 - y3 * x4;

    *intersectionX =
        (determinant1 * (x3 - x4) -
         (x1 - x2) * determinant2) /
        denominator;

    *intersectionY =
        (determinant1 * (y3 - y4) -
         (y1 - y2) * determinant2) /
        denominator;

    return true;
}

int calculateClosestPoint(
    const float endpointDistanceMatrix[])
{

    // since we cant get the totallength at this point of the algorithm we need to use a little trick to help us
    float averageSegmentLength = 0.0f;

    for (int i = 0; i < 5; i++)
    {
        averageSegmentLength += FindLength(
            point_x[i],
            point_y[i],
            point_x[i + 1],
            point_y[i + 1]);
    }

    averageSegmentLength /= 5.0f;
    float dynamicThreshold =
        averageSegmentLength * 3.0f;
    // after this we can continue on our calculation
    float closestDistance = 9999.0f;
    int Index = -1;

    for (int i = 0; i < RESAMPLE_POINTS; i++)
    {
        if (endpointDistanceMatrix[i] < closestDistance)
        {
            Index = i;
            closestDistance = endpointDistanceMatrix[i];
        }
    }
    if (closestDistance > dynamicThreshold)
        return -1;
    else
        return Index;
}

float FindLength(float x1, float y1, float x2, float y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;

    return std::sqrt(dx * dx + dy * dy);
}

int fixPointsForIntersection()
{
    float endpointDistanceMatrix[RESAMPLE_POINTS];
    int closestEndpointPairs[RESAMPLE_POINTS / 4];

    int startPoint = 0;
    int endPoint1 = RESAMPLE_POINTS - 1;
    int endPoint2 = RESAMPLE_POINTS - 1;

    int startSegmentPoint1 = 0;
    int startSegmentPoint2 = 0;

    float intersectionX = -1.0;
    float intersectionY = -1.0;

    bool startSegmentFound = false;
    bool pointsIntersect = false;
    const int endpointCount = RESAMPLE_POINTS / 8;
    // Calculate all distances between the first and last endpoint regions.
    CreateDistanceMatrix(endpointDistanceMatrix);

    // For every starting endpoint, find the two closest ending endpoints.
    FindClosestPointsPerRow(
        endpointDistanceMatrix,
        closestEndpointPairs);
    // Choose the endpoint pair whose total distance is the smallest.
    int bestPairIndex =
        FindBestEndpointPair(
            endpointDistanceMatrix,
            closestEndpointPairs);

    // Convert the pair index into real point indexes.
    bool endpointValid =
        CreateEndpointCandidate(
            bestPairIndex,
            closestEndpointPairs,
            &startPoint,
            &endPoint1,
            &endPoint2);

    if (endpointValid)
    {
        // Decide whether the left or right segment of the start point
        // should be used for the intersection test.
        startSegmentFound =
            FindBestStartSegment(
                startPoint,
                endPoint1,
                endPoint2,
                &startSegmentPoint1,
                &startSegmentPoint2);

        if (startSegmentFound)
        {
            // Check whether the two candidate segments intersect.
            pointsIntersect =
                PointIntersection(
                    point_x[startSegmentPoint1],
                    point_x[startSegmentPoint2],
                    point_y[startSegmentPoint1],
                    point_y[startSegmentPoint2],

                    point_x[endPoint1],
                    point_x[endPoint2],
                    point_y[endPoint1],
                    point_y[endPoint2],
                    &intersectionX,
                    &intersectionY);
        }
    }

    // Default behaviour: do not modify the stroke.
    int fixedStartPoint = 0;
    int fixedEndPoint = RESAMPLE_POINTS - 1;

    // If the intersection test succeeds,
    // trim the stroke using the intersection.
    // Otherwise use the closest endpoint pair
    // as a fallback closure method.
    if (endpointValid &&
        startSegmentFound &&
        pointsIntersect)
    {
        fixedStartPoint =
            std::max(
                startSegmentPoint1,
                startSegmentPoint2);

        fixedEndPoint =
            std::min(
                endPoint1,
                endPoint2);
    }
    else
    {
        int closestIndex =
            calculateClosestPoint(
                endpointDistanceMatrix);

        if (closestIndex != -1)
        {
            fixedStartPoint =
                closestIndex / endpointCount;

            int endLocalIndex =
                closestIndex % endpointCount;

            fixedEndPoint =
                RESAMPLE_POINTS - 1 -
                endLocalIndex;
        }
    }

    // Build the new stroke using the selected start
    // and end indexes.
    int POINT_LENGTH = FixPointArray(
        fixedStartPoint,
        fixedEndPoint,
        pointsIntersect,
        intersectionX,
        intersectionY);

    return POINT_LENGTH;
}

int FixPointArray(int startPoint,
                  int endPoint,
                  bool addIntersection,
                  float intersectionX,
                  float intersectionY)
{
    // Copies the selected stroke segment to the beginning
    // of point_x and point_y and returns the new point count.
    int pointCount = 0;
    if (startPoint == 0 && endPoint == RESAMPLE_POINTS - 1)
        return RESAMPLE_POINTS;

    float sourceX[RESAMPLE_POINTS];
    float sourceY[RESAMPLE_POINTS];

    for (int i = 0; i < RESAMPLE_POINTS; ++i)
    {
        sourceX[i] = point_x[i];
        sourceY[i] = point_y[i];
    }

    if (addIntersection)
    {
        point_x[pointCount] = intersectionX;
        point_y[pointCount] = intersectionY;
        pointCount++;
    }

    for (int i = startPoint; i <= endPoint; ++i)
    {
        if (pointCount >= RESAMPLE_POINTS)
            break;

        point_x[pointCount] = sourceX[i];
        point_y[pointCount] = sourceY[i];
        pointCount++;
    }

    return pointCount;
}

int CalculateDecision(float TrianglePoint, float SquarePoint, float LinePoint, float CirclePoint)
{
    float MaxPoint = 0.0;
    int maxScoredShape;
    if (TrianglePoint > MaxPoint)
    {
        MaxPoint = TrianglePoint;
        maxScoredShape = RECOG_TRIANGLE;
    }
    if (SquarePoint > MaxPoint)
    {
        MaxPoint = SquarePoint;
        maxScoredShape = RECOG_SQUARE;
    }
    if (LinePoint > MaxPoint)
    {
        MaxPoint = LinePoint;
        maxScoredShape = RECOG_LINE;
    }
    if (CirclePoint > MaxPoint)
    {
        MaxPoint = CirclePoint;
        maxScoredShape = RECOG_CIRCLE;
    }
    if (MaxPoint > MIN_SCORE)
        return maxScoredShape;
    else
        return RECOG_UNKNOWN;
}

int stroke_recognition(const QMap<long long, QPointF> &points)
{
    // the code fixes the corner points and checks if the lines meet so they can create corners but it doesnt take those intersections as corners
    // thats because in testing we check the possiblity of first and last points creating a corner so we dont touch the corner side
    StrokeFeatures features;

    bool resampleOutput = resample(points);
    if (!resampleOutput)
    {
        int decision = RECOG_UNKNOWN; // probably will send all the scores 0,0f , 0,0f and such
        return decision;
    }
    int POINT_LENGTH = fixPointsForIntersection();
    CalculateDeltaTheta(POINT_LENGTH);
    findTurnRegions(POINT_LENGTH);

    features.totalTurnDegree = TotalTurnDegree(POINT_LENGTH);
    features.totalAbsTurnDegree = TotalAbsTurnDegree(POINT_LENGTH);
    features.totalLength = TotalPathLength(POINT_LENGTH);
    features.straightnessScore = StraightnessScore(features.totalLength, POINT_LENGTH);
    features.directionChangeCount = DirectionChangeCount(POINT_LENGTH);
    features.turnRegionCount = turnRegionCount;
    features.noise = noiseScore(features.totalTurnDegree, features.totalAbsTurnDegree);

    float lineScore = LineScore(std::abs(features.totalTurnDegree),
                                features.straightnessScore, features.directionChangeCount, features.turnRegionCount, features.totalAbsTurnDegree);
    float triangleShapeFit = CalculateShapeFitTriangle(features.turnRegionCount, POINT_LENGTH, features.totalTurnDegree);
    float closureScore = ClosureScore(features.totalLength, POINT_LENGTH);
    float triangleScore = TriangleScore(triangleShapeFit, closureScore);

    float squareShapeFit = CalculateShapeFitSquare(features.turnRegionCount, POINT_LENGTH, features.totalTurnDegree);
    float squareScore = SquareScore(squareShapeFit, closureScore);

    float circleRadiousScore = CalculateCircleRadiusDiff(POINT_LENGTH) * 100;
    float circleScore = CircleScore(circleRadiousScore, features.totalTurnDegree);

    int decision = CalculateDecision(triangleScore, squareScore, lineScore, circleScore);

    printf("lineScore: %f\n", lineScore);
    printf("totalTurnDegree: %f\n", features.totalTurnDegree);
    printf("totalAbsTurnDegree: %f\n", features.totalAbsTurnDegree);
    printf("straightnessScore: %f\n", features.straightnessScore);
    printf("directionChangeCount: %d\n", features.directionChangeCount);
    printf("turnRegionCount: %d\n", turnRegionCount);
    printf("trianglescore: %f\n", triangleScore);
    printf("closureScore: %f\n", closureScore);
    printf("triangleShapeFit: %f\n", triangleShapeFit);
    printf("noise: %f\n", features.noise);
    printf("new arraylength : %d\n", POINT_LENGTH);
    printf("squareScore: %f\n", squareScore);
    printf("squareShapeFit: %f\n", squareShapeFit);
    printf("shape: %d\n", decision);
    printf("circleRadiousScore: %f\n", circleRadiousScore);
    printf("circleScore: %f\n", circleScore);

    return decision;
}