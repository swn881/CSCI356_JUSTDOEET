#include "Graph.h"

int	Graph::mGrid[TOTAL_NODES] = {
	1, 1, 1, 1, 1,		1, 1, 1, 1, 1, 1, 1, 1,		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,		1, 1, 1, 1, 1, 1, 1, 1,		1, 1, 1, 1, 1,
	1, 0, 0, 0, 0,		1, 0, 1, 1, 1, 0, 0, 1,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		1, 0, 1, 1, 1, 0, 0, 1,		0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		0, 0, 0, 0, 1, 0, 1, 1,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		0, 0, 0, 0, 1, 0, 1, 1,		0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		1, 1, 1, 0, 0, 0, 0, 0,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		1, 1, 1, 0, 0, 0, 0, 0,		0, 0, 0, 0, 1,	
	1, 0, 0, 0, 0,		1, 1, 1, 0, 1, 0, 1, 1,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		1, 1, 1, 0, 1, 0, 1, 1,		0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		1, 0, 0, 0, 1, 0, 1, 1,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		1, 0, 0, 0, 1, 0, 1, 1,		0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		1, 1, 1, 0, 1, 0, 0, 1,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		1, 1, 1, 0, 1, 0, 0, 1,		0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		1, 0, 1, 0, 1, 0, 0, 1,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		1, 0, 1, 0, 1, 0, 0, 1,		0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		1, 0, 1, 0, 0, 0, 1, 1,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		1, 0, 1, 0, 0, 0, 1, 1,		0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		1, 0, 1, 0, 1, 0, 0, 0,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		1, 0, 1, 0, 1, 0, 0, 0, 	0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		1, 0, 0, 0, 0, 1, 1, 1,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		1, 0, 0, 0, 0, 1, 1, 1,		0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		0, 0, 1, 1, 1, 1, 0, 1,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		0, 0, 1, 1, 1, 1, 0, 1,		0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		1, 0, 1, 0, 0, 0, 0, 1,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		1, 0, 1, 0, 0, 0, 0, 1,		0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		1, 0, 0, 0, 1, 0, 1, 1,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		1, 0, 0, 0, 1, 0, 1, 1,		0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		1, 1, 0, 1, 1, 0, 0, 0,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		1, 1, 0, 1, 1, 0, 0, 0,		0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		1, 1, 0, 0, 0, 1, 1, 1,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		1, 1, 0, 0, 0, 1, 1, 1,		0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		0, 0, 0, 1, 0, 0, 0, 1,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		0, 0, 0, 1, 0, 0, 0, 1,		0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		1, 0, 1, 1, 1, 0, 1, 1,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		1, 0, 1, 1, 1, 0, 1, 1,		0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		1, 0, 0, 0, 0, 0, 1, 1,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		1, 0, 0, 0, 0, 0, 1, 1,		0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		1, 0, 0, 1, 1, 0, 1, 1,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		1, 0, 0, 1, 1, 0, 1, 1,		0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		1, 1, 0, 0, 1, 0, 0, 1,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		1, 1, 0, 0, 1, 0, 0, 1,		0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		1, 1, 1, 0, 1, 0, 0, 0,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		1, 1, 1, 0, 1, 0, 0, 0,		0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		1, 0, 1, 0, 0, 0, 1, 1,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		1, 0, 1, 0, 0, 0, 1, 1,		0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		1, 0, 1, 1, 1, 0, 1, 1,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		1, 0, 1, 1, 1, 0, 1, 1,		0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		1, 0, 0, 0, 0, 0, 1, 1,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		1, 0, 0, 0, 0, 0, 1, 1,		0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		0, 0, 1, 1, 1, 0, 1, 1,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		0, 0, 1, 1, 1, 0, 1, 1,		0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		1, 0, 0, 1, 0, 0, 0, 1,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		1, 0, 0, 1, 0, 0, 0, 1,		0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		1, 0, 0, 1, 1, 1, 1, 1,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		1, 0, 0, 1, 1, 1, 1, 1,		0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		1, 0, 1, 1, 0, 0, 0, 1,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		1, 0, 1, 1, 0, 0, 0, 1,		0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		1, 0, 0, 0, 0, 0, 1, 1,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		1, 0, 0, 0, 0, 0, 1, 1,		0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		1, 1, 0, 1, 1, 0, 1, 1,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		1, 1, 0, 1, 1, 0, 1, 1,		0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		1, 1, 0, 1, 1, 0, 1, 1,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		1, 1, 0, 1, 1, 0, 1, 1,		0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		1, 0, 0, 0, 0, 0, 1, 1,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		1, 0, 0, 0, 0, 0, 1, 1,		0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		1, 0, 1, 1, 1, 0, 0, 0,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		1, 0, 1, 1, 1, 0, 0, 0,		0, 0, 0, 0, 1,
	1, 0, 0, 0, 0,		0, 0, 0, 0, 1, 1, 0, 1,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		0, 0, 0, 0, 1, 1, 0, 1,		0, 0, 0, 0, 1,
	1, 1, 1, 1, 1,		1, 1, 1, 1, 1, 1, 1, 1,		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,		1, 1, 1, 1, 1, 1, 1, 1,		1, 1, 1, 1, 1,
};

Graph::Graph()
{
	mDiagonalCost = sqrt(SQUARE_SIZE*SQUARE_SIZE + SQUARE_SIZE*SQUARE_SIZE);
}

Graph::~Graph()
{}

int Graph::getContent(int nodeNum)
{
	return mGrid[nodeNum];
}

Ogre::Vector3 Graph::getPosition(int nodeNum)
{
	// Calculate position of node
	int row		= static_cast<int>(nodeNum/GRID_DIMENSION);
	int column	= nodeNum - row * GRID_DIMENSION;

	Ogre::Vector3 position;
	position.x	= -SQUARE_SIZE*GRID_DIMENSION/2 + SQUARE_SIZE/2 + column*SQUARE_SIZE;
	position.y = 0;
	position.z	= -SQUARE_SIZE*GRID_DIMENSION/2 + SQUARE_SIZE/2 + row*SQUARE_SIZE;

	return position;
}

int Graph::getNode(Ogre::Vector3 position)
{
	int row		= static_cast<int>((position.z + SQUARE_SIZE*GRID_DIMENSION/2) / SQUARE_SIZE);
	int column	= static_cast<int>((position.x + SQUARE_SIZE*GRID_DIMENSION/2) / SQUARE_SIZE);

	int nodeNumber = column + row*GRID_DIMENSION;

	if(mGrid[nodeNumber] > 0)
		return -1;
	else
		return nodeNumber;
}

void Graph::getConnections(int fromNode, Connection connections[])
{
	int row		= static_cast<int>(fromNode/GRID_DIMENSION);
	int column	= static_cast<int>(fromNode - row*GRID_DIMENSION);

	// Connection index numbers around node, n
	// 0 1 2
	// 3 n 4
	// 5 6 7
	int toNode;
	int toRow = row - 1;
	int toColumn = column - 1;

    //upper row
    if (toRow >= 0)
    {
		//upper left
        if (toColumn >= 0)
        {
			toNode = toColumn + toRow * GRID_DIMENSION;
            if (mGrid[toNode] <= 0 && (mGrid[column + toRow * GRID_DIMENSION] <= 0 && mGrid[toColumn + row * GRID_DIMENSION] <= 0))
				connections[0].set(mDiagonalCost, toNode, fromNode);
        }
		//top
		toNode = column + toRow * GRID_DIMENSION;
		if(mGrid[toNode] <= 0)
			connections[1].set(SQUARE_SIZE, toNode, fromNode);
            
		//upper right
		toColumn = column + 1;
		if (toColumn < GRID_DIMENSION)
		{
			toNode = toColumn + toRow * GRID_DIMENSION;
            
			if (mGrid[toNode] <= 0 && (mGrid[column + toRow * GRID_DIMENSION] <= 0 && mGrid[toColumn + row * GRID_DIMENSION] <= 0))
				connections[2].set(mDiagonalCost, toNode, fromNode);
		}
	}

    //left
	toColumn = column - 1;
    
	if (toColumn >= 0)
	{
		toNode = toColumn + row * GRID_DIMENSION;

		if(mGrid[toNode] <= 0)
			connections[3].set(SQUARE_SIZE, toNode, fromNode);
	}

	//right
    toColumn = column + 1;

	if (toColumn < GRID_DIMENSION)
    {
		toNode = toColumn + row * GRID_DIMENSION;

		if(mGrid[toNode] <= 0)
			connections[4].set(SQUARE_SIZE, toNode, fromNode);
	}

	//lower row
    toRow = row + 1;

    if(toRow < GRID_DIMENSION)
    {
		//lower left
        toColumn = column - 1;
        
		if (toColumn >= 0)
        {
			toNode = toColumn + toRow * GRID_DIMENSION;
            
			if (mGrid[toNode] <= 0 && (mGrid[column + toRow * GRID_DIMENSION] <= 0 && mGrid[toColumn + row * GRID_DIMENSION] <= 0))
				connections[5].set(mDiagonalCost, toNode, fromNode);
		}
		//bottom
		toNode = column + toRow * GRID_DIMENSION;

		if(mGrid[toNode] <= 0)
			connections[6].set(SQUARE_SIZE, toNode, fromNode);

		//lower right
		toColumn = column + 1;

		if (toColumn < GRID_DIMENSION)
        {
			toNode = toColumn + toRow * GRID_DIMENSION;
            
			if (mGrid[toNode] <= 0 && (mGrid[column + toRow * GRID_DIMENSION] <= 0 && mGrid[toColumn + row * GRID_DIMENSION] <= 0))
				connections[7].set(mDiagonalCost, toNode, fromNode);
		}
	}
}