#include "vasiliev_m_vec_signs/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <cstddef>
#include <vector>

#include "vasiliev_m_vec_signs/common/include/common.hpp"

namespace vasiliev_m_vec_signs {

VasilievMVecSignsMPI::VasilievMVecSignsMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = OutType{};
}

bool VasilievMVecSignsMPI::ValidationImpl() {
  return !GetInput().empty();
}

bool VasilievMVecSignsMPI::PreProcessingImpl() {
  GetOutput() = 0;
  return true;
}

bool VasilievMVecSignsMPI::RunImpl() {
  auto &vec = GetInput();
  int rank = 0;
  int size = 1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int n = static_cast<int>(vec.size());
  int chunk = n / size;
  int remain = n % size;

  std::vector<int> counts(size, chunk);
  for (int i = 0; i < remain; i++) {
    counts[i]++;
  }

  std::vector<int> displs(size, 0);
  for (int i = 1; i < size; i++) {
    displs[i] = displs[i - 1] + counts[i - 1];
  }

  std::vector<int> local_data(counts[rank]);
  MPI_Scatterv(vec.data(), counts.data(), displs.data(), MPI_INT, local_data.data(), counts[rank], MPI_INT, 0,
               MPI_COMM_WORLD);

  int local_count = 0;

  if (!local_data.empty()) {
    for (size_t i = 0; i < local_data.size() - 1; i++) {
      if (SignChangeCheck(local_data[i], local_data[i + 1])) {
        local_count++;
      }
    }
  }

  int first_elem = local_data.empty() ? 0 : local_data.front();
  int last_elem  = local_data.empty() ? 0 : local_data.back();

  int prev_last = 0;
  if (rank > 0) {
      MPI_Recv(&prev_last, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      if (!local_data.empty() && SignChangeCheck(prev_last, first_elem)) local_count++;
  }

  if (rank < size - 1) {
      MPI_Send(&last_elem, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
  }

  int global_count = 0;
  MPI_Allreduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

  GetOutput() = global_count;

  return true;
}

bool VasilievMVecSignsMPI::SignChangeCheck(int a, int b) {
  return (a > 0 && b < 0) || (a < 0 && b > 0);
}

bool VasilievMVecSignsMPI::PostProcessingImpl() {
  return GetOutput() >= 0;
}

}  // namespace vasiliev_m_vec_signs
