#ifndef CALC_NODE_H_
#define CALC_NODE_H_

#include <vector>
#include <assert.h>
#include <iostream>
#include <string.h>
#include "Node.h"
#include "../Engine.h"

#define GCM_VALUES_SIZE 9

#define PLACEMENT_TYPE_MASK 2
enum PlacementType
{
	Local = 2,
	Remote = 0
};

#define IS_USED_MASK 4

#define IS_BORDER_MASK 8

#define CONTACT_TYPE_MASK 1
enum ContactType
{
	Free = 0,
	InContact = 1
};

#define ENGINE_OWNERSHIP_MASK 48
enum EngineOwner
{
	NONE = 0,
	GCM = 16,
	SPH = 32
};

#define NODE_ORDER_MASK 64
enum NodeOrder
{
	FirstOrder = 0,
	SecondOrder = 64
};

// Error flags
#define X_NEIGH_ERROR_MASK 1
#define Y_NEIGH_ERROR_MASK 2
#define Z_NEIGH_ERROR_MASK 4

using namespace std;
using namespace gcm;

namespace gcm {
	/*
	 * Base class for all nodes used in calculations
	 */
	class CalcNode: public Node {
		
		friend class DataBus;
		friend class VtuTetrFileReader;
		friend class Vtu2TetrFileReader;
		
		public:
			
			CalcNode();
			CalcNode(int _num);
			CalcNode(int _num, float _x, float _y, float _z);
			CalcNode(const CalcNode& src);
			~CalcNode();
			
			CalcNode &operator=(const CalcNode &src);
			
			void clearErrorFlags();
			
			union
			{
				float values[GCM_VALUES_SIZE];
				struct
				{
					/*
					* Velocity vector components.
					*/
					float vx;
					float vy;
					float vz;
					/*
					 * Stress tensor components.
					*/
					float sxx;
					float sxy;
					float sxz;
					float syy;
					float syz;
					float szz;
				};
			};
			
			

			inline bool rheologyIsValid() {
				return ( materialId >= 0 && rho > 0 );
			}
			
			vector<int>* elements;
			vector<int>* border_elements;
			int contactNodeNum;
			
			bool inline isInContact ()
			{
				return InContact == (publicFlags & CONTACT_TYPE_MASK);
			}

			void inline setContactType (ContactType type)
			{
				publicFlags &= (~CONTACT_TYPE_MASK);
				publicFlags |= (CONTACT_TYPE_MASK & type);

				assert (InContact == type ? isInContact () : !isInContact ());
			}

			/**
			 *
			 * @return <code>true</code> if this Node is used and its placement is Local
			 */
			bool inline isLocal ()
			{
				return isUsed () && Local == (privateFlags & PLACEMENT_TYPE_MASK);
			}

			/**
			 *
			 * @return <code>true</code> if this Node is used and its placement is Remote
			 */
			bool inline isRemote ()
			{
				return isUsed () && Remote == (privateFlags & PLACEMENT_TYPE_MASK);
			}

			/**
			 * Set node placement to specified value and also marks node as used
			 * @param placement new node placement
			 */
			void inline setPlacement (PlacementType placement)
			{
				setUsed (true);

				privateFlags &= (~PLACEMENT_TYPE_MASK);
				privateFlags |= (PLACEMENT_TYPE_MASK & placement);

				assert (Local == placement ? isLocal () : isRemote ());
			}

			bool inline isUsed ()
			{
				return 0 != (publicFlags & IS_USED_MASK);
			}

			void inline setUsed (bool used)
			{
				if (used) publicFlags |= IS_USED_MASK;
				else publicFlags &= (~IS_USED_MASK);

				assert (used ? isUsed () : !isUsed ());
			}

			bool inline isFirstOrder ()
			{
				return 0 == (publicFlags & NODE_ORDER_MASK);
			}

			bool inline isSecondOrder ()
			{
				return 0 != (publicFlags & NODE_ORDER_MASK);
			}

			void inline setOrder (NodeOrder order)
			{
				publicFlags &= (~NODE_ORDER_MASK);
				publicFlags |= (NODE_ORDER_MASK & order);

				assert (FirstOrder == order ? isFirstOrder () : isSecondOrder ());
			}

			void inline setIsBorder (bool border)
			{
				if (border) privateFlags |= IS_BORDER_MASK;
				else privateFlags &= (~IS_BORDER_MASK);

				assert (border ? isBorder () : !isBorder ());
			}

			bool inline isBorder ()
			{
				return 0 != (privateFlags & IS_BORDER_MASK);
			}

			bool inline isInner ()
			{
				return !isBorder ();
			}

			bool inline isOwnedBy (EngineOwner owner)
			{
				assert (0 != (publicFlags & ENGINE_OWNERSHIP_MASK));//Node should be owned at least by someone
				return 0 != (publicFlags & ENGINE_OWNERSHIP_MASK & owner);
			}

			void inline addOwner (EngineOwner owner)
			{
				publicFlags |= (ENGINE_OWNERSHIP_MASK & owner);
			}
			
			void inline setXNeighError ()
			{
				errorFlags |= X_NEIGH_ERROR_MASK;
			}
			
			void inline setYNeighError ()
			{
				errorFlags |= Y_NEIGH_ERROR_MASK;
			}
			
			void inline setZNeighError ()
			{
				errorFlags |= Z_NEIGH_ERROR_MASK;
			}
			
			void inline setNeighError (unsigned int axisNum)
			{
				assert ( axisNum < 3 );
				if( axisNum == 0 )
					setXNeighError();
				else if( axisNum == 1 )
					setYNeighError();
				else if( axisNum == 2 )
					setZNeighError();
			}

			/**
			* This method is only to be used to dump mesh state into a file or any other output stream
			* @return
			*/
		   unsigned int inline getPublicFlags ()
		   {
			   return publicFlags;
		   }

		   void inline setBorderConditionId (unsigned char newBorderCondId)
		   {
			   borderCondId = newBorderCondId;
		   }
		   
		   unsigned char inline getBorderConditionId ()
		   {
			   return borderCondId;
		   }
		   
		   void inline setMaterialId (unsigned char id)
		   {
			   materialId = id;
		   }
		   
		   void inline initRheology()
		   {
			   if( rho == 0 )
			   {
				   rho = Engine::getInstance().getMaterial(materialId)->getRho();
			   }
		   }
		   
		   unsigned char inline getMaterialId ()
		   {
			   return materialId;
		   }
		   
		   void inline setRho (float _rho)
		   {
			   rho = _rho;
		   }
		   
		   float inline getRho () const
		   {
			   return rho;
		   }
		   
		   float inline getLambda()
		   {
			   return Engine::getInstance().getMaterial(materialId)->getLambda();
		   }
		   
		   float inline getMu()
		   {
			   return Engine::getInstance().getMaterial(materialId)->getMu();
		   }
		   
   		   /**
			* This method is only to be used to dump mesh state into a file or any other output stream
			* @return
			*/
		   unsigned int inline getPrivateFlags ()
		   {
			   return privateFlags;
		   }
		   
		   unsigned int inline getErrorFlags ()
		   {
			   return errorFlags;
		   }
		   
	protected:

		   /**
			* This method is only supposed to be used to read mesh state from a file or any other input stream
			* @param flags
			*/
		   void inline setPublicFlags (unsigned int flags)
		   {
			   publicFlags = flags;
		   }

		   /**
			* This method is only supposed to be used to read mesh state from a file or any other input stream
			* @param flags
			*/
		   void inline setPrivateFlags (unsigned int flags)
		   {
			   privateFlags = flags;
		   }

		   /**
			* Node flags transmitted and synchronized over MPI.
			*/
		   unsigned int publicFlags;
		   /**
			* Node flags specific for current processor, neither transmitted nor synchronized over MPI.
			*/
		   unsigned int privateFlags;
		   /*
			* Flags to indicate different issues with this node during calculations
			*/
		   unsigned int errorFlags;
		   /**
			* Border condition that is used for this node. Condition should be registered in Engine.
			*/
		   unsigned char borderCondId;
		   
		   /*
		    * Rheology parameters.
		    */
		   float rho;
		   unsigned char materialId;
	};
}

namespace std {
	inline std::ostream& operator<< (std::ostream &os, const gcm::CalcNode &node) {
		os << "\n\tCalcNode number: " << node.number << "\n";
		os << "\tCoords:";
		for( int i = 0; i < 3; i++ )
			os << " " << node.coords[i];
		os << "\n\tVelocity:";
		for( int i = 0; i < 3; i++ )
			os << " " << node.values[i];
		os << "\n\tStress:";
		for( int i = 3; i < GCM_VALUES_SIZE; i++ )
			os << " " << node.values[i];
		os << "\n\tRho: " << node.getRho();
		return os;
	}
}

#endif