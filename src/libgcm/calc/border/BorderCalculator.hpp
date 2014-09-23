/*
 * File:   BorderCalculator.h
 * Author: anganar
 *
 * Created on April 30, 2013, 3:18 PM
 */

#ifndef GCM_BORDER_CALCULATOR_H_
#define GCM_BORDER_CALCULATOR_H_  1

#include <string>
#include <vector>
#include <initializer_list>

#include "libgcm/Logging.hpp"
#include "libgcm/rheology/RheologyMatrix.hpp"

#include <boost/lexical_cast.hpp>
#include "launcher/util/xml.hpp"


namespace gcm
{
    class CalcNode;
    /*
     * Base class for inner points calculators
     */
    class BorderCalculator {
    public:
        /*
         * Destructor
         */
        virtual ~BorderCalculator() = 0;
        /*
         * Calculate next state for the given node
         */
        virtual void doCalc(CalcNode& cur_node, CalcNode& new_node, RheologyMatrixPtr matrix,
                            std::vector<CalcNode>& previousNodes, bool inner[],
                            float outer_normal[], float scale) = 0;
        /*
         * Returns type of the calculator
         */
        virtual std::string getType() = 0;
		
		// TODO: we should either 'officially approve' using xml:Node here or implement loaders
		/*
		 * Initializes calculator parameters
		 */
		virtual void setParameters(const xml::Node& params) = 0;

        /*
         * Intended for SPGCM. Sorry for that.
         */
// FIXME these functions are SPGCM-specific
// The same behavior can be achieved by an approproate
// typecast in spgcm code
//        virtual unsigned int getXmlValue() = 0;
//        virtual const char* getXmlName() = 0;
//
//    private:
    };

}

#endif    /* GCM_BORDER_CALCULATOR_H_ */
