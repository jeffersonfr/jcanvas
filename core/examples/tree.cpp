/***************************************************************************
 *   Copyright (C) 2005 by Jeff Ferr                                       *
 *   root@sat                                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "jcanvas/core/japplication.h"
#include "jcanvas/core/jwindow.h"

#include <vector>

using namespace jcanvas;

static int NUMBER_OF_BRANCHES = 0;

class Vector2 { 

  private:

  public:
    int x, y;
  
  public:
    Vector2()
    {
      x = 0;
      y = 0;
    }

    Vector2(int a, int b)
    {
      x = a;
      y = b;
    }
    
    void Set(int a, int b)
    {
      x = a;
      y = b;
    }
    
    void Rotate(float angle_r)
    {
      float 
        _x = static_cast<float>(x),
        _y = static_cast<float>(y),
        s = sinf(angle_r),
        c = cosf(angle_r),
        a = _x * c - _y * s,
        b = _x * s + _y * c;
      
      x = static_cast<int>(a);
      y = static_cast<int>(b);
    }
};

class branch {

  private:

  public:
    Vector2 start;
    Vector2 end;
    int branchLength;
    int generation;
    float rightAngle;
    float leftAngle;
    bool hasChildren;

    int treec1;
    int treec2;
    int treec3;

    int leavesc1;
    int leavesc2;
    int leavesc3;

  public:
    branch createChild(int maxLength, bool dir, int generation, int generations)
    {
      int childGen = generation + 1;
      int length;
      Vector2 drawToPos;

      if (childGen > generations) {
        length = getRand(0, 50);
        drawToPos.Set(0, length);
      }
      else {
        length = getRand(10, maxLength);
        drawToPos.Set(0, length);
      }

      if (childGen > generations) {
        if (dir) {
          leftAngle -= getRandFloat(0.0f, 42.0f*(M_PI / 180.0f));
          drawToPos.Rotate(leftAngle);
        }
        else {
          rightAngle += getRandFloat(0.0f, 42.0f*(M_PI / 180.0f));
          drawToPos.Rotate(rightAngle);
        }
      } else {
        if (dir) { 
          leftAngle -= getRandFloat(0.0f, 21.0f*(M_PI / 180.0f));
          drawToPos.Rotate(leftAngle);
        }
        else {
          rightAngle += getRandFloat(0.0f, 21.0f*(M_PI / 180.0f));
          drawToPos.Rotate(rightAngle);
        }
      }

      drawToPos.x += end.x;
      drawToPos.y = end.y - drawToPos.y;

      if (dir) {
        branch child(end, drawToPos, length, leftAngle, generation, treec1, treec2, treec3, leavesc1, leavesc2, leavesc3, generations);
        return child;
      } else {
        branch child(end, drawToPos, length, rightAngle, generation, treec1, treec2, treec3, leavesc1, leavesc2, leavesc3, generations);
        return child;
      }
    }

    branch(Vector2 start, int length)
    {
      NUMBER_OF_BRANCHES++;

      hasChildren = false;
      branchLength = getRand(length/3, length);
      rightAngle = 0;
      leftAngle = 0;

      generation = 0;

      this->start.Set(start.x, start.y);
      end.Set(start.x, start.y - branchLength);

      treec1 = 0;
      treec2 = 0;
      treec3 = 0;

      leavesc1 = getRand(50, 255);
      leavesc2 = getRand(50, 255);
      leavesc3 = getRand(50, 255);
    }

    branch(Vector2 start, Vector2 end, int length, float angle, int generation, int brac1, int brac2, int brac3, int leavesc1, int leavesc2, int leavesc3, int generations)
    {
      NUMBER_OF_BRANCHES++;

      hasChildren = false;
      branchLength = length;
      rightAngle = angle;
      leftAngle = angle;

      this->generation = generation+1;

      this->start.Set(start.x, start.y);
      this->end.Set(end.x, end.y);

      this->leavesc1 = leavesc1;
      this->leavesc2 = leavesc2;
      this->leavesc3 = leavesc3;

      this->treec1 = 0x80+(leavesc1/14)*generation;
      this->treec2 = 0x80+(leavesc2/14)*generation;
      this->treec3 = 0x80+(leavesc3/14)*generation;

      if (generation > generations) { 
        treec1 = leavesc1;
        treec2 = leavesc2;
        treec3 = leavesc3;
      }
    }

    virtual ~branch()
    {
    }

    int getRand(int rMin, int rMax)
    {
      return rand() % (rMax - rMin + 1) + rMin;
    }

    float getRandFloat(float fMin, float fMax)
    {
      return fMin + (rand()) / ((float)(RAND_MAX / (fMax - fMin)));
    }

};

class Tree : public Window, public KeyListener {

  private:
    std::vector<branch> createTree(Vector2 origin, int maxBranchLength, int generations, int leafLoops) 
    {
      std::vector<branch> 
        tree;
      branch 
        trunk(origin, maxBranchLength);
      
      tree.push_back(trunk);
      
      NUMBER_OF_BRANCHES = 0;

      for (int g = 0; g < generations+leafLoops; g++) {
        int vSize = tree.size();

        for (int x = 0; x < vSize; x++) {
          if (tree[x].hasChildren == false) {
            tree.push_back(tree[x].createChild(maxBranchLength, true, g, generations));
            tree.push_back(tree[x].createChild(maxBranchLength, false, g, generations));
            tree[x].hasChildren = true;
          }
        }
      }

      return tree;
    }

  public:
    Tree():
      Window(jpoint_t<int>{1600, 900})
    {
    }

    virtual ~Tree()
    {
    }

		void Paint(Graphics *g) 
		{
      Window::Paint(g);

      jpoint_t
        size = GetSize();
      
      Vector2 
        origin(size.x / 2, size.y);
      int 
        generations = 5,
        maxBranchLength = 200,
        leafLoops = 6;

      std::vector<branch> 
        tree = createTree(origin, maxBranchLength, generations, leafLoops); 

      for (int i=0; i<(int)tree.size(); i++) {
        branch branch = tree[i];
        
        if (branch.generation == generations) {
          g->SetColor({branch.treec3, branch.treec2, branch.treec1, 255});
          g->DrawLine({branch.start.x, branch.start.y}, {branch.end.x, branch.end.y});
          g->DrawLine({branch.start.x+2, branch.start.y}, {branch.end.x, branch.end.y});
          g->DrawLine({branch.start.x-2, branch.start.y}, {branch.end.x, branch.end.y});
        } else if (branch.generation > generations) {
          g->SetColor({branch.leavesc3, branch.leavesc2, branch.leavesc1, 150}); 
          g->DrawLine({branch.start.x, branch.start.y}, {branch.end.x, branch.end.y});
        } else {
          g->SetColor({branch.treec3, branch.treec2, branch.treec1, 255});
          g->DrawLine({branch.start.x, branch.start.y}, {branch.end.x, branch.end.y}); 
        }
      }
    }
		
    virtual bool KeyPressed(KeyEvent *event) 
		{
      Repaint();

      return true;
		}
    
};

int main(int argc, char* argv[]) 
{
	Application::Init(argc, argv);

	srand(time(nullptr));

	Tree app;

	app.SetTitle("Tree");

	Application::Loop();

	return 0;
}

