#include "SolARGeometricMatchesFilterOpencv.h"
#include "ComponentFactory.h"
#include "SolAROpenCVHelper.h"
#include <set>


using namespace org::bcom::xpcf;
XPCF_DEFINE_FACTORY_CREATE_INSTANCE(SolAR::MODULES::OPENCV::SolARGeometricMatchesFilterOpencv);

namespace SolAR {
using namespace datastructure;
namespace MODULES {
namespace OPENCV {

SolARGeometricMatchesFilterOpencv::SolARGeometricMatchesFilterOpencv()
{
    setUUID(SolARGeometricMatchesFilterOpencv::UUID);
    addInterface<api::features::IMatchesFilter>(this,api::features::IMatchesFilter::UUID, "interface GeometricMatchesFilterOpencv");
}


SolARGeometricMatchesFilterOpencv::~SolARGeometricMatchesFilterOpencv(){

}



void SolARGeometricMatchesFilterOpencv::filter(const std::vector<DescriptorMatch>&inputMatches,
                                               std::vector<DescriptorMatch>&outputMatches,
                                               const std::vector<SRef<Keypoint>>&inputKeyPoints,
                                               std::vector<SRef<Keypoint>>&outputKeyPoints){
    std::vector<uchar> status(inputKeyPoints.size());
        std::vector<cv::Point2f> pts1, pts2;
        // get Align matches
        for (unsigned int i = 0; i<inputMatches.size(); i++) {
            assert(inputMatches[i].getIndexInDescriptorA() < inputKeyPoints.size());
            pts1.push_back(cv::Point2f(inputKeyPoints[inputMatches[i].getIndexInDescriptorA()]->getX(),
                                       inputKeyPoints[inputMatches[i].getIndexInDescriptorA()]->getY()));

            assert(inputMatches[i].getIndexInDescriptorB() < outputKeyPoints.size());
            pts2.push_back(cv::Point2f(outputKeyPoints[inputMatches[i].getIndexInDescriptorB()]->getX(),
                                       outputKeyPoints[inputMatches[i].getIndexInDescriptorB()]->getY()));

        }


        cv::Mat F;
        {
            double minVal, maxVal;
            cv::minMaxIdx(pts1, &minVal, &maxVal);
            F = cv::findFundamentalMat(pts1, pts2, cv::FM_RANSAC, 0.006 * maxVal, 0.99, status); //threshold from [Snavely07 4.1]
        }

        for (unsigned int i = 0; i<status.size(); i++) {
            if (status[i]) {
                   outputMatches.push_back(inputMatches[i]);
            }
        }
}

}
}
}
