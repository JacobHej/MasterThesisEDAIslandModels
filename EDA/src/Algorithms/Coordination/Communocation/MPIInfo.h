#pragma once

/**
 * @brief Static class storing MPI information. (Must have called MPIWrapper::mpi:init before use)
 */
class MPIInfo {
public:
	/**
	 * @brief Channels used for communication ("tags" in MPI terminology)
	 */
	enum Channels {
		Data = 1,
		Halt = 2
	};

	/**
	 * @brief Size of current MPI session
	 */
	static int Size;

	/**
	 * @brief Current rank
	 */
	static int Rank; 
};

