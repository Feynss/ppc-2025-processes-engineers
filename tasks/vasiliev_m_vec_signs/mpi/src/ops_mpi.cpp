#include "vasiliev_m_vec_signs/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <numeric>
#include <vector>

#include "util/include/util.hpp"
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
  int rank = 0, size = 1;
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

  for (size_t i = 0; i < local_data.size() - 1; i++) {
    if ((local_data[i] > 0 && local_data[i + 1] < 0) || (local_data[i] < 0 && local_data[i + 1] > 0)) {
      local_count++;
    }
  }

  int last_elem = 0;
  int first_elem = 0;
  bool has_first = false;
  bool has_last = false;
  if (!local_data.empty()) {
    first_elem = local_data.front();
    last_elem = local_data.back();
    has_first = has_last = true;
  }

  int prev_last = 0;
  if (rank > 0) {
    MPI_Recv(&prev_last, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    if (has_first) {
      if ((prev_last > 0 && first_elem < 0) || (prev_last < 0 && first_elem > 0)) {
        local_count++;
      }
    }
  }

  if (rank < size - 1) {
    int send_val = has_last ? last_elem : 0;
    MPI_Send(&send_val, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
  }

  int global_count = 0;
  MPI_Allreduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

  GetOutput() = global_count;

  return true;
}

bool VasilievMVecSignsMPI::PostProcessingImpl() {
  return GetOutput() >= 0;
}

}  // namespace vasiliev_m_vec_signs
