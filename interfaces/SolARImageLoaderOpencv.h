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

#ifndef SOLARIMAGELOADEROPENCV_H
#define SOLARIMAGELOADEROPENCV_H

#include "api/image/IImageLoader.h"

#include "ComponentBase.h"
#include "SolAROpencvAPI.h"
#include <string>


namespace SolAR {
using namespace datastructure;
namespace MODULES {
namespace OPENCV {

class SOLAROPENCV_EXPORT_API SolARImageLoaderOpencv : public org::bcom::xpcf::ComponentBase,
    public api::image::IImageLoader {
public:
    SolARImageLoaderOpencv();
    ~SolARImageLoaderOpencv();
    void unloadComponent () override final;
    FrameworkReturnCode loadImage(const std::string & filename, SRef<Image> & img);

    XPCF_DECLARE_UUID("e42d6526-9eb1-4f8a-bb68-53e06f09609c");

};

}
}
}  // end of namespace SolAR



#endif