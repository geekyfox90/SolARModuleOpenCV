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

#include "SolARPoseEstimationOpencv.h"
#include "SolAROpenCVHelper.h"
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/calib3d/calib3d.hpp"

#include "ComponentFactory.h"


#include <map>

XPCF_DEFINE_FACTORY_CREATE_INSTANCE(SolAR::MODULES::OPENCV::SolARPoseEstimationOpencv);

namespace SolAR {
using namespace datastructure;
namespace MODULES {
namespace OPENCV {

SolARPoseEstimationOpencv::SolARPoseEstimationOpencv()
{
    setUUID(SolARPoseEstimationOpencv::UUID);
    addInterface<api::solver::pose::IPoseEstimation>(this,api::solver::pose::IPoseEstimation::UUID, "interface api::solver::pose::IPoseEstimation");

    m_camMatrix.create(3, 3, CV_32FC1);
    m_camDistorsion.create(5, 1, CV_32FC1);

    LOG_DEBUG(" SolARPoseEstimationOpencv constructor");
}


FrameworkReturnCode SolARPoseEstimationOpencv::poseFromSolvePNP( Pose & pose, const std::vector<SRef<Point2Df>> & imagePoints, const std::vector<SRef<Point3Df>> & worldPoints) {

    std::vector<cv::Point2f> imageCVPoints;
    std::vector<cv::Point3f> worldCVPoints;

    if (worldPoints.size()!=imagePoints.size())
        return FrameworkReturnCode::_ERROR_; // vector of 2D and 3D points must have same size

    for (int i=0;i<imagePoints.size();++i) {
        Point2Df point2D = *(imagePoints.at(i));
        Point3Df point3D = *(worldPoints.at(i));
        imageCVPoints.push_back(cv::Point2f(point2D.getX(), point2D.getY()));
        worldCVPoints.push_back(cv::Point3f(point3D.getX(), point3D.getY(),point3D.getZ()));
    }

    cv::Mat Rvec;
    cv::Mat_<float> Tvec;
    cv::Mat raux, taux;
    cv::solvePnP(worldCVPoints, imageCVPoints, m_camMatrix, m_camDistorsion, raux, taux );

    raux.convertTo(Rvec, CV_32F);
    taux.convertTo(Tvec, CV_32F);

    cv::Mat_<float> rotMat(3, 3);
    cv::Rodrigues(Rvec, rotMat);

    Eigen::Matrix3f Rpose;
    Eigen::Vector3f Tpose;

    for (int col = 0; col<3; col++){
          for (int row = 0; row<3; row++){
                  Rpose(row,col) = rotMat(row, col);
          }
         Tpose[col] = Tvec(col);
    }

    pose = Pose(Rpose, Tpose);

    return FrameworkReturnCode::_SUCCESS;

}


void SolARPoseEstimationOpencv::setCameraParameters(const CamCalibration & intrinsicParams, const CamDistortion & distorsionParams) {


    //TODO.. check to inverse
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