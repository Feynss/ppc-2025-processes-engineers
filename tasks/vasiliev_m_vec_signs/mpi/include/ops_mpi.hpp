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

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
  bool SignChangeCheck(int a, int b);
  void BoundaryCheck(std::vector<int> &local_data, int rank, int size, int &local_count);
};

}  // namespace vasiliev_m_vec_signs
