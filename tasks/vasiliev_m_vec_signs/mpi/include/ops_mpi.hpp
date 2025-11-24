#pragma once

#include "task/include/task.hpp"
#include "vasiliev_m_vec_signs/common/include/common.hpp"

namespace vasiliev_m_vec_signs {

class VasilievMVecSignsMPI : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kMPI;
  }
  explicit VasilievMVecSignsMPI(const InType &in);
  static bool SignChangeCheck(int a, int b);
  void calc_counts_displs(int n, int size, std::vector<int> &counts, std::vector<int> &displs);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace vasiliev_m_vec_signs
