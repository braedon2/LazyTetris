#include <iostream>
#include <queue>
#include <vector>
#include "tetris.h"
#include "solver.h"

void setNodeNeighbours(GraphNode& node, Graph& graph, GameGrid& grid) {
    if (not grid.checkCollision(node.tetrimino.move(left).getPositions())) {
        node.neighbours.push_back(&graph.at(node.tetrimino.yDelta).at(node.tetrimino.xDelta - 1));
    }
    if (not grid.checkCollision(node.tetrimino.move(down).getPositions())) {
        node.neighbours.push_back(&graph.at(node.tetrimino.yDelta + 1).at(node.tetrimino.xDelta));
    }
    if (not grid.checkCollision(node.tetrimino.move(right).getPositions())) {
        node.neighbours.push_back(&graph.at(node.tetrimino.yDelta).at(node.tetrimino.xDelta + 1));
    }
}

Graph makeGraph(int width, int height, Tetrimino& tetrimino, GameGrid& grid) {
    Graph graph;
    for (int i = 0; i < height; i++) {
        graph.push_back(std::vector<GraphNode>());
        for (int j = 0; j < width; j++) {
            GraphNode node = {
                Tetrimino(tetrimino.shape, j, i, tetrimino.rotationStep),
                false, // visited
                nullptr, // prev
                std::vector<GraphNode*>() // neighbours
            };
            graph.at(i).push_back(node);
        }
    }

    for (std::vector<GraphNode>& row : graph) {
        for (GraphNode& node : row) {
            setNodeNeighbours(node, graph, grid);
        }
    }
    return graph;
}

std::vector<GraphNode*> search(Graph& graph, Tetrimino& tetrimino, GameGrid& grid) {
    std::queue<GraphNode*> queue;
    std::vector<GraphNode*> results;

    GraphNode& root = graph.at(tetrimino.yDelta).at(tetrimino.xDelta);
    root.visited = true;
    setNodeNeighbours(root, graph, grid);   
    queue.push(&root);

    while (not queue.empty()) {
        GraphNode *node = queue.front();
        queue.pop();

        if (grid.checkCollision(node->tetrimino.move(down).getPositions())) {
            results.push_back(node);
        }

        for (GraphNode *neighbour : node->neighbours) {
            if (not neighbour->visited) {
                neighbour->visited = true;
                neighbour->prev = node;
                queue.push(neighbour);
            }
        }
    }
    return results;
}