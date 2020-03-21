#ifndef CONSTANTS_H
#define CONSTANTS_H

enum class OpType {
		NOT,
		OR,
		AND,
		NEXT,
		UNTIL,
		RELEASE,
		GLOBAL,
		FUTURE,
		IMPLY,
		EQUIV
	};	

enum class Child {
		LEFT = 0 , RIGHT = 1
	};

#endif
