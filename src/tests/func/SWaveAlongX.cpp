#include <vector>
#include <gtest/gtest.h>


#include "tests/func/util.hpp"

#include "libgcm/Exception.hpp"
#include "libgcm/rheology/Material.hpp"
#include "libgcm/node/CalcNode.hpp"

/*
 * Analytics for the test described in tasks/tests/s-wave-test.xml
 * Sets analytical values for CalcNode object provided
 * It does not take into account waves from border, so it works only for wave axis and not too long
 * For high accuracy node should have coordinates (0; 0; z), where -5 < z < 5
 */
void setSWaveAlongXAnalytics(CalcNode& node, float t, Engine& engine)
{
    // Parameters from task file
    float LEFT_MARK_START = 1.0;
    float RIGHT_MARK_START = 3.0;
    float WAVE_AMPLITUDE_SCALE = 0.01;

    const MaterialPtr& mat = engine.getMaterial("trans-isotropic");
    auto p = mat->getRheologyProperties();
    float rho = mat->getRho();

    if (node.x < -5 || node.x > 5)
        THROW_INVALID_INPUT("X is out of acceptable range");
    if (t < 0 || t > 0.02)
        THROW_INVALID_INPUT("T is out of acceptable range");

    float sWaveYvelocity = sqrt(p.c66 / rho);
    float sWaveZvelocity = sqrt(p.c55 / rho);
    
    float leftMarkY = LEFT_MARK_START - t * sWaveYvelocity;
    float rightMarkY = RIGHT_MARK_START - t * sWaveYvelocity;
	float leftMarkZ = LEFT_MARK_START - t * sWaveZvelocity;
    float rightMarkZ = RIGHT_MARK_START - t * sWaveZvelocity;

    node.vx = node.vy = node.vz = 0;
    node.sxx = node.sxy = node.sxz = node.syy = node.syz = node.szz = 0;

    if (node.x >= leftMarkY && node.x <= rightMarkY) {
        node.vy = sWaveYvelocity * WAVE_AMPLITUDE_SCALE;
        node.sxy = p.c66 * WAVE_AMPLITUDE_SCALE;
    }
    
    if (node.x >= leftMarkZ && node.x <= rightMarkZ) {
		node.vz = sWaveZvelocity * WAVE_AMPLITUDE_SCALE;
		node.sxz = p.c55 * WAVE_AMPLITUDE_SCALE;
    }
}

TEST(Waves, SWaveAlongX)
{
    // Major test parameters

    // Number of time steps
    int STEPS = 20;

    // Check values and draw graphs along this line
    SnapshotLine line;
    line.startPoint = {-5.0, 0.0, 0.0};
    line.endPoint = {5.0, 0.0, 0.0};
    line.numberOfPoints = 100;

    // Thresholds
    float ALLOWED_VALUE_DEVIATION_PERCENT = 0.1;
    int ALLOWED_NUMBER_OF_BAD_NODES = 8; // 2 fronts x 2 nodes per front x 2 without any reason

    runTaskAsTest("tasks/demos/anisotropy-veryfication/s-wave-along-x.xml", setSWaveAlongXAnalytics, STEPS, line, { "vy", "vz", "sxy", "sxz" }, ALLOWED_VALUE_DEVIATION_PERCENT, ALLOWED_NUMBER_OF_BAD_NODES);
}
