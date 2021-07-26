#include "libgcm/util/areas/AberratorArea.hpp"

using namespace gcm;

AberratorArea::AberratorArea( float _minX, float _maxX, float _minY, float _maxY, float _flatZ, std::vector<float> &_curve )
{
    minX = _minX;
    maxX = _maxX;
    minY = _minY;
    maxY = _maxY;
    flatZ = _flatZ;
    curve = _curve;
};

AberratorArea::~AberratorArea() {

}

float AberratorArea::getCurveForX(float x) const {
    int x0 = (int)( (x - minX) / (maxX - minX) * curve.size() );
    if (x0 >= curve.size() - 1)
        return curve[curve.size() - 1];
    float dx = (x - minX) / (maxX - minX) * curve.size() - (float)x0;
    return curve[x0] + dx * (curve[x0 + 1] - curve[x0]);
}

bool AberratorArea::isInArea( const Node& cur_node ) const
{
    if (        cur_node.coords[0] > maxX ||
                cur_node.coords[0] < minX ||
                cur_node.coords[1] > maxY ||
                cur_node.coords[1] < minY)
        return false;
    float z = getCurveForX(cur_node.coords[0]);
    if (       (     z < cur_node.coords[2] && cur_node.coords[2] < flatZ )
            || ( flatZ < cur_node.coords[2] && cur_node.coords[2] < z     )     )
        return true;
    return false;
};
