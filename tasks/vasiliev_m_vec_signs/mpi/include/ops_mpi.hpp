#pragma once

#include "vasiliev_m_vec_signs/common/include/common.hpp"
#include "task/include/task.hpp"

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
};

}  // namespace vasiliev_m_vec_signs
