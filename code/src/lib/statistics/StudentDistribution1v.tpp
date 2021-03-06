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

#include "statistics/ChiSquareDistribution.h"
#include "statistics/NormalDistribution.h"
#include "functions/LogGammaFunction.h"
#include "functions/GammaFunction.h"
#include "functions/IncompleteBetaFunction.h"
#include "exceptions/BadArgumentException.h"
#include "exceptions/InvalidOperationException.h"

/******************************************************************************/
/* Constructors and Destructor                                                */
/******************************************************************************/

StudentDistribution<1>::StudentDistribution(double degrees, double location,
    double scale) :
    mLocation(location) {
  setScale(scale);
  setDegrees(degrees);
}

StudentDistribution<1>::StudentDistribution(const StudentDistribution& other) :
    mLocation(other.mLocation),
    mScale(other.mScale),
    mDegrees(other.mDegrees),
    mInverseScale(other.mInverseScale),
    mNormalizer(other.mNormalizer) {
}

StudentDistribution<1>& StudentDistribution<1>::operator =
    (const StudentDistribution& other) {
  if (this != &other) {
    mLocation = other.mLocation;
    mScale = other.mScale;
    mDegrees = other.mDegrees;
    mInverseScale = other.mInverseScale;
    mNormalizer = other.mNormalizer;
  }
  return *this;
}

StudentDistribution<1>::~StudentDistribution() {
}

/******************************************************************************/
/* Stream operations                                                          */
/******************************************************************************/

void StudentDistribution<1>::read(std::istream& stream) {
}

void StudentDistribution<1>::write(std::ostream& stream) const {
  stream << "location: " << mLocation << std::endl
    << "scale: " << mScale << std::endl
    << "degrees: " << mDegrees;
}

void StudentDistribution<1>::read(std::ifstream& stream) {
}

void StudentDistribution<1>::write(std::ofstream& stream) const {
}

/******************************************************************************/
/* Accessors                                                                  */
/******************************************************************************/

void StudentDistribution<1>::setLocation(double location) {
  mLocation = location;
}

double StudentDistribution<1>::getLocation() const {
  return mLocation;
}

void StudentDistribution<1>::setScale(double scale) {
  if (scale <= 0)
    throw BadArgumentException<double>(scale,
      "StudentDistribution<1>::setScale(): scale must be strictly positive",
      __FILE__, __LINE__);
  mInverseScale = 1.0 / scale;
  mScale = scale;
  computeNormalizer();
}

double StudentDistribution<1>::getScale() const {
  return mScale;
}

void StudentDistribution<1>::setDegrees(double degrees) {
  if (degrees <= 0)
    throw BadArgumentException<double>(degrees,
      "StudentDistribution<1>::setDegrees(): degrees must be strictly positive",
      __FILE__, __LINE__);
  mDegrees = degrees;
  computeNormalizer();
}

double StudentDistribution<1>::getDegrees() const {
  return mDegrees;
}

double StudentDistribution<1>::getInverseScale() const {
  return mInverseScale;
}

void StudentDistribution<1>::computeNormalizer() {
  const LogGammaFunction<double> logGammaFunction;
  mNormalizer = logGammaFunction(mDegrees * 0.5) + 0.5 * log(mDegrees *
    M_PI) + 0.5 * log(mScale) - logGammaFunction(0.5 * (mDegrees + 1));
}

double StudentDistribution<1>::getNormalizer() const {
  return mNormalizer;
}

double StudentDistribution<1>::pdf(const RandomVariable& value) const {
  return exp(logpdf(value));
}

double StudentDistribution<1>::logpdf(const RandomVariable& value) const {
  return -0.5 * (1 + mDegrees) * log(1.0 +
    mahalanobisDistance(value) / mDegrees) - mNormalizer;
}

double StudentDistribution<1>::cdf(const RandomVariable& value) const {
  // TODO: SEEMS TO BE INCORRECT
  const IncompleteBetaFunction<double> incBetaFunction(0.5 * mDegrees, 0.5);
  return 1.0 - 0.5 * incBetaFunction(mDegrees / (value * value + mDegrees));
}

StudentDistribution<1>::RandomVariable StudentDistribution<1>::getSample()
    const {
  static NormalDistribution<1> normalDist;
  static ChiSquareDistribution chi2Dist;
  normalDist.setMean(mLocation);
  normalDist.setVariance(mScale);
  chi2Dist.setDegrees(mDegrees);
  return normalDist.getSample() / sqrt(chi2Dist.getSample() / mDegrees);
}

double StudentDistribution<1>::mahalanobisDistance(const RandomVariable& value)
    const {
  return (value - mLocation) * mInverseScale * (value - mLocation);
}

StudentDistribution<1>::Mean StudentDistribution<1>::getMean() const {
  if (mDegrees > 1)
    return mLocation;
  else
    throw InvalidOperationException("StudentDistribution<1>::getMean(): "
      "degrees must be bigger than 1");
}

StudentDistribution<1>::Median StudentDistribution<1>::getMedian() const {
  return mLocation;
}

StudentDistribution<1>::Mode StudentDistribution<1>::getMode() const {
  return mLocation;
}

StudentDistribution<1>::Variance StudentDistribution<1>::getVariance() const {
  if (mDegrees > 2)
    return mDegrees / (mDegrees - 2) * mScale;
  else
    throw InvalidOperationException("StudentDistribution<1>::getVariance(): "
      "degrees must be bigger than 2");
}
