#include "PathFinding.h"

#include "Legacy/NavMesh/NavMeshMath.h"
#include "ISTE/Logger/Logger.h"
#include "ISTE/Context.h"
#include "ISTE/CU/UtilityFunctions.hpp"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"

#include "ISTE/Graphics/GraphicsEngine.h"
#include "ISTE/Graphics/DebugDrawer.h"

#include "ISTE/CU/Geometry/Intersection.h"

#include <cassert>
#include <math.h>

#include "ISTE/CU/MemTrack.hpp"
//#define USE_PIX
//#include <pix3.h>



using Node = ISTE::NavMesh::NavMeshNode;

namespace // Hidden
{
    Node* FindLowestF(std::vector<Node*>&someOpenNodes)
    {
        Node* nodeToReturn = someOpenNodes[0];

        for (size_t i = 0; i < someOpenNodes.size(); i++)
            if (someOpenNodes[i]->status == ISTE::NavMesh::NodeStatus::Open)
                if (someOpenNodes[i]->F < nodeToReturn->F)
                    nodeToReturn = someOpenNodes[i];

        return nodeToReturn;
    }
    Node* FindAnyOpenNodes(std::vector<Node*>&someOpenNodes)
    {
        for (size_t i = someOpenNodes.size(); i-- > 0;)
            if (someOpenNodes[i]->status == ISTE::NavMesh::NodeStatus::Open)
                return someOpenNodes[i];

        return nullptr;
    }
}

const int ISTE::NavMesh::FindTriangleIndexFromPos(const CU::Vec3f& aPos, ISTE::NavMesh::NavMesh& aNavMesh)
{
    // TODO:: I don't like looping through all tris just to find the one // Mathias
    auto& tris = aNavMesh.triangles;
    int closestIndex = -1;
    float currDistance = 0.f;
    float closestDistance = FLT_MAX;

    DebugDrawer& drawer = Context::Get()->myGraphicsEngine->GetDebugDrawer();

    for (size_t i = 0; i < tris.size(); i++)
    {
        //if (ISTE::NAVAccuratePointInTriangle(tris[i].vertices[0], tris[i].vertices[1], tris[i].vertices[2], aPos)) // replaced with using indices below //Mathias
        const CU::Vec3f& a = aNavMesh.vertices[tris[i].vertexIndices[0]];
        const CU::Vec3f& b = aNavMesh.vertices[tris[i].vertexIndices[1]];
        const CU::Vec3f& c = aNavMesh.vertices[tris[i].vertexIndices[2]];
        // TODO: Maybe rework this to just send in an array of 3 vertices or a NavMeshTriangle // Mathias
        if (ISTE::NAVAccuratePointInTriangle(a, b, c, aPos))
            return tris[i].index;

        CU::Vec3f points[4] = { a,b,c, tris[i].Center() };
        CU::Vec3f closePoint;

        int v1vv2 = ((aPos - a).Length() < (aPos - b).Length()) ? 0 : 1;
        int v3vv4 = ((aPos - c).Length() < (aPos - points[3]).Length()) ? 2 : 3;
        int r = ((aPos - points[v1vv2]).Length() < (aPos - points[v3vv4]).Length()) ? v1vv2 : v3vv4;
        //int r = (c.Length() < points[v1vv2].Length()) ? 2 : v1vv2;

        closePoint = points[r];

        //float v2_ls = v2.len_squared();
        //return v2 * (dot(v2, v1) / v2_ls);

        currDistance = (aPos - closePoint).LengthSqr();
        if (currDistance < closestDistance)
        {
            closestDistance = currDistance;
            closestIndex = i;
        }
    }

    return closestIndex;
}

const int ISTE::NavMesh::FindTriangleIndexFromPosInside(const CU::Vec3f& aPos, ISTE::NavMesh::NavMesh& aNavMesh)
{
    // TODO:: I don't like looping through all tris just to find the one // Mathias
    auto& tris = aNavMesh.triangles;
    for (size_t i = 0; i < tris.size(); i++)
    {
        //if (ISTE::NAVAccuratePointInTriangle(tris[i].vertices[0], tris[i].vertices[1], tris[i].vertices[2], aPos)) // replaced with using indices below //Mathias
        const CU::Vec3f& a = aNavMesh.vertices[tris[i].vertexIndices[0]];
        const CU::Vec3f& b = aNavMesh.vertices[tris[i].vertexIndices[1]];
        const CU::Vec3f& c = aNavMesh.vertices[tris[i].vertexIndices[2]];
        // TODO: Maybe rework this to just send in an array of 3 vertices or a NavMeshTriangle // Mathias
        if (ISTE::NAVAccuratePointInTriangle(a, b, c, aPos))
            return tris[i].index;

    }

    return -1;
}

const bool ISTE::NavMesh::RayIntersectsNavMesh(const CU::Vec3f& aRayOri, const CU::Vec3f& aRayDir, CU::Vec3f& anOutIntersectionPoint)
{
    //PIXScopedEvent(PIX_COLOR(0, 255, 255), "Scoped Event RayIntersectsNavMesh()");

    const float EPSILON = 0.0000001f;
    Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();

    const NavMesh& navMesh = scene.GetNavMesh();
    // WIP:: instead of checking the whole navmesh triangles find which grid cell ray is intersecting and get all triangles in that cell // Mathias
    //std::vector<ISTE::NavMesh::NavMeshTriangle*> triangles;
    //if (!scene.GetNavMeshGrid().GetTriangles(aRayOri, aRayDir, triangles))
    //    return false;

    auto& triangles = navMesh.triangles;
    
    for (auto& triangle : triangles)
    {
        const CU::Vec3f& vert0 = navMesh.vertices[triangle.vertexIndices[0]];
        const CU::Vec3f& vert1 = navMesh.vertices[triangle.vertexIndices[1]];
        const CU::Vec3f& vert2 = navMesh.vertices[triangle.vertexIndices[2]];

        const CU::Vec3f edge1 = vert1 - vert0;
        const CU::Vec3f edge2 = vert2 - vert0;

        const CU::Vec3f h = aRayDir.Cross(edge2);
        const float a = edge1.Dot(h);
        if (a > -EPSILON && a < EPSILON)
            continue; // This ray is parallel to this triangle.
        
        const CU::Vec3f s = aRayOri - vert0;
        const float f = 1.0f / a;
        const float u = f * s.Dot(h);
        if (u < 0.0f || u > 1.0f)
            continue;

        const CU::Vec3f q = s.Cross(edge1);
        const float v = f * aRayDir.Dot(q);
        if (v < 0.0f || u + v > 1.0f)
            continue;
        
        // At this stage we can compute t to find out where the intersection point is on the line.
        float t = f * edge2.Dot(q);
        if (t > EPSILON) // ray intersection
        {
            anOutIntersectionPoint = aRayOri + aRayDir * t;
            return true;
        }
        // This means that there is a line intersection but not a ray intersection.
    }
    return false;
}

const bool ISTE::NavMesh::RayIntersectPlane(const CU::Vec3f& aRayOri, const CU::Vec3f& aRayDir, const CU::Vec3f& aStart, CU::Vec3f& anOutIntersectionPoint)
{
    const float t = (aRayDir.GetNormalized().Length() - (aRayOri.Dot(CU::Vec3f(0, 1, 0)))) / (aRayDir.Dot(CU::Vec3f(0, 1, 0)));
    CU::Vec3f pickedPoint = aRayOri + aRayDir.GetNormalized() * t;
    const CU::Vec2f pickedPoint2D = pickedPoint;

    const auto& tris = Context::Get()->mySceneHandler->GetActiveScene().GetNavMesh().triangles;

    const CU::LineSegment lineStartToPick(aStart, pickedPoint);

    CU::LineSegment line1;
    CU::LineSegment line2;
    CU::LineSegment line3;
    CU::Vec2f out2D(FLT_MAX, FLT_MAX);
    CU::Vec2f outDiff;

    for (auto& tri : tris)
    {
        line1.myFrom = *tri.vertices[0];
        line1.myTo   = *tri.vertices[1];
        line2.myFrom = *tri.vertices[1];
        line2.myTo   = *tri.vertices[2];
        line3.myFrom = *tri.vertices[2];
        line3.myTo   = *tri.vertices[0];
        
        if (CU::LineSegmentIntersect(lineStartToPick, line1, outDiff))
            if (CU::Vec2f(pickedPoint2D - outDiff).LengthSqr() < CU::Vec2f(pickedPoint2D - out2D).LengthSqr())
                out2D = outDiff;

        if (CU::LineSegmentIntersect(lineStartToPick, line2, outDiff))
            if (CU::Vec2f(pickedPoint2D - outDiff).LengthSqr() < CU::Vec2f(pickedPoint2D - out2D).LengthSqr())
                out2D = outDiff;

        if (CU::LineSegmentIntersect(lineStartToPick, line3, outDiff))
            if (CU::Vec2f(pickedPoint2D - outDiff).LengthSqr() < CU::Vec2f(pickedPoint2D - out2D).LengthSqr())
                out2D = outDiff;
    }

    const CU::Vec2f offset2D = (lineStartToPick.myTo - lineStartToPick.myFrom).GetNormalized();
    const CU::Vec3f offset(offset2D.x, 0.f, offset2D.y);
    anOutIntersectionPoint = CU::Vec3f(out2D.x, aStart.y, out2D.y) - offset * 0.1f;

    return true;
}

const bool ISTE::NavMesh::RayIntersectionOff(const CU::Vec3f& aRayOri, const CU::Vec3f& aRayDir, float aNonResponsiveAngle, const CU::Vec3f& aStart, CU::Vec3f& anOutIntersectionPoint)
{
    const float t = (aRayDir.GetNormalized().Length() - (aRayOri.Dot(CU::Vec3f(0, 1, 0)))) / (aRayDir.Dot(CU::Vec3f(0, 1, 0)));
    CU::Vec3f pickedPoint = aRayOri + aRayDir.GetNormalized() * t;
    const CU::Vec2f pickedPoint2D = pickedPoint;

    const auto& tris = Context::Get()->mySceneHandler->GetActiveScene().GetNavMesh().triangles;

    //CU::Vec2f sPos2D = CU::Vec2f(aStart.x, aStart.z);

    //sPos2D = sPos2D - pickedPoint2D;

    //float s = sin(45);
    //float c = cos(45);

    //float xNew = sPos2D.x * c - sPos2D.y * s;
    //float yNew = sPos2D.x * s + sPos2D.y * c;

    //sPos2D.x = xNew + pickedPoint.x;
    //sPos2D.y = yNew + pickedPoint.y;

    const CU::LineSegment lineStartToPick(/*CU::Vec3f(sPos2D.x, aStart.y, sPos2D.y)*/aStart, pickedPoint);

    CU::LineSegment line1;
    CU::LineSegment line2;
    CU::LineSegment line3;
    CU::Vec2f out2D(FLT_MAX, FLT_MAX);
    CU::Vec2f outDiff;
    CU::LineSegment intersectedLIne ;

    for (auto& tri : tris)
    {
        line1.myFrom = *tri.vertices[0];
        line1.myTo   = *tri.vertices[1];
        line2.myFrom = *tri.vertices[1];
        line2.myTo   = *tri.vertices[2];
        line3.myFrom = *tri.vertices[2];
        line3.myTo   = *tri.vertices[0];

        if (CU::LineSegmentIntersect(lineStartToPick, line1, outDiff))
            if (CU::Vec2f(pickedPoint2D - outDiff).LengthSqr() < CU::Vec2f(pickedPoint2D - out2D).LengthSqr())
            {
                intersectedLIne = line1;
                out2D = outDiff;
            }

        if (CU::LineSegmentIntersect(lineStartToPick, line2, outDiff))
            if (CU::Vec2f(pickedPoint2D - outDiff).LengthSqr() < CU::Vec2f(pickedPoint2D - out2D).LengthSqr())
            {
                intersectedLIne = line2;
                out2D = outDiff;
            }

        if (CU::LineSegmentIntersect(lineStartToPick, line3, outDiff))
            if (CU::Vec2f(pickedPoint2D - outDiff).LengthSqr() < CU::Vec2f(pickedPoint2D - out2D).LengthSqr())
            {
                intersectedLIne = line3;
                out2D = outDiff;
            }
    }

    const CU::Vec2f offset2D = (lineStartToPick.myTo - lineStartToPick.myFrom).GetNormalized();
    const CU::Vec3f offset(offset2D.x, 0.f, offset2D.y);

    anOutIntersectionPoint = CU::Vec3f(out2D.x, aStart.y, out2D.y) - offset * 0.1f;

    if ((anOutIntersectionPoint - aStart).LengthSqr() >= 0.1f)
        return true;

    CU::Vec2f dToP = pickedPoint2D - out2D;

    CU::Vec2f lineTing = (intersectedLIne.myTo - intersectedLIne.myFrom);

    float angle = dToP.Dot(lineTing);

    if (abs(angle) <= aNonResponsiveAngle)
    {
        anOutIntersectionPoint = aStart;
        return true;
    }

    //this dosn't take in to account a change in elevation //TODO: uppgift2: MAKE IT 

    CU::Vec2f projected = lineTing * angle;

    CU::Vec3f projected3d = CU::Vec3f(projected.x, 0, projected.y);

    anOutIntersectionPoint += projected3d.GetNormalized();
    

    return true;
}

const bool ISTE::NavMesh::RayClosestPointIntersection(const CU::Vec3f& aRayOri, const CU::Vec3f& aRayDir, float aNonResponsiveAngle, const CU::Vec3f& aStart, CU::Vec3f& anOutIntersectionPoint, float aMaxDist, bool aIgnoreProjection)
{
    //const float t = (aRayDir.GetNormalized().Length() - (aRayOri.Dot(CU::Vec3f(0, 1, 0)))) / (aRayDir.Dot(CU::Vec3f(0, 1, 0)));
    //CU::Vec3f pickedPoint = aRayOri + aRayDir.GetNormalized() * t;
    //const CU::Vec2f pickedPoint2D = pickedPoint;

    const auto& tris = Context::Get()->mySceneHandler->GetActiveScene().GetNavMesh().triangles;

    //CU::LineSegment lineStartToPick(/*CU::Vec3f(sPos2D.x, aStart.y, sPos2D.y)*/aStart, pickedPoint);

    CU::LineSegment lineStartToPick;
    CU::Vec2f pickedPoint2D;

    lineStartToPick.myFrom = aStart;

    CU::LineSegment line1;
    CU::LineSegment line2;
    CU::LineSegment line3;
    CU::Vec2f out2D(FLT_MAX, FLT_MAX);
    CU::Vec2f outDiff;
    CU::LineSegment intersectedLIne;
    float outY = 0;

    bool hit = false;

    CU::Vec3f fVert;
    CU::Vec3f tVert;

    //DebugDrawer& drawer = Context::Get()->myGraphicsEngine->GetDebugDrawer();
    //drawer.ClearStaticCommands();
    //LineCommand lineCom;

    //lineCom.myColor = { 100,0,0 };

    for (auto& tri : tris)
    {

        CU::Vec3f middle = tri.Center();

        if ((aRayOri - middle).Length() >= aMaxDist)
            continue;

        float yDiff = abs(aRayOri.y - middle.y);

        yDiff /= abs(aRayDir.y);

        CU::Vec3f pickedPoint = aRayOri + (aRayDir * yDiff);

        //if ((aStart - middle).Length() >= aMaxDist) //more accurate test but dont wanna go through every level
        //     continue;

        lineStartToPick.myTo = pickedPoint;

        pickedPoint2D = pickedPoint;

        line1.myFrom = *tri.vertices[0];
        line1.myTo   = *tri.vertices[1];
        line2.myFrom = *tri.vertices[1];
        line2.myTo   = *tri.vertices[2];
        line3.myFrom = *tri.vertices[2];
        line3.myTo   = *tri.vertices[0];

        //lineCom.myFromPosition = aStart;
        //lineCom.myToPosition = pickedPoint;
        //
        //drawer.AddStaticLineCommand(lineCom);

        //lineCom.myFromPosition = tri.vertices[0];
        //lineCom.myToPosition = tri.vertices[1];
        //drawer.AddStaticLineCommand(lineCom);
        //lineCom.myFromPosition = tri.vertices[1];
        //lineCom.myToPosition = tri.vertices[2];
        //drawer.AddStaticLineCommand(lineCom);
        //lineCom.myFromPosition = tri.vertices[2];
        //lineCom.myToPosition = tri.vertices[0];
        //drawer.AddStaticLineCommand(lineCom);

        if (CU::LineSegmentIntersect(lineStartToPick, line1, outDiff))
            if (CU::Vec2f(pickedPoint2D - outDiff).LengthSqr() < CU::Vec2f(pickedPoint2D - out2D).LengthSqr())
            {
                intersectedLIne = line1;
                out2D = outDiff;
                hit = true;

                CU::Vec2f t = line1.myTo - line1.myFrom;
                CU::Vec2f tDiff = outDiff - line1.myFrom;

                float p = tDiff.Length() / t.Length();
                outY = (tri.vertices[1]->y - tri.vertices[0]->y) * p + tri.vertices[0]->y;

                fVert = *tri.vertices[0];
                tVert = *tri.vertices[1];
            }

        if (CU::LineSegmentIntersect(lineStartToPick, line2, outDiff))
            if (CU::Vec2f(pickedPoint2D - outDiff).LengthSqr() < CU::Vec2f(pickedPoint2D - out2D).LengthSqr())
            {
                intersectedLIne = line2;
                out2D = outDiff;
                hit = true;

                CU::Vec2f t = line2.myTo - line2.myFrom;
                CU::Vec2f tDiff = outDiff - line2.myFrom;

                float p = tDiff.Length() / t.Length();
                outY = (tri.vertices[2]->y - tri.vertices[1]->y) * p + tri.vertices[1]->y;

                fVert = *tri.vertices[1];
                tVert = *tri.vertices[2];
            }

        if (CU::LineSegmentIntersect(lineStartToPick, line3, outDiff))
            if (CU::Vec2f(pickedPoint2D - outDiff).LengthSqr() < CU::Vec2f(pickedPoint2D - out2D).LengthSqr())
            {
                intersectedLIne = line3;
                out2D = outDiff;
                hit = true;

                CU::Vec2f t = line3.myTo - line3.myFrom;
                CU::Vec2f tDiff = outDiff - line3.myFrom;

                float p = tDiff.Length() / t.Length();
                outY = (tri.vertices[0]->y - tri.vertices[2]->y) * p + tri.vertices[2]->y;

                fVert = *tri.vertices[2];
                tVert = *tri.vertices[0];
            }
    }

    if (!hit)
        return false;

    const CU::Vec2f offset2D = (lineStartToPick.myTo - lineStartToPick.myFrom).GetNormalized();
    const CU::Vec3f offset(offset2D.x, 0.f, offset2D.y);

    anOutIntersectionPoint = CU::Vec3f(out2D.x, outY, out2D.y) - offset * 0.1f;

    if ((anOutIntersectionPoint - aStart).Length() >= 0.1f || aIgnoreProjection)
        return true;

    CU::Vec2f dToP = pickedPoint2D - out2D;

    CU::Vec2f lineTing = (intersectedLIne.myTo - intersectedLIne.myFrom);

    float angle = dToP.Dot(lineTing);

    if (abs(angle) <= aNonResponsiveAngle)
    {
        anOutIntersectionPoint = aStart;
        return true;
    }

    //In reality we would also have to account for the offset since, while minimum it would on height diffrences cause a change in elevation
    //I rather not deal with this

    CU::Vec2f projected = lineTing * angle;

    CU::Vec3f projected3d = CU::Vec3f(projected.x, 0, projected.y);

    anOutIntersectionPoint += projected3d.GetNormalized();

    CU::Vec2f t = intersectedLIne.myTo - intersectedLIne.myFrom;
    CU::Vec2f tDiff = CU::Vec2f(anOutIntersectionPoint.x, anOutIntersectionPoint.z) - intersectedLIne.myFrom;

    float p = tDiff.Length() / t.Length();
    outY = (tVert.y - fVert.y) * p + fVert.y;

    anOutIntersectionPoint.y = outY;


    return true;
}

void ISTE::NavMesh::GetIntersections(NavMesh& aNavMesh, const CU::Vec3f& aStart, const CU::Vec3f& aDestination, std::vector<Intersection>& aOutIntersections)
{
    const auto& tris = Context::Get()->mySceneHandler->GetActiveScene().GetNavMesh().triangles;

    //CU::LineSegment lineStartToPick(/*CU::Vec3f(sPos2D.x, aStart.y, sPos2D.y)*/aStart, pickedPoint);

    CU::LineSegment lineStartToPick(aStart, aDestination);
    CU::Vec2f pickedPoint2D;

    CU::LineSegment line1;
    CU::LineSegment line2;
    CU::LineSegment line3;
    CU::Vec2f outDiff;
    float outY = 0;


    for (auto& tri : tris)
    {
        Intersection intersection;
        bool hit = false;

        //intersection.myIntersections.clear();
        //intersection.myTriangleIndex = -1;
      //lineStartToPick.myTo = pickedPoint;

        line1.myFrom = *tri.vertices[0];
        line1.myTo   = *tri.vertices[1];
        line2.myFrom = *tri.vertices[1];
        line2.myTo   = *tri.vertices[2];
        line3.myFrom = *tri.vertices[2];
        line3.myTo   = *tri.vertices[0];

        if (CU::LineSegmentIntersect(lineStartToPick, line1, outDiff))
        {
            CU::Vec2f t = line1.myTo - line1.myFrom;
            CU::Vec2f tDiff = outDiff - line1.myFrom;

            float p = tDiff.Length() / t.Length();
            float y = (tri.vertices[1]->y - tri.vertices[0]->y) * p + tri.vertices[0]->y;

            intersection.myIntersections.push_back({ outDiff.x, y, outDiff.y });
            intersection.myTriangleIndex = tri.index;

            hit = true;
        }

        if (CU::LineSegmentIntersect(lineStartToPick, line2, outDiff))
        {
            CU::Vec2f t = line2.myTo - line2.myFrom;
            CU::Vec2f tDiff = outDiff - line2.myFrom;

            float p = tDiff.Length() / t.Length();
            float y = (tri.vertices[2]->y - tri.vertices[1]->y) * p + tri.vertices[1]->y;

            intersection.myIntersections.push_back({ outDiff.x, y, outDiff.y });
            intersection.myTriangleIndex = tri.index;

            hit = true;
        }

        if (CU::LineSegmentIntersect(lineStartToPick, line3, outDiff))
        {
            CU::Vec2f t = line3.myTo - line3.myFrom;
            CU::Vec2f tDiff = outDiff - line3.myFrom;

            float p = tDiff.Length() / t.Length();
            float y = (tri.vertices[0]->y - tri.vertices[2]->y) * p + tri.vertices[2]->y;

            intersection.myIntersections.push_back({ outDiff.x, y, outDiff.y });
            intersection.myTriangleIndex = tri.index;

            hit = true;
        }

        if (intersection.myIntersections.size() == 0 && hit)
        {
            std::cout << "Fuck you!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        }

        if (hit)
        {
            aOutIntersections.push_back(intersection);
        }
    }

    std::vector<unsigned int> premutation(aOutIntersections.size());
   
    for (unsigned int i = 0; i < aOutIntersections.size(); i++)
        premutation[i] = i;

    std::sort(premutation.begin(), premutation.end(), [&aOutIntersections, aDestination](unsigned int aI1, unsigned int aI2)
        {
            CU::Vec3f i1Min = aOutIntersections[aI1].myIntersections[0];
            CU::Vec3f I2Min = aOutIntersections[aI2].myIntersections[0];

            for (size_t i = 1; i < aOutIntersections[aI1].myIntersections.size(); i++)
            {
                i1Min = (aDestination - i1Min).Length() > (aDestination - aOutIntersections[aI1].myIntersections[i]).Length() ? aOutIntersections[aI1].myIntersections[i] : i1Min;
            }

            for (size_t i = 1; i < aOutIntersections[aI2].myIntersections.size(); i++)
            {
                I2Min = (aDestination - I2Min).Length() > (aDestination - aOutIntersections[aI2].myIntersections[i]).Length() ? aOutIntersections[aI2].myIntersections[i] : I2Min;
            }

            return ((aDestination - i1Min).Length() < (aDestination - I2Min).Length());
        }
    );

    std::vector<Intersection> temp;
    temp = aOutIntersections;

    for (size_t i = 0; i < premutation.size(); i++)
    {
        aOutIntersections[i] = temp[premutation[i]];
    }

    //Fuck, kill me, kill me, kill me not another one

    for (auto& t : aOutIntersections)
    {
        std::vector<unsigned int> premutation2(t.myIntersections.size());

        for (unsigned int i = 0; i < t.myIntersections.size(); i++)
            premutation2[i] = i;

        std::sort(premutation2.begin(), premutation2.end(),[&t, aDestination](unsigned int aI1, unsigned int aI2)
            {
                return (aDestination - t.myIntersections[aI1]).Length() < (aDestination - t.myIntersections[aI2]).Length();
            });


        std::vector<CU::Vec3f> temp2;
        temp2 = t.myIntersections;

        for (size_t i = 0; i < premutation2.size(); i++)
        {
            t.myIntersections[i] = temp2[premutation2[i]];
        }
    }

}

float ISTE::NavMesh::GetAproximatedY(ISTE::NavMesh::NavMeshTriangle& aTri, const CU::Vec3f& aPoint)
{
    //CU::Vec2f vMin;
    //CU::Vec2f vMax;

    //vMin.x = min(aTri.vertices[0].x, min(aTri.vertices[1].x, aTri.vertices[2].x));
    ////vMin.y = min(aTri.vertices[0].y, min(aTri.vertices[1].y, aTri.vertices[2].y));
    //vMin.y = min(aTri.vertices[0].z, min(aTri.vertices[1].z, aTri.vertices[2].z));

    //vMax.x = max(aTri.vertices[0].x, max(aTri.vertices[1].x, aTri.vertices[2].x));
    ////vMax.y = max(aTri.vertices[0].y, max(aTri.vertices[1].y, aTri.vertices[2].y));
    //vMax.y = max(aTri.vertices[0].z, max(aTri.vertices[1].z, aTri.vertices[2].z));

    //CU::Vec2f t = vMax - vMin;
    //CU::Vec2f tDiff = CU::Vec2f(aPoint.x, aPoint.z) - vMin;

    //float p = tDiff.Length() / t.Length();
     
    //float vMinY = min(aTri.vertices[0].y, min(aTri.vertices[1].y, aTri.vertices[2].y));
    //float vMaxY = max(aTri.vertices[0].y, max(aTri.vertices[1].y, aTri.vertices[2].y));

    //return (vMaxY - vMinY) * p + vMinY;

    CU::Vec3f normal = (*aTri.vertices[1] - *aTri.vertices[0]).Cross(*aTri.vertices[2] - *aTri.vertices[0]);
    normal.Normalize();
    CU::Vec3f diff = aPoint - aTri.Center();
    float scalar = diff.Dot(normal);
    CU::Vec3f projectedPoint = aPoint - scalar * normal;

    return projectedPoint.y;
}

CU::Vec3f ISTE::NavMesh::GetRealisticPointInNavmeshFromPoint(ISTE::NavMesh::NavMesh& aNavMesh, const CU::Vec3f& aStart, const CU::Vec3f& aDestination)
{
    const auto& tris = Context::Get()->mySceneHandler->GetActiveScene().GetNavMesh().triangles;

    //CU::LineSegment lineStartToPick(/*CU::Vec3f(sPos2D.x, aStart.y, sPos2D.y)*/aStart, pickedPoint);

    CU::LineSegment lineStartToPick(aStart, aDestination);
    CU::Vec2f pickedPoint2D = aDestination;

    CU::LineSegment line1;
    CU::LineSegment line2;
    CU::LineSegment line3;
    CU::Vec2f out2D(FLT_MAX, FLT_MAX);
    CU::Vec2f outDiff;
    CU::LineSegment intersectedLIne;

    float outY = 0;

    bool hit = false;

    for (auto& tri : tris)
    {

        line1.myFrom = *tri.vertices[0];
        line1.myTo   = *tri.vertices[1];
        line2.myFrom = *tri.vertices[1];
        line2.myTo   = *tri.vertices[2];
        line3.myFrom = *tri.vertices[2];
        line3.myTo   = *tri.vertices[0];

        if (CU::LineSegmentIntersect(lineStartToPick, line1, outDiff))
            if (CU::Vec2f(pickedPoint2D - outDiff).LengthSqr() < CU::Vec2f(pickedPoint2D - out2D).LengthSqr())
            {
                intersectedLIne = line1;
                out2D = outDiff;

                CU::Vec2f t = line1.myTo - line1.myFrom;
                CU::Vec2f tDiff = outDiff - line1.myFrom;

                float p = tDiff.Length() / t.Length();
                outY = (tri.vertices[1]->y - tri.vertices[0]->y) * p + tri.vertices[0]->y;
                hit = true;
            }

        if (CU::LineSegmentIntersect(lineStartToPick, line2, outDiff))
            if (CU::Vec2f(pickedPoint2D - outDiff).LengthSqr() < CU::Vec2f(pickedPoint2D - out2D).LengthSqr())
            {
                intersectedLIne = line2;
                out2D = outDiff;

                CU::Vec2f t = line2.myTo - line2.myFrom;
                CU::Vec2f tDiff = outDiff - line2.myFrom;

                float p = tDiff.Length() / t.Length();
                outY = (tri.vertices[2]->y - tri.vertices[1]->y) * p + tri.vertices[1]->y;
                hit = true;
            }

        if (CU::LineSegmentIntersect(lineStartToPick, line3, outDiff))
            if (CU::Vec2f(pickedPoint2D - outDiff).LengthSqr() < CU::Vec2f(pickedPoint2D - out2D).LengthSqr())
            {
                intersectedLIne = line3;
                out2D = outDiff;

                CU::Vec2f t = line3.myTo - line3.myFrom;
                CU::Vec2f tDiff = outDiff - line3.myFrom;

                float p = tDiff.Length() / t.Length();
                outY = (tri.vertices[0]->y - tri.vertices[2]->y) * p + tri.vertices[2]->y;
                hit = true;
            }
    }

    if (!hit)
    {
        return aStart;
    }

    const CU::Vec2f offset2D = (lineStartToPick.myTo - lineStartToPick.myFrom).GetNormalized();
    const CU::Vec3f offset(offset2D.x, 0.f, offset2D.y);

    return CU::Vec3f(out2D.x, outY, out2D.y) - offset * 0.1f;
}

std::vector<int> ISTE::NavMesh::FindIndexPath(const CU::Vec3f& aStart, const CU::Vec3f& aDestination)
{
    //PIXScopedEvent(PIX_COLOR(0, 255, 0), "Scoped Event FindIndexPath()");

    NavMesh& navMesh = Context::Get()->mySceneHandler->GetActiveScene().GetNavMesh();
    std::vector<int> path;

    //changed from FindTriangleIndexFromPos //Lukas
    //int startIndex = FindTriangleIndexFromPosInside(aStart, navMesh);
    //int endIndex = FindTriangleIndexFromPosInside(aDestination, navMesh);

    int startIndex = FindTriangleIndexFromPos(aStart, navMesh);
    int endIndex = FindTriangleIndexFromPos(aDestination, navMesh);

    if (startIndex < 0) // TODO:: Find out why we get index -1. This check is currently needed because FindTriangleIndexFromPos() sometimes return -1; // Mathias
        return std::vector<int> {};
    if (endIndex < 0)
        return std::vector<int> {};

    std::vector<Node*> nodes;
    std::vector<Node*> nodeList;
    nodeList.resize(navMesh.triangles.size());

    nodes.push_back(new Node(nullptr, startIndex, NodeStatus::Open));
    nodeList[nodes[0]->index] = nodes[0];

    while (FindAnyOpenNodes(nodes) != nullptr)
    {
        Node* current = FindLowestF(nodes);

        current->status = NodeStatus::Closed;

        if (current->index == endIndex)
        {
            Node* parent = current->parent;
            path.push_back(current->index);

            while (parent != nullptr)
            {
                path.push_back(parent->index);
                parent = parent->parent;
            }

            for (size_t i = nodes.size(); i-- > 0;)
                delete nodes[i];

            std::reverse(path.begin(), path.end());
            return path;
        }

        // AddMore to open
        for (int i : navMesh.triangles[current->index].connections)
        {
            if (i == current->index)
                continue;
            if (nodeList[i] != nullptr)
                continue;


            Node* node = new Node(current, i, NodeStatus::Open);
            {
                CU::Vec3f from = navMesh.triangles[current->index].Center();
                CU::Vec3f to   = navMesh.triangles[node->index].Center();
                const float length = CU::Vec3f(to - from).Length();
                node->G = current->G + 1;
                node->H = length;
                node->F = (float)node->G * node->H;
            }
            nodes.push_back(node);
            nodeList[node->index] = node;
        }
    }

    for (size_t i = nodes.size(); i-- > 0;)
        delete nodes[i];

    return std::vector<int>{};
}

void ISTE::NavMesh::FindPointPath(const CU::Vec3f& aStart, const CU::Vec3f& aDestination, std::queue<CU::Vec3f>& aPathOut, bool aIgnoreExtraPoints)
{
    const NavMesh& navMesh = Context::Get()->mySceneHandler->GetActiveScene().GetNavMesh();

    const std::vector<int> nodeList = FindIndexPath(aStart, aDestination);
    //PIXScopedEvent(PIX_COLOR(0, 0, 255), "Scoped Event FindPointPath()");
    const size_t nodeCount = nodeList.size();

    // Abort if list is empty.
    if (nodeCount == 0)
    {
        aPathOut.push(aStart);
        return;
    }
    // Return immediately if list only has one entry.
    if (nodeCount == 1)
    {
        aPathOut.push(aDestination);
        return;
    }

    //aPathOut.push(aStart);

    std::vector<int> leftVerts;
    std::vector<int> rightVerts;

    leftVerts.resize(nodeCount + 1);
    rightVerts.resize(nodeCount + 1);

    CU::Vec3f apex = aStart;
    size_t left = 1;
    size_t right = 1;

    //DebugDrawer& drawer = Context::Get()->myGraphicsEngine->GetDebugDrawer();
    //drawer.ClearStaticCommands();
    //LineCommand rightLine;
    //rightLine.myColor = { 1,0,0 };
    //LineCommand leftLine;
    //leftLine.myColor = { 0,0,1 };



    //by me lukas to gather y data
    struct Portal
    {
        CU::Vec3f myLeftPos;
        CU::Vec3f myRightPos;
    };
    std::vector<Portal> portals;



    // Initialise portal vertices.
    for (size_t node = 0; node + 1 < nodeCount; node++)
    {
        auto& connections = navMesh.triangles[nodeList[node]].connections;

        const std::array<unsigned int, 3> currVertices = navMesh.triangles[nodeList[node]].vertexIndices;
        const std::array<unsigned int, 3> nextVertices = navMesh.triangles[nodeList[node + 1]].vertexIndices;

        int exclusive = INT_MAX;
        std::vector<int> sharedVerts;

        for (const int& curr : currVertices)
        {
            bool isShared = false;

            for (const int& next : nextVertices)
            {
                if (curr == next)
                {
                    isShared = true;
                    sharedVerts.push_back(curr);
                    break;
                }
            }

            if (!isShared)
                exclusive = curr;
        }

        const CU::Vec3f first = navMesh.vertices[sharedVerts[0]];
        const CU::Vec3f second = navMesh.vertices[sharedVerts[1]];
        const CU::Vec3f exclusiveVert = navMesh.vertices[exclusive];
        if (CU::VectorSign(first - exclusiveVert, second - exclusiveVert) < 0)
            std::swap(sharedVerts[0], sharedVerts[1]);



        Portal portal;
        portal.myLeftPos = navMesh.vertices[sharedVerts[0]];
        portal.myRightPos = navMesh.vertices[sharedVerts[1]];
        portals.push_back(portal);



        leftVerts[node + 1] = sharedVerts[0];
        rightVerts[node + 1] = sharedVerts[1];
    }

    // Initialise portal vertices first point.
    for (size_t i = 0; i < 3; i++)
    {
        if (navMesh.triangles[nodeList[0]].vertexIndices[i] != leftVerts[1] &&
            navMesh.triangles[nodeList[0]].vertexIndices[i] != rightVerts[1])
        {
            leftVerts[0] = navMesh.triangles[nodeList[0]].vertexIndices[i];
            rightVerts[0] = navMesh.triangles[nodeList[0]].vertexIndices[i];
        }
    }

    // Initialise portal vertices last point.
    for (int j = 0; j < 3; j++)
    {
        if (navMesh.triangles[nodeList[nodeCount - 1]].vertexIndices[j] != leftVerts[nodeCount - 1] &&
            navMesh.triangles[nodeList[nodeCount - 1]].vertexIndices[j] != rightVerts[nodeCount - 1])
        {
            leftVerts[nodeCount] = navMesh.triangles[nodeList[nodeCount - 1]].vertexIndices[j];
            rightVerts[nodeCount] = navMesh.triangles[nodeList[nodeCount - 1]].vertexIndices[j];
        }
    }

    // Step through channel.
    for (size_t i = 2; i <= nodeCount; i++)
    {
        // If new left vertex is different, process.
        if (leftVerts[i] != leftVerts[left] && i > left)
        {
            const CU::Vec3f newSide = navMesh.vertices[leftVerts[i]] - apex;

            // If new side does not widen funnel, update.
            if (CU::VectorSign(newSide, navMesh.vertices[leftVerts[left]] - apex) < 0)
            {
                if (CU::VectorSign(newSide, navMesh.vertices[rightVerts[right]] - apex) < 0)
                {
                    size_t next = right;

                    // Find next vertex.
                    for (size_t j = next; j <= nodeCount; j++)
                    {
                        if (navMesh.vertices[rightVerts[j]] != navMesh.vertices[rightVerts[next]])
                        {
                            next = j;
                            break;
                        }
                    }

                    aPathOut.push(navMesh.vertices[rightVerts[right]]);
                    apex = navMesh.vertices[rightVerts[right]];
                    right = next;
                }
                else
                {
                  /*  leftLine.myFromPosition = apex;
                    leftLine.myToPosition = navMesh.vertices[leftVerts[left]];
                    drawer.AddStaticLineCommand(leftLine);*/
                    left = i;
                }
            }
        }
        // If new right vertex is different, process.
        if (rightVerts[i] != rightVerts[right] && i > right)
        {
            const CU::Vec3f newSide = navMesh.vertices[rightVerts[i]] - apex;

            // If new side does not widen funnel, update. // here
            if (CU::VectorSign(newSide, navMesh.vertices[rightVerts[right]] - apex) > 0)
            {
                if (CU::VectorSign(newSide, navMesh.vertices[leftVerts[left]] - apex) > 0)
                {
                    size_t next = left;
                    // Find next vertex.
                    for (size_t j = next; j <= nodeCount; j++)
                    {
                        if (navMesh.vertices[leftVerts[j]] != navMesh.vertices[leftVerts[next]])
                        {
                            next = j;
                            break;
                        }
                    }

                    aPathOut.push(navMesh.vertices[leftVerts[left]]);
                    apex = navMesh.vertices[leftVerts[left]];
                    left = next;
                }
                else
                {
                   /* rightLine.myFromPosition = apex;
                    rightLine.myToPosition = navMesh.vertices[rightVerts[right]];
                    drawer.AddStaticLineCommand(rightLine);*/
                    right = i;
                }
            }
        }
    }





    if (aIgnoreExtraPoints)
    {
        aPathOut.push(aDestination);
        return;
    }

    CU::Vec3f lastP = aStart;

    ////if (!aPathOut.empty())
    ////{
    ////    lastP = aPathOut.front();
    ////}

    ////this is not really the best of solutions but most cases
    ////where we wrap the next wrap will be in a good position even though there are a few edge cases
    if (aPathOut.empty())
    {
        CU::LineSegment line1(lastP, aDestination);

        CU::Vec2f lastOut = lastP;

        for (auto& port : portals)
        {
            CU::LineSegment line2(port.myLeftPos, port.myRightPos);

            CU::Vec2f out;
            if (CU::LineSegmentIntersect(line1, line2, out))
            {
                //if it's tiny we want to skip
                if ((out - lastOut).Length() < 0.01f)
                    continue;

                lastOut = out;

                CU::Vec2f t = line2.myTo - line2.myFrom;
                CU::Vec2f tDiff = out - line2.myFrom;

                float p = tDiff.Length() / t.Length();
                float y = (port.myRightPos.y - port.myLeftPos.y) * p + port.myLeftPos.y;
                //float y = abs(port.myRightPos.y - port.myLeftPos.y) / 2.f;
                //y = min(port.myRightPos.y, port.myLeftPos.y) + y;

                aPathOut.push(CU::Vec3f(out.x, y, out.y));
            }
        }
    }
    else
    {
        std::queue<CU::Vec3f> temp = aPathOut;
        aPathOut = std::queue<CU::Vec3f>();

        lastP = temp.front();

        CU::LineSegment line1(aStart, lastP);

        CU::Vec2f lastOut = lastP;

        for (auto& port : portals)
        {
            CU::LineSegment line2(port.myLeftPos, port.myRightPos);

            CU::Vec2f out;
            if (CU::LineSegmentIntersect(line1, line2, out))
            {
                //if it's tiny we want to skip
                if ((out - lastOut).Length() < 0.01f)
                    continue;

                lastOut = out;

                CU::Vec2f t = line2.myTo - line2.myFrom;
                CU::Vec2f tDiff = out - line2.myFrom;

                float p = tDiff.Length() / t.Length();
                float y = (port.myRightPos.y - port.myLeftPos.y) * p + port.myLeftPos.y;

                aPathOut.push(CU::Vec3f(out.x, y, out.y));
            }
        }

        while (!temp.empty())
        {
            aPathOut.push(temp.front());
            temp.pop();
        }

    }

    aPathOut.push(aDestination);
    return;
}
/*
std::vector<int> ISTE::NavMesh::FindIndexPath(const CU::Vec3f& aStart, const CU::Vec3f& aDestination, NavMesh& aNavMesh)
{
    std::vector<int> path;

    //SetUp data

    int startIndex = FindTriangleIndexFromPos(aStart, aNavMesh);
    int endIndex = FindTriangleIndexFromPos(aDestination, aNavMesh);

    // TODO:: Refactor to use the NavMeshNodes instead of NavMeshTriangle // Mathias, see NewFindIndexPath
    std::vector<NavMeshTriangle*> openSet;
    std::vector<NavMeshTriangle*> closedSet;

    closedSet.push_back(&aNavMesh.triangles[startIndex]);

    closedSet[0]->F = 0;
    closedSet[0]->status = NodeStatus::Closed;

    for (auto& conn : aNavMesh.triangles[startIndex].connections)
    {
        openSet.push_back(&aNavMesh.triangles[conn]);
        openSet.back()->parentIndex = closedSet[0]->index;
    }


    while (!openSet.empty())
    {
        if (openSet[0]->index == endIndex)
        {
            closedSet.push_back(openSet[0]);
            break;
        }

        openSet[0]->F = aNavMesh.triangles[openSet[0]->parentIndex].F + 1;

        for (auto& conn : openSet[0]->connections)
        {
            if (aNavMesh.triangles[conn].status == NodeStatus::Closed)
                continue;

            openSet.push_back(&aNavMesh.triangles[conn]);
            openSet.back()->parentIndex = openSet[0]->index;
        }

        //openSet <- openSet[0].neighbours unllesa UwU neighbours is closed->prev = openset[0]

        openSet[0]->status = NodeStatus::Closed;
        closedSet.push_back(openSet[0]);
        openSet.erase(openSet.begin());

    }

    int indexc = endIndex;

    while (indexc != startIndex)
    {
        path.push_back(indexc);
        indexc = aNavMesh.triangles[indexc].parentIndex;
    }

    std::reverse(path.begin(), path.end());

    //reset data
    ClearCache(aNavMesh); // TODO:: This will not be needed when we're using NavMeshNodes // Mathias

    //add sometype of algorithm to generate new point "optimal path"

    //depending on above solution and unity export data we may need some type of steering algorithm

    return path;
}
*/
const CU::Vec2f ISTE::MovementMath::GetUnitVector(const CU::Vec3f& aStart, const CU::Vec3f& aEnd)
{
    CU::Vec2f vector = { aEnd.x - aStart.x, aEnd.z - aStart.z };

    CU::Vec2f unitVector = vector.GetNormalized();

    return unitVector;
}

CU::Vec2f ISTE::MovementMath::GetUnitVector(CU::Vec3f& aStart, CU::Vec3f& aEnd, float& aLength)
{
    CU::Vec2f vector = { aEnd.x - aStart.x, aEnd.z - aStart.z };

    aLength = vector.Length();

    CU::Vec2f unitVector = vector.GetNormalized();

    return unitVector;
}

CU::Vec3f ISTE::MovementMath::GetUnitVector3(CU::Vec3f& aStart, CU::Vec3f& aEnd, float& aLength)
{
    CU::Vec3f vector = aEnd - aStart;

    aLength = vector.Length();

    CU::Vec3f unitVector = vector.GetNormalized();

    return unitVector;
}

const CU::Vec3f ISTE::MovementMath::GetUnitVector3(CU::Vec3f& aStart, CU::Vec3f& aEnd)
{
    CU::Vec3f vector = aEnd - aStart;

    CU::Vec3f unitVector = vector.GetNormalized();

    return unitVector;
}