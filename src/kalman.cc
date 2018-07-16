#include <Eigen/Dense>
#include <sltbench/Bench.h>

#include <algorithm>
#include <iostream>
#include <vector>

struct KalmanStuff {
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    Eigen::MatrixXd P_x; // covariance matrix
    Eigen::Matrix3Xd H_dx; // observation model
    Eigen::Matrix3d R_meas; // measurement noise

    // scratch space for computations
    Eigen::MatrixX3d K_gain;
    Eigen::Matrix3d S_res;
    Eigen::MatrixXd I_KH;
    Eigen::MatrixXd P_x_ref;
};

class KalmanFixture {
public:
    using Type = KalmanStuff;

    KalmanFixture() {
    }

    Type& SetUp(const long int& stateSize) {
        // Only need to re-initialize the matrices if the state size has
        // changed.
        if (_stuff.P_x.rows() != stateSize) {
            // Random symmetric matrix for the initial covariance.
            _stuff.P_x = Eigen::MatrixXd::Random(stateSize, stateSize);
            _stuff.P_x = (_stuff.P_x.transpose() * _stuff.P_x).eval();

            // Observation model.
            _stuff.H_dx = Eigen::Matrix3Xd::Zero(3, stateSize);
            _stuff.H_dx.middleCols<3>(0) = Eigen::Matrix3d::Identity();

            // Random symmetric matrix for measurement noise.
            _stuff.R_meas = Eigen::Matrix3d::Random();
            _stuff.R_meas = (_stuff.R_meas.transpose() * _stuff.R_meas).eval();

            _stuff.K_gain.resize(stateSize, 3);
            _stuff.I_KH.resize(stateSize, stateSize);
            _stuff.P_x_ref.resize(stateSize, stateSize);            
        }

        return _stuff;
    }

    void TearDown() {}
private:
    Type _stuff;
};

void KalmanCovarianceUpdate(KalmanFixture::Type& state, const long int & stateSize) {
    state.S_res = state.H_dx * state.P_x * state.H_dx.transpose() + state.R_meas;
    state.K_gain = state.P_x * state.H_dx.transpose() * state.S_res.inverse();

    state.I_KH =
        Eigen::MatrixXd::Identity(stateSize, stateSize)
        - state.K_gain * state.H_dx;

    state.P_x_ref = state.I_KH * state.P_x * state.I_KH.transpose();
    state.P_x_ref += state.K_gain * state.R_meas * state.K_gain.transpose();

    sltbench::DoNotOptimize(state.P_x_ref);
}

static const std::vector<long int> stateSizes = { 100, 200, 400 };

SLTBENCH_FUNCTION_WITH_FIXTURE_AND_ARGS(KalmanCovarianceUpdate, KalmanFixture, stateSizes);

SLTBENCH_MAIN();
