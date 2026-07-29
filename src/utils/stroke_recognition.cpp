#include "utils/stroke_recognition.h"
#include "widgets/DrawingWidget.h"

float FindLength(float x1, float y1, float x2, float y2);
void FixPointArray(int startPoint, int endPoint, bool addIntersection,
                   float intersectionX, float intersectionY,
                   StrokeVariables &variables);

bool resample(const QMap<long long, QPointF> &points, StrokeVariables &variables)
{
    int totalPoints = points.size();

    if (totalPoints < 100)
    {
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

        variables.points[i] = sampledPoints[index];
    }

    variables.pointCount = RESAMPLE_POINTS;
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

void CalculateDeltaTheta(StrokeVariables &variables)
{
    // it calculates the angle between consecutive segments of the stroke and stores them in DeltaTheta array.
    for (int i = 0; i < variables.pointCount - 1; i++)
    {
        float dx =
            variables.points[i + 1].x() -
            variables.points[i].x();

        float dy =
            variables.points[i + 1].y() -
            variables.points[i].y();

        variables.theta[i] = std::atan2(dy, dx) * 180.0f / M_PI;
    }

    for (int i = 0; i < variables.pointCount - 2; i++)
    {
        variables.deltaTheta[i] = variables.theta[i + 1] - variables.theta[i];

        if (variables.deltaTheta[i] > 180)
            variables.deltaTheta[i] -= 360;

        if (variables.deltaTheta[i] < -180)
            variables.deltaTheta[i] += 360;
    }
}

float TotalTurnDegree(const StrokeVariables &variables)
{
    // it calculates the total turn degree of the stroke by summing up the angles in DeltaTheta array.
    // it stores positive and negative angles so that it can be used to calculate the total turn degree and total absolute turn degree separately.
    // with help of this we can calculate lines circles and squares which has different turn degree values.
    float totalTurnDegree = 0;
    for (int i = 0; i < variables.pointCount - 2; i++)
    {
        totalTurnDegree += variables.deltaTheta[i];
    }
    return totalTurnDegree;
}

float TotalAbsTurnDegree(const StrokeVariables &variables)
{
    // it calculates the total absolute turn degree of the stroke by summing up the absolute values of angles in DeltaTheta array.
    // with help of this we can see how much noise or zigzag this function has
    float totalAbsTurnDegree = 0;
    for (int i = 0; i < variables.pointCount - 2; i++)
    {
        totalAbsTurnDegree += std::abs(variables.deltaTheta[i]);
    }
    return totalAbsTurnDegree;
}

float TotalPathLength(const StrokeVariables &variables)
{
    // it calculates the total path of the stroke by summing up the distances between consecutive points in the point_x and point_y arrays.
    // we use this function to see how long the stroke is
    // with the help of this function and the start and end point distance we can calculate the straightness score of the stroke
    float totalLength = 0.0f;

    for (int i = 0; i < variables.pointCount - 1; i++)
    {
        float dx = variables.points[i + 1].x() - variables.points[i].x();
        float dy = variables.points[i + 1].y() - variables.points[i].y();

        totalLength += std::sqrt(dx * dx + dy * dy);
    }

    return totalLength;
}

void findTurnRegions(StrokeVariables &variables)
{
    // it identifies regions of the stroke where the turn degree exceeds a certain threshold (MIN_CHANGE_DEGREE).
    // this function stores the start and end of the regions and the sum of the turn degrees in those regions.
    // if the sum is greater than STRONG_REGION_TURN then we can say that this is a strong turn region
    variables.turnRegionCount = 0;
    bool inRegion = false;
    int gapCount = 0;
    const int MAX_GAP = 1;

    for (int i = 0; i < variables.pointCount - 2; i++)
    {
        float turn = std::abs(variables.deltaTheta[i]);

        if (turn > MIN_CHANGE_DEGREE)
        {
            if (!inRegion)
            {
                variables.turnRegionStart[variables.turnRegionCount] = i;
                variables.turnRegionSum[variables.turnRegionCount] = variables.deltaTheta[i];
                inRegion = true;
            }
            else
            {
                variables.turnRegionSum[variables.turnRegionCount] += variables.deltaTheta[i];
            }

            gapCount = 0;
        }
        // we forgive the small gaps in the turn regions, if the gap count is more then MAX_GAP(i set it to 1) it ends the turn region
        else if (inRegion)
        {
            gapCount++;

            if (gapCount > MAX_GAP)
            {
                variables.turnRegionEnd[variables.turnRegionCount] = i - gapCount;
                variables.turnRegionCount++;

                inRegion = false;
                gapCount = 0;
            }
        }
    }

    // if the stroke ends while still in a turn region, we close that region at the end of the stroke
    // i might change thet in the next updates
    if (inRegion)
    {
        variables.turnRegionEnd[variables.turnRegionCount] =
            variables.pointCount - 3 - gapCount;

        variables.turnRegionCount++;
    }

    int validRegionCount = 0;
    // we filter out the turn regions that do not meet the STRONG_REGION_TURN threshold, keeping only the valid ones
    // in later versions i can substract the last delta theta if its a gap
    for (int i = 0; i < variables.turnRegionCount; i++)
    {
        if (std::abs(variables.turnRegionSum[i]) >= STRONG_REGION_TURN)
        {
            variables.turnRegionStart[validRegionCount] = variables.turnRegionStart[i];
            variables.turnRegionEnd[validRegionCount] = variables.turnRegionEnd[i];
            variables.turnRegionSum[validRegionCount] = variables.turnRegionSum[i];

            validRegionCount++;
        }
    }

    variables.turnRegionCount = validRegionCount;
}

float StraightnessScore(const StrokeFeatures &features, const StrokeVariables &variables)
{
    float dx =
        variables.points[variables.pointCount - 1].x() - variables.points[0].x();

    float dy =
        variables.points[variables.pointCount - 1].y() - variables.points[0].y();

    float startEndDistance =
        std::sqrt(dx * dx + dy * dy);

    if (features.totalLength <= 0.0f)
        return 0.0f;

    float straightness =
        startEndDistance / features.totalLength;

    if (straightness > 1.0f)
        straightness = 1.0f;

    if (straightness < 0.0f)
        straightness = 0.0f;

    return straightness;
}

int DirectionChangeCount(const StrokeVariables &variables)
{
    int directionChangeCount = 0;

    for (int i = 1; i < variables.pointCount - 2; i++)
    {
        if (std::abs(variables.deltaTheta[i]) < MIN_CHANGE_DEGREE ||
            std::abs(variables.deltaTheta[i - 1]) < MIN_CHANGE_DEGREE)
            continue;

        if (variables.deltaTheta[i] * variables.deltaTheta[i - 1] < 0)
            directionChangeCount++;
    }

    return directionChangeCount;
}

float LineScore(const StrokeFeatures &features)
{

    /*
        Turn amount puanı:

        Dönüş 90 derece ise 0 puan.
        Dönüş 0 derece ise +20 puan.
        Dönüş 180 derece ise -20 puan.
    */
    float turnScore =
        20.0f -
        (std::abs(features.totalTurnDegree) / 180.0f) * 40.0f;

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

    if (features.turnRegionCount == 0)
    {
        regionScore = 20.0f;
    }
    else
    {
        regionScore = -10.0f * features.turnRegionCount;
    }
    float degreePenalty = 0;
    if (features.totalAbsTurnDegree > 180.0f)
        degreePenalty = -20;
    /*
        Her yön değişiminde 2 puan ceza.
        En fazla 60 puan ceza.
    */
    float zigzagPenalty =
        features.directionChangeCount * 2.0f;

    if (zigzagPenalty > 60.0f)
        zigzagPenalty = 60.0f;

    float strScore = 0;
    if (features.straightnessScore >= 0.90)
        strScore = features.straightnessScore * 60.0f;

    float score = strScore;
    score += turnScore;
    score += regionScore;
    score -= zigzagPenalty;
    score += degreePenalty;

    return score;
}

float TriangleScore(const StrokeScore &strokeScore)
{
    float score = 0.0f;

    score += strokeScore.triangleShapeFit * 0.80f;
    score += strokeScore.closureScore * 0.20f;

    return score;
}

float CalculateShapeFitError(
    int shapeType,
    const std::array<QPointF, 4> &idealCorners,
    const int regionStart[],
    const int regionEnd[],
    const StrokeVariables &variables)
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

        float dx = idealCorners[next].x() - idealCorners[i].x();
        float dy = idealCorners[next].y() - idealCorners[i].y();

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
            (start == 0 && end == variables.pointCount - 1) ||
            (start == variables.pointCount - 1 && end == 0);

        if (isVirtualClosure)
        {
            float dx1 = variables.points[0].x() - idealCorners[corner].x();
            float dy1 = variables.points[0].y() - idealCorners[corner].y();

            float dx2 =
                variables.points[variables.pointCount - 1].x() -
                idealCorners[corner].x();

            float dy2 =
                variables.points[variables.pointCount - 1].y() -
                idealCorners[corner].y();

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
                pointIndex >= variables.pointCount)
            {
                continue;
            }

            float dx =
                variables.points[pointIndex].x() -
                idealCorners[corner].x();

            float dy =
                variables.points[pointIndex].y() -
                idealCorners[corner].y();

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
    const StrokeVariables &variables,
    float *centerX,
    float *centerY,
    float *averageTheta)
{
    if (variables.pointCount < 2)
        return false;

    if (startIndex < 0)
        startIndex = 0;

    if (startIndex >= variables.pointCount)
        startIndex = variables.pointCount - 1;

    if (endIndex < 0)
        endIndex = 0;

    if (endIndex >= variables.pointCount)
        endIndex = variables.pointCount - 1;

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
        ((endIndex - startIndex + variables.pointCount) % variables.pointCount) + 1;

    for (int offset = 0; offset < numberOfPoints; offset++)
    {
        int pointIndex =
            (startIndex + offset) % variables.pointCount;

        sumX += variables.points[pointIndex].x();
        sumY += variables.points[pointIndex].y();
        pointCount++;

        /*
            Theta[i], point[i] -> point[i+1] segmentini temsil ediyor.

            Son noktaya ait Theta yok:
            Theta dizisi 0 ... POINT_LENGTH-2 aralığında.
        */
        if (pointIndex < variables.pointCount - 1)
        {
            float radians =
                variables.theta[pointIndex] * M_PI / 180.0f;

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
    const StrokeVariables &variables,
    StrokeResult &result)
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
            (regionEnd[i] + 1) % variables.pointCount;

        int edgeEnd =
            regionStart[nextRegion];

        bool doesEdgeWork = FindLineToEdge(
            edgeStart,
            edgeEnd,
            variables,
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

        result.idealCorners[i] = QPointF(idealCornerXValue, idealCornerYValue);
    }

    return true;
}

float CalculateShapeFitTriangle(const StrokeFeatures &features,
                                const StrokeVariables &variables,
                                StrokeResult &result)
{
    // i will check that and look for bugs
    float score = 0.0f;
    int newturnRegionCount = features.turnRegionCount;
    int newturnRegionStart[5];
    int newturnRegionEnd[5];
    if (features.turnRegionCount == 2)
    {
        newturnRegionCount += 1;
        newturnRegionStart[0] = variables.pointCount - 1;
        newturnRegionEnd[0] = 0;
        for (int i = 0; i < 2; i++)
        {
            newturnRegionStart[i + 1] = variables.turnRegionStart[i];
            newturnRegionEnd[i + 1] = variables.turnRegionEnd[i];
        }
        // add 0 and 63 as the first region
    }
    else if (features.turnRegionCount == 3 && std::abs(360 - std::abs(features.totalTurnDegree)) < ANGLE_THRESHOLD)
    {
        for (int i = 0; i < 3; i++)
        {
            newturnRegionStart[i] = variables.turnRegionStart[i];
            newturnRegionEnd[i] = variables.turnRegionEnd[i];
        }
    }
    else
    {
        return 0.0f;
    }
    if (newturnRegionCount == 3)
    {
        bool doesfunctionwork = CreateIdealShape(
            newturnRegionStart, newturnRegionEnd,
            newturnRegionCount, variables, result);
        if (!doesfunctionwork)
            return 0;
        score = CalculateShapeFitError(
            RECOG_TRIANGLE,
            result.idealCorners,
            newturnRegionStart,
            newturnRegionEnd,
            variables);
        return score;
    }
    else
    {
        score = 0.0f;
    }

    return score;
}

float CalculateShapeFitSquare(const StrokeFeatures &features,
                              const StrokeVariables &variables,
                              StrokeResult &result)
{
    // i will check that and look for bugs
    float score = 0.0f;
    int newturnRegionCount = features.turnRegionCount;
    int newturnRegionStart[5];
    int newturnRegionEnd[5];
    if (features.turnRegionCount == 3 && std::abs(270 - std::abs(features.totalTurnDegree)) < ANGLE_THRESHOLD)
    {
        newturnRegionCount += 1;
        newturnRegionStart[0] = variables.pointCount - 1;
        newturnRegionEnd[0] = 0;
        for (int i = 0; i < 3; i++)
        {
            newturnRegionStart[i + 1] = variables.turnRegionStart[i];
            newturnRegionEnd[i + 1] = variables.turnRegionEnd[i];
        }
        // add 0 and 63 as the first region
    }
    else if (features.turnRegionCount == 4)
    {
        for (int i = 0; i < 4; i++)
        {
            newturnRegionStart[i] = variables.turnRegionStart[i];
            newturnRegionEnd[i] = variables.turnRegionEnd[i];
        }
    }
    else
    {
        return 0.0f;
    }
    if (newturnRegionCount == 4)
    {
        bool doesfunctionwork = CreateIdealShape(
            newturnRegionStart, newturnRegionEnd,
            newturnRegionCount, variables, result);
        if (!doesfunctionwork)
            return 0;
        score = CalculateShapeFitError(
            RECOG_SQUARE,
            result.idealCorners,
            newturnRegionStart,
            newturnRegionEnd,
            variables);
        return score;
    }
    else
    {
        score = 0.0f;
    }

    return score;
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

float CalculateCircleRadiusDiff(const StrokeVariables &variables,
                                StrokeResult &result)
{
    const int pointLength = variables.pointCount;
    if (pointLength <= 0 || pointLength > RESAMPLE_POINTS)
        return 0.0f;

    int groupSize[TOTAL_GROUP];

    float radius[RESAMPLE_POINTS];
    float radiusDiffPoint[TOTAL_GROUP];
    float radiusAvgDiffPoint[TOTAL_GROUP];

    CalculateGroupSize(pointLength, groupSize);

    float centerX = 0.0f;
    float centerY = 0.0f;
    for (int i = 0; i < pointLength; ++i)
    {
        centerX += variables.points[i].x();
        centerY += variables.points[i].y();
    }
    centerX /= pointLength;
    centerY /= pointLength;

    result.circleCenter = QPointF(centerX, centerY);

    float totalRadiusSum = 0.0f;

    for (int i = 0; i < pointLength; i++)
    {
        radius[i] = DistanceBetweenPoints(
            variables.points[i].x(),
            variables.points[i].y(),
            centerX,
            centerY);

        totalRadiusSum += radius[i];
    }

    float totalRadius =
        totalRadiusSum / static_cast<float>(pointLength);
    result.circleRadius = totalRadius;

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

float ClosureScore(const StrokeFeatures &features, const StrokeVariables &variables)
{
    if (features.totalLength <= 0.0f)
        return 0.0f;

    float dx = variables.points[variables.pointCount - 1].x() - variables.points[0].x();
    float dy = variables.points[variables.pointCount - 1].y() - variables.points[0].y();

    float closureDistance = std::sqrt(dx * dx + dy * dy);

    float closureRatio = closureDistance / features.totalLength;

    /*
        ratio = 0.00  -> 100 puan
        ratio = 0.20  -> 0 puan
    */
    const float MAX_CLOSURE_RATIO = 0.20f;

    float score =
        1.0f - closureRatio / MAX_CLOSURE_RATIO;

    return clamp01(score) * 100.0f;
}

float noiseScore(const StrokeFeatures &features)
{

    // the minimum is -20 and the maximum is + 20 and the zero point starts at 200 diff
    float noiseValue = features.totalAbsTurnDegree - std::abs(features.totalTurnDegree);

    if (noiseValue <= 400)
        return (200 - noiseValue) / 10;
    else
        return -20.0f;
}

float SquareScore(const StrokeScore &strokeScore)
{
    float score = 0.0f;

    score += strokeScore.squareShapeFit * 0.80f;
    score += strokeScore.closureScore * 0.20f;

    return score;
}

float CircleScore(const StrokeScore &strokeScore, const StrokeFeatures &features)
{
    float score = 0;

    score += strokeScore.circleRadiousScore * 0.80;

    float diff = std::abs(std::abs(features.totalTurnDegree) - 360);

    if (diff < CIRCLE_ANGLE_THRESHOLD)
        score += 20.0;
    else
        score -= 20.0;

    return score;
}

void CreateDistanceMatrix(float endpointDistanceMatrix[], const StrokeVariables &variables)
{
    const int endpointCount = RESAMPLE_POINTS / 8;
    float x1, y1, x2, y2;
    for (int i = 0; i < endpointCount; i++)
    {
        x1 = variables.points[i].x();
        y1 = variables.points[i].y();
        for (int j = 0; j < endpointCount; j++)
        {
            x2 = variables.points[RESAMPLE_POINTS - j - 1].x();
            y2 = variables.points[RESAMPLE_POINTS - j - 1].y();
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
    int *startSegmentPoint2,
    const StrokeVariables &variables)
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
            variables.points[startPoint - 1].x(),
            variables.points[startPoint - 1].y(),
            variables.points[endPoint1].x(),
            variables.points[endPoint1].y()) +
        FindLength(
            variables.points[startPoint - 1].x(),
            variables.points[startPoint - 1].y(),
            variables.points[endPoint2].x(),
            variables.points[endPoint2].y());

    float rightScore =
        FindLength(
            variables.points[startPoint + 1].x(),
            variables.points[startPoint + 1].y(),
            variables.points[endPoint1].x(),
            variables.points[endPoint1].y()) +
        FindLength(
            variables.points[startPoint + 1].x(),
            variables.points[startPoint + 1].y(),
            variables.points[endPoint2].x(),
            variables.points[endPoint2].y());

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
    const float endpointDistanceMatrix[],
    const StrokeVariables &variables)
{

    // since we cant get the totallength at this point of the algorithm we need to use a little trick to help us
    float averageSegmentLength = 0.0f;

    for (int i = 0; i < 5; i++)
    {
        averageSegmentLength += FindLength(
            variables.points[i].x(),
            variables.points[i].y(),
            variables.points[i + 1].x(),
            variables.points[i + 1].y());
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

void fixPointsForIntersection(StrokeVariables &variables)
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
    CreateDistanceMatrix(endpointDistanceMatrix, variables);

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
                &startSegmentPoint2,
                variables);

        if (startSegmentFound)
        {
            // Check whether the two candidate segments intersect.
            pointsIntersect =
                PointIntersection(
                    variables.points[startSegmentPoint1].x(),
                    variables.points[startSegmentPoint2].x(),
                    variables.points[startSegmentPoint1].y(),
                    variables.points[startSegmentPoint2].y(),

                    variables.points[endPoint1].x(),
                    variables.points[endPoint2].x(),
                    variables.points[endPoint1].y(),
                    variables.points[endPoint2].y(),
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
                endpointDistanceMatrix,
                variables);

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
    FixPointArray(
        fixedStartPoint,
        fixedEndPoint,
        pointsIntersect,
        intersectionX,
        intersectionY,
        variables);
}

void FixPointArray(int startPoint,
                   int endPoint,
                   bool addIntersection,
                   float intersectionX,
                   float intersectionY,
                   StrokeVariables &variables)
{
    // Copies the selected stroke segment to the beginning
    // of point_x and point_y and returns the new point count.
    int pointCount = 0;
    if (startPoint == 0 && endPoint == RESAMPLE_POINTS - 1)
    {
        variables.pointCount = RESAMPLE_POINTS;
        return;
    }

    float sourceX[RESAMPLE_POINTS];
    float sourceY[RESAMPLE_POINTS];

    for (int i = 0; i < RESAMPLE_POINTS; ++i)
    {
        sourceX[i] = variables.points[i].x();
        sourceY[i] = variables.points[i].y();
    }

    if (addIntersection)
    {
        variables.points[pointCount] = QPointF(intersectionX, intersectionY);
        pointCount++;
    }

    for (int i = startPoint; i <= endPoint; ++i)
    {
        if (pointCount >= RESAMPLE_POINTS)
            break;

        variables.points[pointCount] = QPointF(sourceX[i], sourceY[i]);
        pointCount++;
    }

    variables.pointCount = pointCount;
}

int CalculateDecision(const StrokeScore &strokeScore)
{
    float MaxPoint = 0.0;
    int maxScoredShape = RECOG_UNKNOWN;
    if (strokeScore.triangleScore > MaxPoint)
    {
        MaxPoint = strokeScore.triangleScore;
        maxScoredShape = RECOG_TRIANGLE;
    }
    if (strokeScore.squareScore > MaxPoint)
    {
        MaxPoint = strokeScore.squareScore;
        maxScoredShape = RECOG_SQUARE;
    }
    if (strokeScore.lineScore > MaxPoint)
    {
        MaxPoint = strokeScore.lineScore;
        maxScoredShape = RECOG_LINE;
    }
    if (strokeScore.circleScore > MaxPoint)
    {
        MaxPoint = strokeScore.circleScore;
        maxScoredShape = RECOG_CIRCLE;
    }
    if (MaxPoint > MIN_SCORE)
        return maxScoredShape;
    else
        return RECOG_UNKNOWN;
}

#ifdef DEBUG
void printDebugLine(const StrokeScore &score,
                    const StrokeFeatures &features,
                    const StrokeVariables &variables)
{
    printf("lineScore: %f\n", score.lineScore);
    printf("totalTurnDegree: %f\n", features.totalTurnDegree);
    printf("totalAbsTurnDegree: %f\n", features.totalAbsTurnDegree);
    printf("straightnessScore: %f\n", features.straightnessScore);
    printf("directionChangeCount: %d\n", features.directionChangeCount);
    printf("turnRegionCount: %d\n", variables.turnRegionCount);
    printf("trianglescore: %f\n", score.triangleScore);
    printf("closureScore: %f\n", score.closureScore);
    printf("triangleShapeFit: %f\n", score.triangleShapeFit);
    printf("noise: %f\n", features.noise);
    printf("new arraylength : %d\n", variables.pointCount);
    printf("squareScore: %f\n", score.squareScore);
    printf("squareShapeFit: %f\n", score.squareShapeFit);
    printf("shape: %d\n", score.decision);
    printf("circleRadiousScore: %f\n", score.circleRadiousScore);
    printf("circleScore: %f\n", score.circleScore);
}
#endif

void calculateFeatures(StrokeFeatures &features,
                       const StrokeVariables &variables)
{
    features.totalTurnDegree = TotalTurnDegree(variables);
    features.totalAbsTurnDegree = TotalAbsTurnDegree(variables);
    features.totalLength = TotalPathLength(variables);
    features.straightnessScore = StraightnessScore(features, variables);
    features.directionChangeCount = DirectionChangeCount(variables);
    features.turnRegionCount = variables.turnRegionCount;
    features.noise = noiseScore(features);
}

void calculateScore(StrokeScore &score,
                    const StrokeFeatures &features,
                    const StrokeVariables &variables,
                    StrokeResult &result)
{
    score.lineScore = LineScore(features);
    score.triangleShapeFit = CalculateShapeFitTriangle(features, variables, result);
    score.closureScore = ClosureScore(features, variables);
    score.triangleScore = TriangleScore(score);

    score.squareShapeFit = CalculateShapeFitSquare(features, variables, result);
    score.squareScore = SquareScore(score);

    score.circleRadiousScore = CalculateCircleRadiusDiff(variables, result) * 100;
    score.circleScore = CircleScore(score, features);

    score.decision = CalculateDecision(score);
}

bool calculateProcess(const QMap<long long, QPointF> &points, StrokeVariables &variables)
{
    bool resampleOutput = resample(points, variables);
    if (!resampleOutput)
        return false;

    fixPointsForIntersection(variables);
    CalculateDeltaTheta(variables);
    findTurnRegions(variables);
    return true;
}
int stroke_recognition(const QMap<long long, QPointF> &points,
                       StrokeVariables &variables,
                       StrokeResult &result)
{
    // the code fixes the corner points and checks if the lines meet so they can create corners but it doesnt take those intersections as corners
    // thats because in testing we check the possiblity of first and last points creating a corner so we dont touch the corner side
    StrokeFeatures features;
    variables = StrokeVariables{};
    result = StrokeResult{};
    StrokeScore score;

    bool didsuccess = calculateProcess(points, variables);

    if(!didsuccess)
        return RECOG_UNKNOWN;

    calculateFeatures(features, variables);

    calculateScore(score, features, variables, result);

#ifdef DEBUG
    printDebugLine(score, features, variables);
#endif

    return score.decision;
}
