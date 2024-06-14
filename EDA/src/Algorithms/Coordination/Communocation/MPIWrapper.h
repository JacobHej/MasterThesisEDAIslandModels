#pragma once
#include <mpi.h>
#include "../Topology/Operation.h"
#include <vector>
#include "MPIInfo.h"
#include "SampleContainer.h"
/**
 * @brief Class that handles serialized MPI communication through a simple interface
 */
class MPIWrapper {
public:
	/**
	 * @brief Checks if current MPI process should initiate halt, and communicates
	 * this to the remaining MPI procs. Also check if any proc has initiated a halt
	 * 
	 * @param initiate_halt Should current proc initiate halt 
	 * @return true if proc should halt
	 * @return false if proc should NOT halt
	 */
	static bool should_halt(bool initiate_halt) {
		if (initiate_halt) {
			int data = 1;
			std::vector<MPI_Request> requests(MPIInfo::Size - 1);
			int request_index = 0;
			for (int target_rank = 0; target_rank < MPIInfo::Size; target_rank++) {
				if (target_rank != MPIInfo::Rank) {  
					MPI_Isend(&data, 1, MPI_INT, target_rank, MPIInfo::Channels::Halt, MPI_COMM_WORLD, &requests[request_index++]);
				}
			}
			MPI_Waitall(requests.size(), requests.data(), MPI_STATUSES_IGNORE);
		}

		MPI_Barrier(MPI_COMM_WORLD);

		MPI_Status status; int flag = 0;
		MPI_Iprobe(MPI_ANY_SOURCE, MPIInfo::Channels::Halt, MPI_COMM_WORLD, &flag, &status);
		if (flag) {
			int buf = 0;
			while(flag){
				MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, MPIInfo::Channels::Halt, MPI_COMM_WORLD, &status);
				MPI_Iprobe(MPI_ANY_SOURCE, MPIInfo::Channels::Halt, MPI_COMM_WORLD, &flag, &status);
			}
			MPI_Barrier(MPI_COMM_WORLD);
			return true;
		}
		else {
			MPI_Barrier(MPI_COMM_WORLD);
			return initiate_halt;
		}
	}

	static SampleContainer do_communicate_sample_container(SampleContainer& outgoing_data, std::vector<Operation>& comm_operations, MPI_Datatype mpi_datatype) {
		int recieve_sample_index = 0;
		int recieve_fitness_index = 0;
		int recieve_count = 0;
		for (Operation opr : comm_operations) recieve_count += opr.operation_type == OperationType::recieve;

		SampleContainer recieved_samples(outgoing_data.sample_count * recieve_count, outgoing_data.sample_size);

		MPI_Status status;

		MPI_Barrier(MPI_COMM_WORLD);

		for (Operation opr : comm_operations) {
			switch (opr.operation_type) {
			case(OperationType::send):
				MPI_Send(outgoing_data.samples.get(), outgoing_data.samples.size(), MPI_UNSIGNED_SHORT, opr.target, MPIInfo::Channels::Data, MPI_COMM_WORLD);
				MPI_Send(outgoing_data.sample_fitnesses.get(), outgoing_data.sample_fitnesses.size(), MPI_UNSIGNED_SHORT, opr.target, MPIInfo::Channels::Data, MPI_COMM_WORLD);
				break;
			case(OperationType::recieve):
				MPI_Recv(
					recieved_samples.samples.get() + recieve_sample_index,
					outgoing_data.samples.size(),
					mpi_datatype, opr.target, MPIInfo::Channels::Data, MPI_COMM_WORLD, &status);
				MPI_Recv(
					recieved_samples.sample_fitnesses.get() + recieve_fitness_index,
					outgoing_data.sample_fitnesses.size(),
					mpi_datatype, opr.target, MPIInfo::Channels::Data, MPI_COMM_WORLD, &status);

				recieve_fitness_index += outgoing_data.sample_fitnesses.size();
				recieve_sample_index += outgoing_data.samples.size();
				break;
			}
		}

		MPI_Barrier(MPI_COMM_WORLD);

		return recieved_samples;
	}

	static DynamicArray<uint16_t> do_communicate_dynamic_array(DynamicArray<uint16_t>& outgoing_data, std::vector<Operation>& comm_operations, MPI_Datatype mpi_datatype) {
		int recieve_sample_index = 0;
		int recieve_count = 0;
		for (Operation opr : comm_operations) recieve_count += opr.operation_type == OperationType::recieve;

		DynamicArray<uint16_t> recieved_samples(outgoing_data.size() * recieve_count);

		MPI_Status status;

		MPI_Barrier(MPI_COMM_WORLD);

		for (Operation opr : comm_operations) {
			switch (opr.operation_type) {
			case(OperationType::send):
				MPI_Send(outgoing_data.get(), outgoing_data.size(), MPI_UNSIGNED_SHORT, opr.target, MPIInfo::Channels::Data, MPI_COMM_WORLD);
				break;
			case(OperationType::recieve):
				MPI_Recv(
					recieved_samples.get() + recieve_sample_index,
					outgoing_data.size(),
					mpi_datatype, opr.target, MPIInfo::Channels::Data, MPI_COMM_WORLD, &status);

				recieve_sample_index += outgoing_data.size();
				break;
			}
		}

		MPI_Barrier(MPI_COMM_WORLD);

		return recieved_samples;
	}

	static DynamicArray<int> do_communicate_bestFitFe(int outgoing_data) {
		MPI_Barrier(MPI_COMM_WORLD);

		DynamicArray<int> recieved_data(MPIInfo::Size - 1);
		MPI_Status status;
		int meDone = 0;
		for (int i = 0; i < MPIInfo::Size; i++) {
			if (i == MPIInfo::Rank) {
				for (int j = 0; j < MPIInfo::Size; j++) {
					if (j == MPIInfo::Rank) continue;
					MPI_Send(&outgoing_data, 1, MPI_INT, j, MPIInfo::Channels::Data, MPI_COMM_WORLD);
				}
				meDone = 1;
				continue;
			}

			MPI_Recv(recieved_data.get() + i - meDone, 1, MPI_INT, i, MPIInfo::Channels::Data, MPI_COMM_WORLD, &status);
		}

		MPI_Barrier(MPI_COMM_WORLD);

		return recieved_data;
	}

	/**
	 * @brief Initializes MPI size and rank in MPIInfo
	 * 
	 * @param argc 
	 * @param argv 
	 */
	static void mpi_init(int argc, char* argv[]) {
		MPI_Init(&argc, &argv);

		int world_size;
		MPI_Comm_size(MPI_COMM_WORLD, &MPIInfo::Size);

		int world_rank;
		MPI_Comm_rank(MPI_COMM_WORLD, &MPIInfo::Rank);
	}
	
	static void mpi_finalize() {
		MPI_Finalize();
	}
};