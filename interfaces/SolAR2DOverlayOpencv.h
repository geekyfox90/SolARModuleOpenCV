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

#ifndef SOLAR2DOVERLAYOPENCV_H
#define SOLAR2DOVERLAYOPENCV_H
#include <vector>

#include "api/display/I2DOverlay.h"

#include "xpcf/component/ConfigurableBase.h"

#include "SolAROpencvAPI.h"

namespace SolAR {
using namespace datastructure;
namespace MODULES {
namespace OPENCV {

/**
 * @class SolAR2DOverlayOpencv
 * @brief <B>Draws 2D features (circles, lines, etc.) on an image.</B>
 * <TT>UUID: cc51d685-9797-4ffd-a9dd-cec4f367fa6a</TT>
 *
 */

class SOLAROPENCV_EXPORT_API SolAR2DOverlayOpencv : public org::bcom::xpcf::ConfigurableBase,
    public api::display::I2DOverlay
{
public:
    SolAR2DOverlayOpencv();

    /// @brief Draw one Circle.
    /// Draws a circle on the image displayImage center on the point with specified radius and thickness.
    /// @param[in] point The position of the circle to draw
    /// @param[in,out] displayImage The image on which the the circles will be drawn.
    void drawCircle(const SRef<Point2Df> point, SRef<Image> displayImage) override;

    /// @brief Draw Circles.
    /// Draws all the circles stored in the vector std::vector <SRef<Point2Df>> & points on image displayImage with specified radius, thickness and colors (defined in the configuration file).
    /// @param[in] point The positions of the circles to draw
    /// @param[in,out] displayImage The image on which the circles will be drawn.
    void drawCircles(const std::vector<SRef<Point2Df>>& points, SRef<Image> displayImage) override;

    /// @brief Draw Circles.
    /// Draws all the circles stored in the vector std::vector <SRef<Keypoint>> & keypoints on image displayImage with specified radius, thickness and colors (defined in the configuration file).
    /// @param[in] keypoint The positions of the circles to draw
    /// @param[in,out] displayImage The image on which the circles will be drawn.
    void drawCircles(const std::vector<SRef<Keypoint>>& keypoints, SRef<Image> displayImage) override;

    /// @brief Draw a Contour.
    /// Draws a contour on image displayImage
    /// @param[in] contour The contour in 2D to draw with specified radius, thickness and colors (defined in the configuration file).
    /// @param[in,out] displayImage The image on which the contours will be drawn.
    void drawContour (const Contour2Df& contour, SRef<Image> displayImage);

    /// @brief Draw Contours.
    /// Draws all the contours stored in the vector  std::vector <SRef<Contour2Df>> & contours on image displayImage
    /// @param[in] contours The vector of contours in 2D to draw with specified radius, thickness and colors (defined in the configuration file).
    /// @param[in,out] displayImage The image on which the contours will be drawn.
    void drawContours (const std::vector <SRef<Contour2Df>> & contours, SRef<Image> displayImage) override;

    /// @brief Draws a Squared Binary Pattern.
    /// @param[in] pattern The squared binary pattern to display.
    /// @param[in,out] displayImage The image on which the squared binary pattern will be drawn (on the whole image).
    void drawSBPattern (const SRef<SquaredBinaryPattern> pattern, SRef<Image> displayImage) override;

    void unloadComponent () override final;

private:
    /// @brief The thickness of the displayed features (not used for SBPattern)
    unsigned int m_thickness = 1;

    /// @brief The radius of a circle (not used for contours and SBPattern)
    unsigned int m_radius = 5;

    /// @brief The color in BGR format of the displayed features.
    std::vector<unsigned int> m_color = {0,255,0};

    /// @brief if not null, the color will be randomized for each elements
    unsigned int m_randomColor = 0;
};

}
}
}

#endif // SOLAR2DOVERLAYOPENCV_H
