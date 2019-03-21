/**
 * @copyright Copyright (c) 2017 B-com http://www.b-com.com/
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "SolARPoseEstimationPlanarPointsOpencv.h"
#include "SolAROpenCVHelper.h"
#include "core/Log.h"
#include "opencv2/calib3d/calib3d.hpp"
#include <opencv2/imgproc.hpp>

XPCF_DEFINE_FACTORY_CREATE_INSTANCE(SolAR::MODULES::OPENCV::SolARPoseEstimationPlanarPointsOpencv);

namespace xpcf  = org::bcom::xpcf;

namespace SolAR {
using namespace datastructure;
namespace MODULES {
namespace OPENCV {

SolARPoseEstimationPlanarPointsOpencv::SolARPoseEstimationPlanarPointsOpencv():ConfigurableBase(xpcf::toUUID<SolARPoseEstimationPlanarPointsOpencv>())
{
    addInterface<api::solver::pose::I3DTransformSACFinderFrom2D3D>(this);
    SRef<xpcf::IPropertyMap> params = getPropertyRootNode();
    params->wrapInteger("minNbInliers", m_minNbInliers);
    params->wrapFloat("reprojErrorThreshold", m_reprojErrorThreshold);

    m_camMatrix.create(3, 3, CV_32FC1);
    m_camDistorsion.create(5, 1, CV_32FC1);

    LOG_DEBUG(" SolARPoseEstimationOpencv constructor");
}

SolARPoseEstimationPlanarPointsOpencv::~SolARPoseEstimationPlanarPointsOpencv(){

}

FrameworkReturnCode SolARPoseEstimationPlanarPointsOpencv::estimate(const std::vector<SRef<Point2Df>> & imagePoints,
                                                                    const std::vector<SRef<Point3Df>> & worldPoints,
                                                                    std::vector<SRef<Point2Df>> &imagePoints_inlier,
                                                                    std::vector<SRef<Point3Df>> &worldPoints_inlier,
                                                                    Transform3Df & pose,
                                                                    const Transform3Df initialPose) {

    std::vector<cv::Point2f> imageCVPoints;
    std::vector<cv::Point2f> worldCVPoints;
    std::vector<cv::Point2f> correctedImageCVPoints;

    if (worldPoints.size()!=imagePoints.size() || worldPoints.size()< 4 ){
        LOG_WARNING("world/image points must be valid ( equal and > to 4)");
        return FrameworkReturnCode::_ERROR_  ; // vector of 2D and 3D points must have same size
    }

    for (int i=0;i<imagePoints.size();++i) {

        Point2Df point2D = *(imagePoints.at(i));
        Point3Df point3D = *(worldPoints.at(i));
        imageCVPoints.push_back(cv::Point2f(point2D.getX(), point2D.getY()));
        worldCVPoints.push_back(cv::Point2f(point3D.getX(), point3D.getY()));
    }

    // undistort 2D points
    cv::undistortPoints(imageCVPoints, correctedImageCVPoints, m_camMatrix, m_camDistorsion);

    // Compute the homography matrix with ransac
    cv::Mat status;
    cv::Mat oHw = findHomography(worldCVPoints, correctedImageCVPoints, cv::RANSAC, m_reprojErrorThreshold, status);

    imagePoints_inlier.clear();
    worldPoints_inlier.clear();
    std::vector<cv::Point2f> tmp_cvImagePoints, tmp_cvWorldPoints;
    for (int i = 0; i < status.rows; ++i)
    {
        if (status.at<uchar>(i, 0) == 1)
        {
            imagePoints_inlier.push_back(imagePoints[i]);
            worldPoints_inlier.push_back(worldPoints[i]);
            tmp_cvImagePoints.push_back(correctedImageCVPoints[i]);
            tmp_cvWorldPoints.push_back(worldCVPoints[i]);
        }
    }

    if (imagePoints_inlier.size() < m_minNbInliers){
        return FrameworkReturnCode::_ERROR_;
    }

    // Refine the homography matrix only with inliers
    oHw = cv::findHomography(tmp_cvWorldPoints, tmp_cvImagePoints);

    // Normalization to ensure that ||c1|| = 1
    double norm = sqrt(oHw.at<double>(0, 0) * oHw.at<double>(0, 0) + oHw.at<double>(1, 0) * oHw.at<double>(1, 0) + oHw.at<double>(2, 0) * oHw.at<double>(2, 0));
    oHw /= norm;

    // 3rd column = cross product between first and second columns
    oHw.col(2)= oHw.col(0).cross(oHw.col(1));

    for (int row = 0; row<3; row++){
        for (int col = 0; col<3; col++){
            pose(row,col) = (float)(oHw.at<double>(row, col));
        }
        pose(row,3) = (float)(oHw.at<double>(row,2));
    }

    pose(3,0)  = 0.0;
    pose(3,1)  = 0.0;
    pose(3,2)  = 0.0;
    pose(3,3)  = 1.0;

    pose = pose.inverse();

    return FrameworkReturnCode::_SUCCESS;
}
void SolARPoseEstimationPlanarPointsOpencv::setCameraParameters(const CamCalibration & intrinsicParams, const CamDistortion & distorsionParams) {

    this->m_camDistorsion.at<float>(0, 0)  = distorsionParams(0);
    this->m_camDistorsion.at<float>(1, 0)  = distorsionParams(1);
    this->m_camDistorsion.at<float>(2, 0)  = distorsionParams(2);
    this->m_camDistorsion.at<float>(3, 0)  = distorsionParams(3);
    this->m_camDistorsion.at<float>(4, 0)  = distorsionParams(4);

    this->m_camMatrix.at<float>(0, 0) = intrinsicParams(0,0);
    this->m_camMatrix.at<float>(0, 1) = intrinsicParams(0,1);
    this->m_camMatrix.at<float>(0, 2) = intrinsicParams(0,2);
    this->m_camMatrix.at<float>(1, 0) = intrinsicParams(1,0);
    this->m_camMatrix.at<float>(1, 1) = intrinsicParams(1,1);
    this->m_camMatrix.at<float>(1, 2) = intrinsicParams(1,2);
    this->m_camMatrix.at<float>(2, 0) = intrinsicParams(2,0);
    this->m_camMatrix.at<float>(2, 1) = intrinsicParams(2,1);
    this->m_camMatrix.at<float>(2, 2) = intrinsicParams(2,2);
}

}
}
}
