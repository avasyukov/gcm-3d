#ifndef CALC_NODE_H_
#define CALC_NODE_H_

#include <vector>
#include <cassert>
#include <iostream>
#include <string>

#include "node/Node.h"
#include "util/Types.h"
#include "Engine.h"

#define GCM_VALUES_SIZE 9
using namespace std;
using namespace gcm;

namespace gcm {

    /**
     * Calculation node implementaion. Contains all necessary information
     * about node used in computations.
     */
    class CalcNode : public ICalcNode {
        friend class DataBus;
        friend class VtuTetrFileReader;
        friend class Vtu2TetrFileReader;
        friend class VTKSnapshotWriter;
        friend class VTK2SnapshotWriter;
        friend class VTKCubicSnapshotWriter;
        friend class VTKMarkeredMeshSnapshotWriter;
        friend class Mesh;

    protected:
        //  public flags container type

        typedef union {
            uchar flags;

            struct {
                uchar contact : 1;
                uchar order : 1;
                uchar flag1 : 1;
                uchar flag2 : 1;
                uchar flag3 : 1;
                uchar flag4 : 1;
                uchar flag5 : 1;
                uchar flag6 : 1;
            };
        } PublicFlags;

        // private flags container

        typedef union {
            uchar flags;

            struct {
                uchar local : 1;
                uchar used : 1;
                uchar border : 1;
                mutable uchar mainStressCalculated : 1;
            };
        } PrivateFlags;

        // error flags container type

        typedef union {
            uchar flags;

            struct {
                uchar xNeigh : 1;
                uchar yNeigh : 1;
                uchar zNeigh : 1;
                uchar outerNormal : 1;
            };
        } ErrorFlags;

        // Node public flags. These flags are to synchrinized using MPI.
        PublicFlags publicFlags;
        // Node private flags. Intended to be for internal use only.
        PrivateFlags privateFlags;
        // Node flags to indicate different issues with this node during calculations
        ErrorFlags errorFlags;

        // border and contac conditions
        // FIXME should these fields be protected?
        uchar borderConditionId;
        uchar contactConditionId;
        // main stress components
        mutable gcm_real mainStresses[3];

        // current material density
        gcm_real rho;
        // node material id
        uchar materialId;

        // crack direction
        // TODO  document it
        gcm_real crackDirection[3];

        // calculates main stress components
        void calcMainStressComponents() const;

        // returns private flags
        uchar getPrivateFlags() const;
        // sets private flags
        void setPrivateFlags(uchar flags);
        // returns public flags
        uchar getPublicFlags() const;
        // sets public flags
        void setPublicFlags(uchar flags);
        // returns error flags
        uchar getErrorFlags() const;
        // sets error flags
        void setErrorFlags(uchar flags);
    public:
        /**
         * Number of pair node in contact. FIXME
         */
        int contactNodeNum;
        /**
         * Contact direction. FIXME
         */
        uchar contactDirection;
        /**
         * Identifier of the body this node belongs to.
         */
        uchar bodyId;

        /**
         * Default constructor. Constructs node with default parameters.
         */
        CalcNode();
        /**
         * Constructor. Creates node with specified number assigned.
         *
         * @param num Number of the node
         */
        CalcNode(int num);
        /**
         * Constructor. Creates node with specified number assigned and set
         * coordinates.
         *
         * @param num Number of the node
         * @param x X coordinate of the node
         * @param y Y coordinate of the node
         * @param z Z coordinate of the node
         */
        CalcNode(int num, gcm_real x, gcm_real y, gcm_real z);
        /**
         * Constructor. Constructs copy of the passed object.
         *
         * @param src Calc node to make copy of.
         */
        CalcNode(const CalcNode& src);
        /**
         * Destructor.
         */
        ~CalcNode();

        /**
         * Assignment operator. Allows creation of calc node copy using
         * assignment statement.
         *
         * @param src Calc node to be copied.
         */
        CalcNode &operator=(const CalcNode &src);

        /**
         * Clears flags state. Drops main stresses and error flags.
         */
        void clearState();
        /**
         * Clears error flags.
         */
        void clearErrorFlags();
        /**
         * Clears main stresses flag.
         */
        void clearMainStresses();

        /**
         * Returns compression.
         *
         * @return Compression value.
         */
        gcm_real getCompression() const;
        /**
         * Returns compression.
         *
         * @return Compression value.
         */
        gcm_real getTension() const;
        /**
         * Returns shear.
         *
         * @return Shear value.
         */
        gcm_real getShear() const;
        /**
         * Returns deviator.
         *
         * @return Deviator value.
         */
        gcm_real getDeviator() const;
        /**
         * Returns pressure.
         *
         * @return Pressure value.
         */
        gcm_real getPressure() const;

        /**
         * Returns tensor J1 invariant. TODO document
         *
         * @return J1 value.
         */
        gcm_real getJ1() const;
        /**
         * Returns tensor J2 invariant. TODO document
         *
         * @return J2 value.
         */
        gcm_real getJ2() const;
        /**
         * Returns tensor J3 invariant. TODO document
         *
         * @return J3 value.
         */
        gcm_real getJ3() const;
        /**
         * Returns main stress components. TODO document
         *
         * @param s1 Variable to put first main stress component in.
         * @param s2 Variable to put second main stress component in.
         * @param s3 Variable to put third main stress component in.
         */
        void getMainStressComponents(gcm_real& s1, gcm_real& s2, gcm_real& s3) const;
        /**
         * TODO document.
         *
         * @param stress TODO document
         * @param vector TODO document
         */
        void calcMainStressDirectionByComponent(gcm_real stress, vector3& vector) const;

        /**
         * Checks if rheology is valid. TODO document
         *
         * @return True in case rheology is valid and false otherwise.
         */
        bool rheologyIsValid() const;

        /**
         * Indicates whether node is in contact or not.
         *
         * @return True in case of contact and false otherwise.
         */
        bool isInContact() const;
        /**
         * Sets new contact state of the node
         *
         * @param value Contact state.
         */
        void setInContact(bool value);

        /**
         * Indicates whether node is local or not.
         *
         * @return True in case of local node and false otherwise.
         */
        bool isLocal() const;
        /**
         * Indicates whether node is remote or not.
         *
         * @return True in case of remote node and false otherwise.
         */
        bool isRemote() const;
        /**
         * Sets node placement to specified value and also marks node as used
         *
         * @param Flag that indicates whether node is local or not
         */
        void setPlacement(bool local);

        /**
         * Indicates whether node is used or not.
         *
         * @return True in case of used node and false otherwise.
         */
        bool isUsed() const;
        /**
         * Sets node used flag.
         *
         * @param value Used flag value
         */
        void setUsed(bool value);

        /**
         * Indicates whether node is first order or not.
         *
         * @return True in case of first order node and false otherwise.
         */
        bool isFirstOrder() const;
        /**
         * Indicates whether node is second order or not.
         *
         * @return True in case of second order node and false otherwise.
         */
        bool isSecondOrder() const;
        /**
         * Sets node order flag.
         *
         * @param order Node order
         */
        void setOrder(uchar order);

        /**
         * Returns whether node is border or not.
         *
         * @return True in case of border node and false otherwise.
         */
        bool isBorder() const;
        /**
         * Returns whether node is inner or not.
         *
         * @return True in case of inner node and false otherwise.
         */
        bool isInner() const;
        /**
         * Sets node border flag.
         *
         * @param value  Border flag value.
         */
        void setIsBorder(bool value);

        /**
         * Sets node xneigh error flag. TODO document
         */
        void setXNeighError();
        /**
         * Sets node yneigh error flag. TODO document
         */
        void setYNeighError();
        /**
         * Sets node zneigh error flag. TODO document
         */
        void setZNeighError();
        /**
         * Sets node outer normal error flag. TODO document
         */
        void setNormalError();
        /**
         * Sets node neigh error flag depending on specified axis. TODO document
         *
         * @param axisName Number of axis to set error flag for.
         */
        void setNeighError(unsigned int axisNum);

        /**
         * Returns value of specified custom flag.
         *
         * @param flag Flag to get value of.
         * @return Flag state.
         */
        bool getCustomFlag(uchar flag) const;
        /**
         * Sets value of custom flag.
         *
         * @param flag Flag to set value of.
         * @param value New flag value
         */
        void setCustomFlag(uchar flag, bool value);

        /**
         * Sets border condition id. TODO document
         *
         * @param  newBorderConditionId Id of the new border condition.
         */
        void setBorderConditionId(uchar newBorderConditionId);
        /**
         * Returns id of the node border condition. TODO document
         *
         * @return Id of the border condition.
         */
        uchar getBorderConditionId() const;

        /**
         * Sets contact condition id. TODO document
         *
         * @param  newContactConditionId Id of the new contact condition.
         */
        void setContactConditionId(uchar newContactConditionId);
        /**
         * Returns id of the node contact condition. TODO document
         *
         * @return Id of the contact condition.
         */
        uchar getContactConditionId() const;
        /**
         * Sets node material id.  Sets material id and updates values for
         * node material-related fields (e.g. rho)
         *
         * @param id Material id
         */
        void setMaterialId(uchar id);
        /**
         * Returns node material id.
         *
         * @return Material id.
         */
        uchar getMaterialId() const;
        /**
         * Returns node material.
         *
         * @return Material.
         */
        Material* getMaterial() const override;

        /**
         * Returns rheology matrix for node. It's wrapper for corresponding
         * material APIs.
         *
         * @return Rheology matrix.
         */
        RheologyMatrix3D& getRheologyMatrix() const;

        /**
         * Sets density value for node.
         *
         * @param rho Density value
         */
        void setRho(gcm_real rho);
        /**
         * Returns density value for node.
         * @return Density value.
         */
        gcm_real getRho() const override;
        /**
         * Returns density value for the material. Returns node-independent
         * density value (material-specific).
         *
         * @return Density value.
         */
        gcm_real getRho0() const;

        /**
         * Returns crack direction. TODO document
         *
         * @return Crack direction.
         */
        const vector3& getCrackDirection() const;
        /**
         * Creates crack in specified direction. TODO document
         *
         * @param direction Crack direction.
         */
        void createCrack(int direction);
        /**
         * Creates crack in specified direction. TODO document
         *
         * @param direction Crack direction.
         */
        void createCrack(const vector3& crack);
        /**
         * TODO document
         *
         * @param TODO document
         */
        void cleanStressByDirection(const vector3& direction);

        /**
         * Constant to access custom flag1 using getCustomFlag / setCustomFlag.
         */
        static const int FLAG_1 = 1;
        /**
         * Constant to access custom flag2 using getCustomFlag / setCustomFlag.
         */
        static const int FLAG_2 = 2;
        /**
         * Constant to access custom flag3 using getCustomFlag / setCustomFlag.
         */
        static const int FLAG_3 = 3;
        /**
         * Constant to access custom flag4 using getCustomFlag / setCustomFlag.
         */
        static const int FLAG_4 = 4;
        /**
         * Constant to access custom flag5 using getCustomFlag / setCustomFlag.
         */
        static const int FLAG_5 = 5;
        /**
         * Constant to access custom flag6 using getCustomFlag / setCustomFlag.
         */
        static const int FLAG_6 = 6;
    };
}

namespace std {

    inline std::ostream& operator<<(std::ostream &os, const gcm::CalcNode &node) {
        os << "\n\tCalcNode number: " << node.number << "\n";
        os << "\tCoords:";
        for (int i = 0; i < 3; i++)
            os << " " << node.coords[i];
        os << "\n\tVelocity:";
        for (int i = 0; i < 3; i++)
            os << " " << node.values[i];
        os << "\n\tStress:";
        for (int i = 3; i < GCM_VALUES_SIZE; i++)
            os << " " << node.values[i];
        os << "\n\tRho: " << node.getRho();
        return os;
    }
}

#endif
