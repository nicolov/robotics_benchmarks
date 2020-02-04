#include <Eigen/Dense>
#include <Fastor/Fastor.h>
#include <benchmark/benchmark.h>

#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

std::uniform_int_distribution<int> int_distribution(0, 100);

template <int CStateSize, int CObsSize = 3, typename NumType = double>
static void CovarianceUpdateEigen(benchmark::State &state) {
  using Eigen::Dynamic;
  using Eigen::Matrix;

  if (CStateSize != Dynamic && CStateSize != state.range(0)) {
    throw std::runtime_error("Compile time state size doesn't match runtime");
  }

  const int stateSize = state.range(0);
  const int obsSize = CObsSize;

  Matrix<NumType, CStateSize, CStateSize> P_x;  // covariance matrix
  P_x.resize(stateSize, stateSize);
  P_x.setOnes();
  P_x = (P_x.transpose() * P_x).eval();

  Matrix<NumType, CObsSize, CStateSize> H_dx;  // observation model
  H_dx.resize(obsSize, stateSize);
  H_dx.setOnes();

  Matrix<NumType, CObsSize, CObsSize> R_meas;  // measurement noise
  R_meas.resize(obsSize, obsSize);
  std::default_random_engine eng(0);
  for (int i = 0; i < obsSize; i++) {
    for (int j = 0; j < obsSize; j++) {
      R_meas(i, j) = int_distribution(eng);
    }
  }
  R_meas = R_meas.transpose() * R_meas;

  // scratch space for computations
  Matrix<NumType, CStateSize, CObsSize> K_gain;
  K_gain.resize(stateSize, obsSize);
  Matrix<NumType, CObsSize, CObsSize> S_res;
  S_res.resize(obsSize, obsSize);
  Matrix<NumType, CStateSize, CStateSize> I_KH;
  I_KH.resize(stateSize, stateSize);
  Matrix<NumType, CStateSize, CStateSize> P_x_ref;
  P_x_ref.resize(stateSize, stateSize);

  for (auto _ : state) {
    S_res = H_dx * P_x * H_dx.transpose() + R_meas;
    K_gain = P_x * H_dx.transpose() * S_res.inverse();

    I_KH = Matrix<NumType, CStateSize, CStateSize>::Identity(stateSize,
                                                             stateSize) -
           K_gain * H_dx;

    P_x_ref = I_KH * P_x * I_KH.transpose();
    P_x_ref += K_gain * R_meas * K_gain.transpose();

    benchmark::DoNotOptimize(P_x_ref);
    benchmark::ClobberMemory();
  }
}

template <int CStateSize, int CObsSize = 3, typename NumType = double>
static void CovarianceUpdateFastor(benchmark::State &state) {
  using Fastor::inverse;
  using Fastor::matmul;
  using Fastor::Tensor;
  using Fastor::transpose;

  Tensor<NumType, CStateSize, CStateSize> P_x;
  P_x.ones();
  P_x = matmul(transpose(P_x), P_x);

  Tensor<NumType, CObsSize, CStateSize> H_dx;
  H_dx.ones();

  Tensor<NumType, CObsSize, CObsSize> R_meas;
  std::default_random_engine eng(0);
  for (int i = 0; i < CObsSize; i++) {
    for (int j = 0; j < CObsSize; j++) {
      R_meas(i, j) = int_distribution(eng);
    }
  }
  R_meas = Fastor::matmul(transpose(R_meas), R_meas);

  Tensor<NumType, CStateSize, CObsSize> K_gain;
  Tensor<NumType, CObsSize, CObsSize> S_res;
  Tensor<NumType, CStateSize, CStateSize> I_KH;
  Tensor<NumType, CStateSize, CStateSize> P_x_ref;

  for (auto _ : state) {
    S_res = matmul(matmul(H_dx, P_x), transpose(H_dx)) + R_meas;
    K_gain = matmul(matmul(P_x, transpose(H_dx)), inverse(S_res));

    I_KH.eye();
    I_KH -= matmul(K_gain, H_dx);

    P_x_ref = matmul(matmul(I_KH, P_x), transpose(I_KH));
    P_x_ref += matmul(matmul(K_gain, R_meas), transpose(K_gain));

    benchmark::DoNotOptimize(P_x_ref);
    benchmark::ClobberMemory();
  }
}

#define COVARIANCE_UPDATE_FOR_SIZE(n)                   \
  BENCHMARK_TEMPLATE(CovarianceUpdateFastor, n);        \
  BENCHMARK_TEMPLATE(CovarianceUpdateEigen, n)->Arg(n); \
  BENCHMARK_TEMPLATE(CovarianceUpdateEigen, Eigen::Dynamic)->Arg(n);

COVARIANCE_UPDATE_FOR_SIZE(8);
COVARIANCE_UPDATE_FOR_SIZE(16);
COVARIANCE_UPDATE_FOR_SIZE(32);
COVARIANCE_UPDATE_FOR_SIZE(64);
COVARIANCE_UPDATE_FOR_SIZE(128);

#undef COVARIANCE_UPDATE_FOR_SIZE

BENCHMARK_MAIN();
