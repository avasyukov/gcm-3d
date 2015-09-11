/*
 * File:   ContactCalculator.h
 * Author: anganar
 *
 * Created on April 30, 2013, 3:18 PM
 */

#ifndef GCM_CONTACT_CALCULATOR_H_
#define GCM_CONTACT_CALCULATOR_H_  1

#include <string>
#include <vector>

#include "libgcm/rheology/RheologyMatrix.hpp"
#include "libgcm/node/CalcNode.hpp"


namespace gcm
{
    class CalcNode;
    /*
     * Base class for inner points calculators
     */
    class ContactCalculator {
    protected:
    	std::string type;
    public:
        /*
         * Destructor
         */
        virtual ~ContactCalculator() = 0;
        /*
         * Calculate next state for the given node
         */
        virtual void doCalc(CalcNode& cur_node, CalcNode& new_node, CalcNode& virt_node,
                            RheologyMatrixPtr matrix, std::vector<CalcNode>& previousNodes, bool inner[],
                            RheologyMatrixPtr virt_matrix, std::vector<CalcNode>& virtPreviousNodes, bool virt_inner[],
                            float outer_normal[], float scale) = 0;
        /*
         * Returns type of the calculator
         */
        std::string getType();

    };

}

#endif    /* GCM_CONTACT_CALCULATOR_H_ */
