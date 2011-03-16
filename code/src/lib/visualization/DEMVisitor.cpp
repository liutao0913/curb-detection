#include "DEMVisitor.h"

#include "DEM.h"
#include "Cell.h"

#include <GL/gl.h>

#include <iostream>
#include <fstream>
#include <map>

#include <stdint.h>
#include <stdlib.h>
#include <math.h>

using namespace std;

DEMVisitor::DEMVisitor() {
}

DEMVisitor::~DEMVisitor() {
}

void DEMVisitor::read(istream& stream) {
}

void DEMVisitor::write(ostream& stream) const {
}

void DEMVisitor::read(ifstream& stream) {
}

void DEMVisitor::write(ofstream& stream) const {
}

ostream& operator << (ostream& stream,
  const DEMVisitor& obj) {
  obj.write(stream);
  return stream;
}

istream& operator >> (istream& stream,
  DEMVisitor& obj) {
  obj.read(stream);
  return stream;
}

ofstream& operator << (ofstream& stream,
  const DEMVisitor& obj) {
  obj.write(stream);
  return stream;
}

ifstream& operator >> (ifstream& stream,
  DEMVisitor& obj) {
  obj.read(stream);
  return stream;
}

void DEMVisitor::visit(const DEM* dem) {
  GLuint glListIdx = glGenLists(1);
  glNewList(glListIdx, GL_COMPILE);
  glPointSize(2.0);
  const vector<Cell>& cellsVector = dem->getCellsVector();
  const vector<uint32_t>& labelsVector = dem->getLabelsVector();
  map<uint32_t, Color>::iterator it;
  srand(time(NULL));
  map<uint32_t, Color> colorMap;
  for (uint32_t i = 0; i < labelsVector.size(); i++) {
    it = colorMap.find(labelsVector[i]);
    if (it == colorMap.end()) {
      Color color;
      color.mRedColorByte = round((double)rand() / (double)RAND_MAX * 255.0);
      color.mGreenColorByte = round((double)rand() / (double)RAND_MAX * 255.0);
      color.mBlueColorByte = round((double)rand() / (double)RAND_MAX * 255.0);
      colorMap[labelsVector[i]] = color;
    }
  }
  for (uint32_t i = 0; i < cellsVector.size(); i++) {
    glColor3f(colorMap[labelsVector[i]].mRedColorByte,
      colorMap[labelsVector[i]].mGreenColorByte,
      colorMap[labelsVector[i]].mBlueColorByte);
    cellsVector[i].accept(*this);
  }
  glEndList();
  mGLListVector.push_back(glListIdx);
}

void DEMVisitor::visit(const Cell* cell) const {
  if (cell->mu32PointsNbr != 0) {
    glBegin(GL_POLYGON);
    glVertex3f(cell->mf64CenterX + cell->mf64CellSizeX / 2.0,
      cell->mf64CenterY + cell->mf64CellSizeY / 2.0, cell->mf64HeightMean);
    glVertex3f(cell->mf64CenterX + cell->mf64CellSizeX / 2.0,
      cell->mf64CenterY - cell->mf64CellSizeY / 2.0, cell->mf64HeightMean);
    glVertex3f(cell->mf64CenterX - cell->mf64CellSizeX / 2.0,
      cell->mf64CenterY - cell->mf64CellSizeY / 2.0, cell->mf64HeightMean);
    glVertex3f(cell->mf64CenterX - cell->mf64CellSizeX / 2.0,
      cell->mf64CenterY + cell->mf64CellSizeY / 2.0, cell->mf64HeightMean);
    glEnd();
  }
}