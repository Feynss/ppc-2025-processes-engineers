#include <gtest/gtest.h>
#include <stb/stb_image.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "vasiliev_m_vec_signs/common/include/common.hpp"
#include "vasiliev_m_vec_signs/mpi/include/ops_mpi.hpp"
#include "vasiliev_m_vec_signs/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace vasiliev_m_vec_signs {

class VasilievMVecSignsFuncTests : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return std::to_string(std::get<0>(test_param)) + "_" + std::get<1>(test_param);
  }

 protected:
  void SetUp() override {
    std::string abs_path = ppc::util::GetAbsoluteTaskPath(PPC_ID_vasiliev_m_vec_signs, "test_vectors.txt");
    std::ifstream file(abs_path);
    if (!file.is_open()) {
      throw std::runtime_error("Wrong path.");
    }

    test_vectors_.clear();
    std::string line;
    while (std::getline(file, line)) {
      if (line.empty()) continue;

      std::stringstream ss(line);
      std::vector<int> vec;
      int val;
      while (ss >> val) {
        vec.push_back(val);
        ss >> std::ws;
        if (ss.peek() == ';') {
          ss.get();
          break;
        }
      }

      int expected = 0;
      ss >> expected;
      test_vectors_.push_back({vec, expected});
    }
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return expected_output_== output_data;
  }

  InType GetTestInputData() final {
    TestType params = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());
    int index = std::get<0>(params);
    input_data_ = test_vectors_[index].first;
    expected_output_ = test_vectors_[index].second;
    return input_data_;
  }

 private:
  std::vector<std::pair<std::vector<int>, int>> test_vectors_;
  InType input_data_;
  OutType expected_output_;
};

namespace {

TEST_P(VasilievMVecSignsFuncTests, AlternationsInVector) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 5> kTestParam = {
    std::make_tuple(0, "case1"),
    std::make_tuple(1, "case2"),
    std::make_tuple(2, "case3"),
    std::make_tuple(3, "case4"),
    std::make_tuple(4, "case5")};

const auto kTestTasksList =
    std::tuple_cat(ppc::util::AddFuncTask<VasilievMVecSignsMPI, InType>(kTestParam, PPC_SETTINGS_vasiliev_m_vec_signs),
                   ppc::util::AddFuncTask<VasilievMVecSignsSEQ, InType>(kTestParam, PPC_SETTINGS_vasiliev_m_vec_signs));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName = VasilievMVecSignsFuncTests::PrintFuncTestName<VasilievMVecSignsFuncTests>;

INSTANTIATE_TEST_SUITE_P(SignAlternationsTests, VasilievMVecSignsFuncTests, kGtestValues, kPerfTestName);

}  // namespace

}  // namespace vasiliev_m_vec_signs
