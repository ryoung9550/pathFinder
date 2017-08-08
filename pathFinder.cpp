#include <graph.h>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <vector>
#include <deque>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDLShapes.hpp>
	
const int gridSize = 100;
const int screenWidth = 500;
const int w = screenWidth / gridSize; // square width
const unsigned CPS = 1000; // Cycles per second
const Uint32 cycleTime = 1000 / CPS; // Cycle time in milliseconds
const int heuristicStrength = 8;
const int movingCost = 25;
const int wallPercent = 10;

int gridIndex(const unsigned& i, const unsigned& j) {
	return ( i + j * gridSize);
}

int getX(const unsigned& gridIndex) {
	return (gridIndex % gridSize);
}

int getY(const unsigned& gridIndex) {
	return (gridIndex / gridSize);
}

void DrawRect(SDL_Surface* surf, const int& x, const int& y, const int& w, const int& h, const sp::RGB8 & color) {
	SDL_FillRect(surf, new SDL_Rect{x, y, w, h}, (Uint32) SDL_MapRGB(surf->format, color.R, color.G, color.B));
}

int heuristicScore(gr::Node<int>* current, gr::Node<int>* end) {
	int x1 = getX(current->getData());
	int y1 = getY(current->getData());
	int x2 = getX(end->getData());
	int y2 = getY(end->getData());
	int xDif = std::abs(x1 - x2);
	int yDif = std::abs(y1 - y2);
	//int score = xDif + yDif;
	//return 0;
	int score = (xDif * xDif + yDif * yDif) / heuristicStrength;
	return score;
}

bool isInSet(gr::Node<int>* node, std::vector<gr::Node<int>*> set) {
	for(unsigned i = 0; i < set.size(); ++i) {
		if(node == set[i])
			return true;
	}
	return false;
}

bool isInSet(gr::Node<int>* node, std::deque<gr::Node<int>*> set) {
	for(unsigned i = 0; i < set.size(); ++i) {
		if(node == set[i])
			return true;
	}
	return false;
}

int main() {

	srand(time(0));

	// Setup
	SDL_Init(SDL_INIT_EVERYTHING);
	gr::Graph<int, int> grid;
	std::map<int, gr::Node<int>*> allNodes;
	std::map<int, int> cameFrom;
	std::map<int, int> gScore;
	std::map<int, int> fScore;
	std::deque<gr::Node<int>*> openSet;
	std::vector<gr::Node<int>*> closedSet;

	// Initialize the grid with nodes
	for(int i = 0; i < gridSize * gridSize; ++i) {
		//int random = rand() % 100;
		//if(random > wallPercent) {
			auto newNode = new gr::Node<int>(i);
			grid.addNode(newNode, i);
			allNodes[i] = newNode;
		//}
	}

	// Initialize the node's neighbors
	for(auto it = allNodes.begin(); it != allNodes.end(); it++) {
		int x = getX(it->second->getData());
		int y = getY(it->second->getData());
		gr::Node<int>* node = grid.getNode(it->second->getData());
		if(y > 0) { node->addNeighbors(grid.getNode(gridIndex(x, y - 1))); } // Top case
		if(x > 0) { node->addNeighbors(grid.getNode(gridIndex(x - 1, y))); } // Left case
		if(y < gridSize - 1) { node->addNeighbors(grid.getNode(gridIndex(x, y + 1))); } // Bottom case
		if(x < gridSize - 1) { node->addNeighbors(grid.getNode(gridIndex(x + 1, y))); } // Right case
	}

	gr::Node<int>* endNode = grid.getNode(gridSize * gridSize - 1);
	gr::Node<int>* startNode = grid.getNode(50*100 + 50);
	openSet.push_back(startNode); // Put top left node into the open set
	gScore[0] = 0;
	fScore[0] = heuristicScore(startNode, endNode);


	SDL_Window* window = SDL_CreateWindow("Path Finder!", 
	                               SDL_WINDOWPOS_UNDEFINED,
								   SDL_WINDOWPOS_UNDEFINED,
								   screenWidth,
								   screenWidth,
								   0);

	if(window == nullptr) {
		std::cout << "ERROR: Window did not initialize! " << SDL_GetError() << '\n';
		return 1;
	}

	SDL_Surface* winSurf = SDL_GetWindowSurface(window);
	SDL_Surface* bufSurf = SDL_ConvertSurface(winSurf, winSurf->format, 0);


	bool running = true;
	bool search = true;

	while(running) {

		Uint32 startTicks = SDL_GetTicks();


		gr::Node<int>* current;

		if(SDL_FillRect(bufSurf, NULL, 0)) { std::cout << "Screen buffer could not be cleared: " << SDL_GetError() << '\n'; }
		if(SDL_FillRect(winSurf, NULL, 0)) { std::cout << "Screen buffer could not be cleared: " << SDL_GetError() << '\n'; }

		// Check for interupts
		SDL_Event e;
		while(SDL_PollEvent(&e)) {
			if(e.type == SDL_QUIT) {
				running = false;
			}
		}

		// Update
		if(!openSet.empty() && search) {
			for(unsigned i = 0; i < openSet.size(); ++i) {
				int index;
				if( i == 0 ) {
					current = openSet[i];
					index = i;
				}
				if(fScore[openSet[i]->getData()] < fScore[current->getData()]) {
					current = openSet[i];
					index = i;
				}
				if( i == openSet.size() - 1 ) {
					openSet.erase(openSet.begin() + index);
				}
			}
			if(current == endNode) {
				search = false;	
			}

			closedSet.push_back(current);

			std::vector<gr::Node<int>*> currentNeighbors = current->getNeighbors();
			for(unsigned i = 0; i < currentNeighbors.size(); ++i) {
				if(isInSet(currentNeighbors[i], closedSet)) {
					continue;
				}
				if(!isInSet(currentNeighbors[i], openSet)) {
					openSet.push_back(currentNeighbors[i]);
					gScore[currentNeighbors[i]->getData()] = 0x7fff;
				}
				int tempScore = gScore[current->getData()] + movingCost;
				if(tempScore >=	gScore[currentNeighbors[i]->getData()]) {
					continue;
				}
				cameFrom[currentNeighbors[i]->getData()] = current->getData();
				gScore[currentNeighbors[i]->getData()] = tempScore;
				fScore[currentNeighbors[i]->getData()] = tempScore + heuristicScore(currentNeighbors[i], endNode);
			}
		}


		// Clear buffer Surface


		// Draw Squares
		for(auto it = allNodes.begin(); it != allNodes.end(); it++) {
			int nodeData = it->second->getData();
			int x = getX(nodeData) * w;
			int y = getY(nodeData) * w;
			DrawRect(bufSurf, x, y, w, w, sp::White);
		}
		for(unsigned i = 0; i < openSet.size(); ++i) {
			int pos = openSet[i]->getData();
			int x = getX(pos) * w;
			int y = getY(pos) * w;
			DrawRect(bufSurf, x, y, w, w, sp::Green);
		}
		for(unsigned i = 0; i < closedSet.size(); ++i) {
			int pos = closedSet[i]->getData();
			int x = getX(pos) * w;
			int y = getY(pos) * w;
			DrawRect(bufSurf, x, y, w, w, sp::Red);
		}
		
		int pos = current->getData();
		int x = getX(pos) * w;
		int y = getY(pos) * w;
		DrawRect(bufSurf, x, y, w, w, sp::Blue);

		pos = endNode->getData();
		x = getX(pos) * w;
		y = getY(pos) * w;
		DrawRect(bufSurf, x, y, w, w, sp::Magenta);

		if(!search) {
			gr::Node<int>* pathNode = endNode;
			while(true){
				int pos = pathNode->getData();
				int x = getX(pos) * w;
				int y = getY(pos) * w;
				DrawRect(bufSurf, x, y, w, w, sp::Blue);

				if(pathNode ==startNode) { break; }	

				//Get next Node
				pathNode = grid.getNode(cameFrom[pathNode->getData()]);

			}
		}
		
		// Draw Grid
		for(int i = 1; i < gridSize; ++i) {
			unsigned val = i * w;
			sp::DrawLine(bufSurf, val, 0, val, screenWidth, sp::Black);
			sp::DrawLine(bufSurf, 0, val, screenWidth, val, sp::Black);
		}

		// Update Screen

		SDL_BlitSurface(bufSurf, NULL, winSurf, NULL);
		SDL_UpdateWindowSurface(window);

		Uint32 currentTicks = SDL_GetTicks();
		Uint32 tickCycle = currentTicks - startTicks;
		if(tickCycle < cycleTime)
			SDL_Delay(cycleTime - tickCycle);

	}


	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
