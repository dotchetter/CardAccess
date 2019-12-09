#pragma once
#include <iostream>
#include <algorithm>
#include <vector>
#include "accesscard.h"

namespace CardAccess {
	
	template<class DATATYPE>
	class queue {
		/* Represent a queue of arbitrary typedefinition.
		   Logical represetation of a queue where elements
		   are added in fair order, and the least recent 
		   most items are returned prior to the ones added
		   more recent. */
	
	private:
		std::vector<DATATYPE> scan_queue;

	public:
		
		void enqueue(DATATYPE object) {
			/* Enqueue an instance in the queue */
			scan_queue.push_back(object);
		}
		
		DATATYPE dequeue() {
			/* Returns the least recent most 
			added item and pops it. */
			if (scan_queue.size() > 0) {
				DATATYPE object = scan_queue[0];
				scan_queue.erase(scan_queue.begin());
				return object;
			}
		}
	
		const DATATYPE peek() const {
			/* Returns the least recent most added 
			item but doesn't remove it. */
			if (scan_queue.size() > 0) {
				return scan_queue[0];
			}
		}
	
		const int size() const {
			/* Returns the amount of elements in queue.*/
			return scan_queue.size();
		}
	
		const bool empty() const {
			/* checks whether the underlying container 
			is empty */
			return scan_queue.size() > 0;
		}
	};	
}
