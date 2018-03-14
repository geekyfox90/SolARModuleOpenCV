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

#ifndef SOLARDESCRIPTORMATCHERRADIUSOPENCV_H
#define SOLARDESCRIPTORMATCHERRADIUSOPENCV_H

#include "api/features/IDescriptorMatcher.h"
// Definition of SolARDescriptorMatcherRadiusOpencv Class //
// part of SolAR namespace //

#include "ComponentBase.h"
#include "SolAROpencvAPI.h"
#include <string>
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

namespace SolAR {
using namespace datastructure;
using namespace api::features;
namespace MODULES {
namespace OPENCV {

class SOLAROPENCV_EXPORT_API SolARDescriptorMatcherRadiusOpencv : public org::bcom::xpcf::ComponentBase,
        public api::features::IDescriptorMatcher {
public:
    SolARDescriptorMatcherRadiusOpencv();
    ~SolARDescriptorMatcherRadiusOpencv();
    void unloadComponent () override final;

  DescriptorMatcher::RetCode match(
            SRef<DescriptorBuffer>& desc1,
            SRef<DescriptorBuffer>& desc2,
            std::vector<DescriptorMatch>& matches);

    DescriptorMatcher::RetCode match(
           SRef<DescriptorBuffer>& descriptors1,
           std::vector<SRef<DescriptorBuffer>>& descriptors2,
           std::vector<DescriptorMatch>& matches
        );

    XPCF_DECLARE_UUID("904e64f6-d502-11e7-9296-cec278b6b50a");


private:
    float m_maxDistance;
    cv::FlannBasedMatcher m_matcher;

};

}
}
}  // end of namespace SolAR

#endif // SOLARDESCRIPTORMATCHERRADIUSOPENCV_H