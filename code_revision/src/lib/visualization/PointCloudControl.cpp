/******************************************************************************
 * Copyright (C) 2011 by Jerome Maye                                          *
 * jerome.maye@gmail.com                                                      *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the Lesser GNU General Public License as published by*
 * the Free Software Foundation; either version 3 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * Lesser GNU General Public License for more details.                        *
 *                                                                            *
 * You should have received a copy of the Lesser GNU General Public License   *
 * along with this program. If not, see <http://www.gnu.org/licenses/>.       *
 ******************************************************************************/

#include "visualization/PointCloudControl.h"

#include "ui_PointCloudControl.h"

#include <QtGui/QFileDialog>

#include <fstream>

/******************************************************************************/
/* Constructors and Destructor                                                */
/******************************************************************************/

PointCloudControl::PointCloudControl(bool showPoints) :
  mUi(new Ui_PointCloudControl()) {
  mUi->setupUi(this);
  mUi->colorChooser->setPalette(&mPalette);
  connect(&mPalette, SIGNAL(colorChanged(const QString&, const QColor&)),
    this, SLOT(colorChanged(const QString&, const QColor&)));
  connect(&GLView::getInstance().getScene(), SIGNAL(render(GLView&, Scene&)),
    this, SLOT(render(GLView&, Scene&)));
  setPointColor(Qt::gray);
  setPointSize(1.0);
  setShowPoints(showPoints);
  setSmoothPoints(true);
  setLogFilename(mUi->logEdit->text());
}

PointCloudControl::~PointCloudControl() {
  delete mUi;
}

/******************************************************************************/
/* Accessors                                                                  */
/******************************************************************************/

void PointCloudControl::setPointColor(const QColor& color) {
  mPalette.setColor("Point", color);
}

void PointCloudControl::setPointSize(double pointSize) {
  mUi->pointSizeSpinBox->setValue(pointSize);
  GLView::getInstance().update();
}

void PointCloudControl::setShowPoints(bool showPoints) {
  mUi->showPointsCheckBox->setChecked(showPoints);
  GLView::getInstance().update();
}

void PointCloudControl::setSmoothPoints(bool smoothPoints) {
  mUi->smoothPointsCheckBox->setChecked(smoothPoints);
  GLView::getInstance().update();
}

void PointCloudControl::setLogFilename(const QString& filename) {
  mUi->logEdit->setText(filename);
  QFileInfo fileInfo(filename);
  if (fileInfo.isFile() && fileInfo.isReadable()) {
    std::ifstream pointCloudFile(filename.toAscii().constData());
    pointCloudFile >> mPointCloud;
    mUi->showPointsCheckBox->setEnabled(true);
    GLView::getInstance().update();
    emit pointCloudRead(mPointCloud);
  }
}

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void PointCloudControl::renderPoints(double size, bool smooth) {
  glPushAttrib(GL_CURRENT_BIT);
  if (size > 1.0)
    glPointSize(size);
  else
    glPointSize(1.0);
  if (smooth)
    glEnable(GL_POINT_SMOOTH);
  else
    glDisable(GL_POINT_SMOOTH);
  glBegin(GL_POINTS);
  GLView::getInstance().setColor(mPalette, "Point");
  for (size_t i = 0; i < mPointCloud.getNumPoints(); ++i)
    glVertex3f(mPointCloud[i](0), mPointCloud[i](1), mPointCloud[i](2));
  glEnd();
  glPointSize(1.0);
  glDisable(GL_POINT_SMOOTH);
  glPopAttrib();
}

void PointCloudControl::logBrowseClicked() {
  QString filename = QFileDialog::getOpenFileName(this,
    "Open Log File", mUi->logEdit->text(),
    "Point cloud log files (*.*)");
  if (!filename.isNull())
    setLogFilename(filename);
}

void PointCloudControl::colorChanged(const QString& role, const QColor& color) {
  GLView::getInstance().update();
}

void PointCloudControl::pointSizeChanged(double pointSize) {
  setPointSize(pointSize);
}

void PointCloudControl::showPointsToggled(bool checked) {
  setShowPoints(checked);
}

void PointCloudControl::smoothPointsToggled(bool checked) {
  setSmoothPoints(checked);
}

void PointCloudControl::render(GLView& view, Scene& scene) {
  if (mUi->showPointsCheckBox->isChecked())
    renderPoints(mUi->pointSizeSpinBox->value(),
      mUi->smoothPointsCheckBox->isChecked());
}
