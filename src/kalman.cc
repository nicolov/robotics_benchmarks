#include <benchmark/benchmark.h>
#include <Eigen/Dense>

#include <algorithm>
#include <iostream>
#include <vector>

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
  // Random symmetric matrix for the initial covariance.
  P_x = Matrix<NumType, Dynamic, Dynamic>::Random(stateSize, stateSize);
  P_x = (P_x.transpose() * P_x).eval();

  Matrix<NumType, CObsSize, CStateSize> H_dx;  // observation model
  // Observation model.
  H_dx = Matrix<NumType, Dynamic, Dynamic>::Zero(obsSize, stateSize);

  Matrix<NumType, CObsSize, CObsSize> R_meas;  // measurement noise
  // Random symmetric matrix for measurement noise.
  R_meas = Matrix<NumType, Dynamic, Dynamic>::Random(obsSize, obsSize);
  R_meas = (R_meas.transpose() * R_meas).eval();

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

#define GENERATE_EIGEN_DYNAMIC_AND_STATIC_FOR_SIZE(n)   \
  BENCHMARK_TEMPLATE(CovarianceUpdateEigen, n)->Arg(n); \
  BENCHMARK_TEMPLATE(CovarianceUpdateEigen, Eigen::Dynamic)->Arg(n);

GENERATE_EIGEN_DYNAMIC_AND_STATIC_FOR_SIZE(8);
GENERATE_EIGEN_DYNAMIC_AND_STATIC_FOR_SIZE(16);
GENERATE_EIGEN_DYNAMIC_AND_STATIC_FOR_SIZE(32);
GENERATE_EIGEN_DYNAMIC_AND_STATIC_FOR_SIZE(64);
GENERATE_EIGEN_DYNAMIC_AND_STATIC_FOR_SIZE(128);

#undef GENERATE_EIGEN_DYNAMIC_STATIC_FOR_SIZE

BENCHMARK_MAIN();
