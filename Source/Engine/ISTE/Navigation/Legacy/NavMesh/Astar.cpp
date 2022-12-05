#include "Astar.hpp"

#include "ISTE/CU/UtilityFunctions.hpp"

#ifdef _DEBUG
#include "ISTE/Graphics/DebugDrawer.h"
#include "ISTE/Graphics/GraphicsEngine.h"
#include "ISTE/Context.h"
#endif // _DEBUG
#include <d3d11.h>


ISTE::Astar::Astar(std::vector<CU::Vec3f> somePositions, std::vector<CU::Vec3i> someTrianglePoints)
{
#ifdef _DEBUG
    LineDrawer = &Context::Get()->myGraphicsEngine->GetDebugDrawer();
#endif // _DEBUG

    TileCount = (int)someTrianglePoints.size();

    for (int i = 0; i < TileCount; i++)
    {
        TrianglesData.push_back(TileData());

        TrianglesData[i].Index = i;

        TrianglesData[i].Points.push_back(somePositions[someTrianglePoints[i].x]); // Point One
        TrianglesData[i].Points.push_back(somePositions[someTrianglePoints[i].y]); // Point Two
        TrianglesData[i].Points.push_back(somePositions[someTrianglePoints[i].z]); // Point Three

        TrianglesData[i].Center = CU::Vec3f(  // Formula from https://brilliant.org/wiki/triangles-centroid/
            { (TrianglesData[i].Points[0].x + TrianglesData[i].Points[1].x + TrianglesData[i].Points[2].x) / 3 },
            { (TrianglesData[i].Points[0].y + TrianglesData[i].Points[1].y + TrianglesData[i].Points[2].y) / 3 },
            { (TrianglesData[i].Points[0].z + TrianglesData[i].Points[1].z + TrianglesData[i].Points[2].z) / 3 });

#ifdef _DEBUG
        { // From Pos, To Pos and Color
            LineCommand command;
            command.myColor = { 1.f, 1.f, 1.f };

            command.myFromPosition = somePositions[someTrianglePoints[i].x];
            command.myToPosition = somePositions[someTrianglePoints[i].y];
            LineDrawer->AddStaticLineCommand(command); // Line: point one >> point two

            command.myFromPosition = somePositions[someTrianglePoints[i].y];
            command.myToPosition = somePositions[someTrianglePoints[i].z];
            LineDrawer->AddStaticLineCommand(command); // Line: point two >> point three

            command.myFromPosition = somePositions[someTrianglePoints[i].z];
            command.myToPosition = somePositions[someTrianglePoints[i].x];
            LineDrawer->AddStaticLineCommand(command); // Line: point three >> point one
        }
#endif // _DEBUG
    }

    for (int i = 0; i < TrianglesData.size(); i++)
    {
        int neighbors = 0;
        bool hasSearchedAll = false;
        bool isConnected = false;

        //Removed unnecessary loop UwU
        for (int y = i; y < TrianglesData.size(); y++)
        {
            if (i == y)
                continue;

            if (someTrianglePoints[i].x == someTrianglePoints[y].x
                || someTrianglePoints[i].x == someTrianglePoints[y].y
                || someTrianglePoints[i].x == someTrianglePoints[y].z)
            {
                if (someTrianglePoints[i].z == someTrianglePoints[y].x
                    || someTrianglePoints[i].z == someTrianglePoints[y].y
                    || someTrianglePoints[i].z == someTrianglePoints[y].z)
                {
                    isConnected = true;
                }
            }
            if (someTrianglePoints[i].y == someTrianglePoints[y].x
                || someTrianglePoints[i].y == someTrianglePoints[y].y
                || someTrianglePoints[i].y == someTrianglePoints[y].z)
            {
                if (someTrianglePoints[i].x == someTrianglePoints[y].x
                    || someTrianglePoints[i].x == someTrianglePoints[y].y
                    || someTrianglePoints[i].x == someTrianglePoints[y].z)
                {
                    isConnected = true;
                }
            }
            if (someTrianglePoints[i].z == someTrianglePoints[y].x
                || someTrianglePoints[i].z == someTrianglePoints[y].y
                || someTrianglePoints[i].z == someTrianglePoints[y].z)
            {
                if (someTrianglePoints[i].y == someTrianglePoints[y].x
                    || someTrianglePoints[i].y == someTrianglePoints[y].y
                    || someTrianglePoints[i].y == someTrianglePoints[y].z)
                {
                    isConnected = true;
                }
            }

            if (isConnected)
            {
                neighbors++;
                TrianglesData[i].NeighborsIndex.push_back(y);
                TrianglesData[y].NeighborsIndex.push_back(i);

                isConnected = false;
            }

            if (neighbors == 3)
                break;
        }

    }

    //DebugDrawer* lineDrawer = nullptr;
    //lineDrawer = &Context::Get()->myGraphicsEngine->GetDebugDrawer();

    //for (auto& triangle : TrianglesData)
    //{
    //	for (auto& neighbour : triangle.NeighborsIndex)
    //	{
    //		LineCommand command;

    //		float colorCode = (float)triangle.Index / ((float)TrianglesData.size() * 5.f);

    //		command.myColor = { colorCode * 5, 1, 1 };
    //		command.myFromPosition = triangle.Center;
    //		command.myToPosition = TrianglesData[neighbour].Center;

    //		lineDrawer->AddLineCommand(command);

    //	}
    //}

    return;
}

std::vector<int> ISTE::Astar::FindPath(int aStartIndex, int aEndIndex)
{
    std::vector<int> path;

    //SetUp data

    std::vector<TileData*> openSet;
    std::vector<TileData*> closedSet;

    closedSet.push_back(&TrianglesData[aStartIndex]);

    closedSet[0]->Length = 0;
    closedSet[0]->NodeIs = NodeData::Closed;

    for (auto& neighbour : TrianglesData[aStartIndex].NeighborsIndex)
    {
        openSet.push_back(&TrianglesData[neighbour]);
        openSet.back()->BeforeTile = closedSet[0]->Index;
    }


    while (openSet.size())
    {
        if (openSet[0]->Index == aEndIndex)
        {
            closedSet.push_back(openSet[0]);
            break;
        }

        openSet[0]->Length = TrianglesData[openSet[0]->BeforeTile].Length + 1;

        for (auto& neighbour : openSet[0]->NeighborsIndex)
        {
            if (TrianglesData[neighbour].NodeIs == NodeData::Closed)
                continue;

            openSet.push_back(&TrianglesData[neighbour]);
            openSet.back()->BeforeTile = openSet[0]->Index;
        }

        //openSet <- openSet[0].neighbours unllesa UwU neighbours is closed->prev = openset[0]

        openSet[0]->NodeIs = NodeData::Closed;
        closedSet.push_back(openSet[0]);
        openSet.erase(openSet.begin());

    }


    int indexc = aEndIndex;

    while (indexc != aStartIndex)
    {
        path.push_back(indexc);
        indexc = TrianglesData[indexc].BeforeTile;
    }

    //add sometype of algorithm to generate new point "optimal path"

    //


    //depending on above solution and unity export data we may need some type of steering algorithm

    //

    std::reverse(path.begin(), path.end());

    //reset data
    ClearCache();

    return path;
}

void ISTE::Astar::Funneling(const CU::Vec3f& aStart, const CU::Vec3f& anEnd, std::vector<CU::Vec3f>& aPathOut)
{


    CU::Vec3f start = aStart;
    CU::Vec3f end = anEnd;

    auto& triangleData = GetTriangleData();

    // Abort if list is empty.
    if (triangleData.size() <= 0)
    {
        aPathOut.push_back(start);
        return;
    }

    // Return immediately if list only has one entry.
    if (triangleData.size() <= 1)
    {
        aPathOut.push_back(end);
        return;
    }


    std::vector<CU::Vec3f>& path = aPathOut;
    path.push_back(start);

    const size_t size = triangleData.size();
    
    std::vector<CU::Vec3f> leftVertices;
    std::vector<CU::Vec3f> rightVertices;
    leftVertices.resize(size + 1);
    rightVertices.resize(size + 1);

    CU::Vec3f apex = start;
    int left = 1;
    int right = 1;
    int next = 0; // what should next be?

    // Initialise portal vertices.
    for (int i = 0; i < triangleData.size() - 1; i++)
    {
        for (int j = 0; j < triangleData[i].NeighborsIndex.size(); j++)
        {
            if (triangleData[triangleData[i].NeighborsIndex[j]].Index == triangleData[i + 1].Index)
            {
                int k = j + 1 >= triangleData[i].NeighborsIndex.size() ? 0 : j + 1;

                leftVertices[i + 1] = (triangleData[i]).Points[j];
                rightVertices[i + 1] = (triangleData[i]).Points[k];
                break;
            }
        }
    }

    // Initialise portal vertices first point.
    for (int j = 0; j < (triangleData[0]).Points.size(); j++)
    {
        if ((triangleData[0]).Points[j] != leftVertices[1]
            && (triangleData[0]).Points[j] != rightVertices[1])
        {
            leftVertices[0] = (triangleData[0]).Points[j];
            rightVertices[0] = (triangleData[0]).Points[j];
        }
    }

    // Initialise portal vertices last point.
    for (int j = 0; j < (triangleData[triangleData.size() - 1]).Points.size(); j++)
    {
        if ((triangleData[triangleData.size() - 1]).Points[j]
            != leftVertices[triangleData.size() - 1]
            && (triangleData[triangleData.size() - 1]).Points[j]
            != rightVertices[triangleData.size() - 1])
        {
            leftVertices[triangleData.size()]
                = (triangleData[triangleData.size() - 1]).Points[j];
            rightVertices[triangleData.size()]
                = (triangleData[triangleData.size() - 1]).Points[j];
        }
    }

    // Step through channel.
    for (int i = 2; i <= triangleData.size() - 1; i++)
    {
        // If new left vertex is different, process.
        if (leftVertices[i] != leftVertices[left] && i > left)
        {
            CU::Vec3f newSide = leftVertices[i] - apex;

            // If new side does not widen funnel, update.
            if (CU::VectorSign(newSide, leftVertices[left] - apex) > 0)
            {
                // If new side crosses other side, update apex.
                if (CU::VectorSign(newSide, rightVertices[right] - apex) > 0)
                {
                    // Find next vertex.
                    for (int j = next; j <= triangleData.size(); j++)
                    {
                        if (rightVertices[j] != rightVertices[next])
                        {
                            next = j;
                            break;
                        }
                    }

                    path.push_back(rightVertices[right]);
                    apex = rightVertices[right];
                    right = next;
                }
                else
                {
                    left = i;
                }
            }
        }

        // If new right vertex is different, process.
        if (rightVertices[i] != rightVertices[right] && i > right)
        {
            CU::Vec3f newSide = rightVertices[i] - apex;

            // If new side does not widen funnel, update.
            if (CU::VectorSign(newSide, rightVertices[right] - apex) < 0)
            {
                // If new side crosses other side, update apex.
                if (CU::VectorSign(newSide, leftVertices[left] - apex) < 0)
                {
                    // Find next vertex.
                    for (int j = next; j <= triangleData.size(); j++)
                    {
                        if (leftVertices[j] != leftVertices[next])
                        {
                            next = j;
                            break;
                        }
                    }

                    path.push_back(leftVertices[left]);
                    apex = leftVertices[left];
                    left = next;
                }
                else
                {
                    right = i;
                }
            }
        }
    }

    return;
}

void ISTE::Astar::ClearCache()
{
    for (int i = 0; i < TrianglesData.size(); i++)
    {
        TrianglesData[i].Length = INT_MAX;
        TrianglesData[i].BeforeTile = -1;
        TrianglesData[i].NodeIs = NodeData::Unvisited;
    }
}

/*
int ISTE::Astar::ManhattanDistance(TileData aNode, TileData aGoalNode)
{
    return abs(aNode.Center.x - aGoalNode.Center.x) + abs(aNode.Center.y - aGoalNode.Center.y);
}
std::vector<int> ISTE::Astar::FindPath(int aStartIndex, int anEndIndex, int aLengthToCheck)
{
    bool hasFoundTile = false;
    bool hasSameDistance = false;

    std::vector<TileData> chosenTiles;

    for (int i = 0; i < TrianglesData.size(); i++)
    {
        if (TrianglesData[i].Length < MAX_VALUE && TrianglesData[i].Length == aLengthToCheck)
        {
            chosenTiles.push_back(TrianglesData[i]);
            if (chosenTiles.size() > 1)
            {
                bool hasChanged = true;
                while (hasChanged)
                {
                    hasChanged = false;
                    for (int y = 0; y < chosenTiles.size() - 1; y++)
                    {
                        if (ManhattanDistance(chosenTiles[y], TrianglesData[aStartIndex]) == ManhattanDistance(chosenTiles[y + 1], TrianglesData[aStartIndex]))
                        {
                            hasSameDistance = true;
                        }
                        else if (ManhattanDistance(chosenTiles[y], TrianglesData[aStartIndex]) < ManhattanDistance(chosenTiles[y + 1], TrianglesData[aStartIndex]))
                        {
                            std::swap(chosenTiles[y], chosenTiles[y + 1]);
                            hasChanged = true;
                            hasSameDistance = false;
                        }
                    }
                }
            }
        }
    }

    if (hasSameDistance)
    {
        for (int i = 0; i < chosenTiles.size(); i++)
        {
            TrianglesData[chosenTiles[i].Index].NodeIs = NodeData::Closed;
            hasFoundTile = true;

            if (&TrianglesData[chosenTiles[i].Index] == &TrianglesData[anEndIndex])
            {
                std::vector<int> path;
                path.push_back(chosenTiles[i].Index);
                int beforeIndex = TrianglesData[chosenTiles[i].Index].BeforeTile;
                while (TrianglesData[beforeIndex].Length > 0)
                {
                    path.push_back(beforeIndex);
                    beforeIndex = TrianglesData[beforeIndex].BeforeTile;
                }
                path.push_back(aStartIndex);

                return path;
            }

            for (int y = 0; y < chosenTiles[i].NeighborsIndex.size(); y++)
            {
                if (TrianglesData[chosenTiles[i].NeighborsIndex[y]].NodeIs == NodeData::Unvisited || TrianglesData[chosenTiles[i].NeighborsIndex[y]].NodeIs == NodeData::Open)
                {
                    TrianglesData[chosenTiles[i].NeighborsIndex[y]].Length = TrianglesData[chosenTiles[i].Index].Length + 1;
                    TrianglesData[chosenTiles[i].NeighborsIndex[y]].BeforeTile = chosenTiles[i].Index;
                    TrianglesData[chosenTiles[i].NeighborsIndex[y]].NodeIs = NodeData::Open;
                }
            }
        }
    }
    else
    {
        if (!chosenTiles.empty())
        {
            TrianglesData[chosenTiles.back().Index].NodeIs = NodeData::Closed;
            hasFoundTile = true;

            if (&TrianglesData[chosenTiles.back().Index] == &TrianglesData[anEndIndex])
            {
                std::vector<int> path;
                path.push_back(chosenTiles.back().Index);
                int beforeIndex = TrianglesData[chosenTiles.back().Index].BeforeTile;
                while (TrianglesData[beforeIndex].Length > 0)
                {
                    path.push_back(beforeIndex);
                    beforeIndex = TrianglesData[beforeIndex].BeforeTile;
                }
                path.push_back(aStartIndex);

                return path;
            }

            for (int y = 0; y < chosenTiles.back().NeighborsIndex.size(); y++)
            {
                if (TrianglesData[chosenTiles.back().NeighborsIndex[y]].NodeIs == NodeData::Unvisited || TrianglesData[chosenTiles.back().NeighborsIndex[y]].NodeIs == NodeData::Open)
                {
                    TrianglesData[chosenTiles.back().NeighborsIndex[y]].Length = TrianglesData[chosenTiles.back().Index].Length + 1;
                    TrianglesData[chosenTiles.back().NeighborsIndex[y]].BeforeTile = chosenTiles.back().Index;
                    TrianglesData[chosenTiles.back().NeighborsIndex[y]].NodeIs = NodeData::Open;
                }
            }
        }
    }



    if (hasFoundTile)
    {
        return FindPath(aStartIndex, anEndIndex, aLengthToCheck + 1);
    }
    else
    {
        std::vector<int> noPath;

        return noPath;
    }
}
std::vector<int> ISTE::Astar::Start(int aStartIndex, int anEndIndex)
{
    if (aStartIndex == anEndIndex)
    {
        Path.push_back(aStartIndex);
        return Path;
    }

    TrianglesData[aStartIndex].Length = 0;
    TrianglesData[aStartIndex].NodeIs = NodeData::Closed;

    for (int i = 0; i < TrianglesData[aStartIndex].NeighborsIndex.size(); i++)
    {
        TrianglesData[TrianglesData[aStartIndex].NeighborsIndex[i]].Length = TrianglesData[aStartIndex].Length + 1;
        TrianglesData[TrianglesData[aStartIndex].NeighborsIndex[i]].BeforeTile = aStartIndex;
        TrianglesData[TrianglesData[aStartIndex].NeighborsIndex[i]].NodeIs = NodeData::Open;
    }

    return FindPath(aStartIndex, anEndIndex, TrianglesData[aStartIndex].Length + 1);
}
*/