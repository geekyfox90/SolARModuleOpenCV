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

#include <iostream>
#include <string>
#include <vector>

// ADD COMPONENTS HEADERS HERE

#include "SolARSVDTriangulationOpencv.h"
#include "SolARImageLoaderOpencv.h"
#include "SolARCameraOpencv.h"
#include "SolARKeypointDetectorOpencv.h"
#include "SolARDescriptorsExtractorSIFTOpencv.h"
//#include "SolARDescriptorMatcherKNNOpencv.h"
#include "SolARDescriptorMatcherRadiusOpencv.h"
#include "SolARImageViewerOpencv.h"
#include "SolARSideBySideOverlayOpencv.h"
#include "SolARBasicMatchesFilterOpencv.h"
#include "SolARGeometricMatchesFilterOpencv.h"

#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/xfeatures2d/cuda.hpp"
#include "opencv2/opencv_modules.hpp"
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/xfeatures2d.hpp>

using namespace SolAR;
using namespace SolAR::datastructure;
using namespace SolAR::api;
using namespace SolAR::MODULES::OPENCV;

namespace xpcf  = org::bcom::xpcf;

void load_2dpoints(std::string&path_file, int points_no, std::vector<SRef<Point2Df>>&pt2d){
    std::ifstream ox(path_file);
    float pt[2];
  //  Point2Df point_temp;
    std::string dummy;
    pt2d.resize(points_no);
    float v[2];
    for(int i = 0; i < points_no; ++i){
       ox>>dummy;
       v[0]  = std::stof(dummy);
       ox>>dummy;
       v[1]= std::stof(dummy);
       pt2d[i]  = sptrnms::make_shared<Point2Df>(v[0], v[1]);
    }
  ox.close();
}


void load_pose(std::string &path_file, SRef<Pose>&P){
    std::ifstream ox(path_file);

    Eigen::Matrix3f Rpose;
    Eigen::Vector3f Tpose;
    P = sptrnms::make_shared<Pose>();

    float v;
    std::string dummy;
    for(int i = 0; i < 3; ++i){
        for(int j = 0; j < 4; ++j){

            ox>>dummy;
            /*
            v = std::stof(dummy);
            if(j < 3){
                Rpose(i,j) = v;
            }
            if(j>=3){
                Tpose[i] = v;
            }
            */
            P->m_poseTransform(i,j) = std::stof(dummy);
        }
    }

    P->m_poseTransform(3,0) = 0.0;
    P->m_poseTransform(3,1) = 0.0;
    P->m_poseTransform(3,2) = 0.0;
    P->m_poseTransform(3,3) = 1.0;

    for(int i = 0; i < 4; ++i){
        for(int j = 0; j < 4; ++j){
            std::cout<<P->m_poseTransform(i,j)<<" ";
        }
        std::cout<<std::endl;
    }
   ox.close();
}

void load_camMatrix(std::string &path_file, CamCalibration&intrinsic){
    std::ifstream ox(path_file);
    std::string dummy;
    float v;
    for(int i = 0; i < 3; ++i){
        for(int j = 0; j < 3; ++j){
            ox>>dummy;
            v = std::stof(dummy);
            intrinsic(i,j) = v;
            std::cout<<intrinsic(i,j)<<" ";
        }
        std::cout<<std::endl;
    }
    ox.close();
}

void load_distorsion(std::string&path_file, CamDistortion&dist){
     std::ifstream ox(path_file);
    std::string dummy;
    float v;
    for(int i = 0; i < 4; ++i){
        ox>>dummy;
        v = std::stof(dummy);
        dist(i,0) = v;
    }
    dist(4,0) = 0.0;
    std::cout<<dist<<std::endl;

}

int run()
{

 // declarations
    xpcf::utils::uuids::string_generator    gen;
    SRef<image::IImageLoader>               imageLoader;
    SRef<display::IImageViewer>             imageViewer;
    SRef<display::ISideBySideOverlay>       overlay;
    SRef<solver::map::ITriangulator>        mapper;




    // The escape key to exit the sample
    char escape_key = 27;

 // component creation
    xpcf::ComponentFactory::createComponent<SolARImageLoaderOpencv>(gen(image::IImageLoader::UUID ), imageLoader);
    xpcf::ComponentFactory::createComponent<SolARSideBySideOverlayOpencv>(gen(display::ISideBySideOverlay::UUID ), overlay);

    xpcf::ComponentFactory::createComponent<SolARImageViewerOpencv>(gen(display::IImageViewer::UUID ), imageViewer);
    xpcf::ComponentFactory::createComponent<SolARSVDTriangulationOpencv>(gen(solver::map::ITriangulator::UUID ), mapper);



   std::string path_pt1 = "./pt1_F.txt";
   std::string path_pt2 = "./pt2_F.txt";
   std::string path_pose1 = "./pose_1.txt";
   std::string path_pose2 = "./pose_2.txt";
   std::string path_intrinsic = "./K.txt";
   std::string path_distorsion = "./dist.txt";

   std::cout<<" wthat's happenning here.."<<std::endl;

   std::vector<SRef<Point2Df>>pt2d_1;
   std::vector<SRef<Point2Df>>pt2d_2;
   std::vector<SRef<Point3Df>>pt3d;

   SRef<Pose> pose_1;
   SRef<Pose> pose_2 = xpcf::utils::make_shared<Pose>();
   CamCalibration K;
   CamDistortion dist;

      const int points_no = 6703;
      std::cout<<"->load points 2d view 1: "<<std::endl;
      load_2dpoints(path_pt1, points_no, pt2d_1);

      std::cout<<"->load points 2d view 2: "<<std::endl;
      load_2dpoints(path_pt2, points_no, pt2d_2);

	  std::cout << "-> load Pose view 1: " << std::endl;
	  load_pose(path_pose1, pose_1);

	  std::cout << "-> load Pose view 2: " << std::endl;
	  load_pose(path_pose2, pose_2);

     std::cout<<"->load K: "<<std::endl;
      load_camMatrix(path_intrinsic,K);

      std::cout<<"->load K: "<<std::endl;
      load_distorsion(path_distorsion,dist);

      mapper->triangulate(pt2d_1, pt2d_2,pose_1,pose_2,K,dist,pt3d);


      std::ofstream log_cloud("./solar_cloud.txt");
      log_cloud<<pt3d.size()<<std::endl;
      for(int k = 0; k < pt3d.size(); ++k){
          log_cloud<<pt3d[k]->getX()<<" "<<pt3d[k]->getY()<<" "<<pt3d[k]->getZ()<<std::endl;
      }
      log_cloud.close();

	  return(0);
}

int main(){
	run();
	std::cout << "\n\n\nHit return key, please\n";
	getchar();
}
