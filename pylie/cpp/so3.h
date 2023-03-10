#ifndef _SO3_H_
#define _SO3_H_

#include <Eigen/Dense>

#include "base.h"

class SO3 : public MatrixLieGroup<3, 3> {
 public:
  static Element random() { return Exp(Vector::Random()); }

  /**
   * @brief Exponential map: converts a rotation vector to an element of SO(3)
   *
   * @param x rotation vector
   * @return Eigen::Matrix3d
   */
  static Eigen::Matrix3d Exp(const Eigen::Vector3d& x) {
    const Eigen::Matrix3d Xi = wedge(x);
    const double theta = x.norm();
    if (theta < SO3::small_angle_tol) {
      return Eigen::Matrix3d::Identity() + Xi;
    } else {
      return Eigen::Matrix3d::Identity() + Xi * sin(theta) / theta +
             Xi * Xi * (1 - cos(theta)) / (theta * theta);
    }
  };

  /**
   * @brief Logarithm map: converts an element of SO(3) to a rotation vector
   *
   * @param x Element of SO3 as 3x3 eigen matrix
   * @return Eigen::Vector3d
   */
  static Eigen::Vector3d Log(const Eigen::Matrix3d& x) {
    Eigen::Vector3d xi;
    double theta = acos((x.trace() - 1) / 2);
    if (theta < SO3::small_angle_tol) {
      // xi = x.block<3, 1>(0, 2);
      xi = SO3::vee(x - Eigen::Matrix3d::Identity());
    } else {
      xi = SO3::vee((theta / (2 * sin(theta))) * (x - x.transpose()));
    }
    return xi;
  };

  static Eigen::Matrix3d wedge(const Eigen::Vector3d& x) {
    Eigen::Matrix3d X;
    // clang-format off
    X <<     0, -x(2),  x(1),
          x(2),     0, -x(0), 
         -x(1),  x(0),     0;
    // clang-format on
    return X;
  };

  static Eigen::Vector3d vee(const Eigen::Matrix3d& x) {
    Eigen::Vector3d xi;
    xi << x(2, 1), x(0, 2), x(1, 0);
    return xi;
  };

  static Eigen::Matrix3d leftJacobian(const Eigen::Vector3d& x) {
    Eigen::Matrix3d J{Eigen::Matrix3d::Identity()};
    double theta{x.norm()};
    if (theta < SO3::small_angle_tol) {
      J = Eigen::Matrix3d::Identity();
    } else {
      Eigen::Matrix3d cross_x{wedge(x)};
      J = J + (1 - cos(theta)) / (theta * theta) * cross_x +
          (theta - sin(theta)) / (theta * theta * theta) * cross_x * cross_x;
    }
    return J;
  };

  static Eigen::Matrix3d leftJacobianInverse(const Eigen::Vector3d& x) {
    Eigen::Matrix3d J{Eigen::Matrix3d::Identity()};
    double theta{x.norm()};
    if (theta < SO3::small_angle_tol) {
      J = Eigen::Matrix3d::Identity();
    } else {
      Eigen::Matrix3d cross_x{wedge(x)};
      J = J - 0.5 * cross_x +
          (1 / (theta * theta)) *
              (1.0 - (theta * sin(theta) / (2.0 * (1.0 - cos(theta))))) *
              cross_x * cross_x;
    }
    return J;
  };

  static Eigen::Matrix3d inverse(const Eigen::Matrix3d& x) { return x.transpose(); };

  static Eigen::Matrix3d adjoint(const Eigen::Matrix3d& x) { return x; };

  static Eigen::Matrix3d odot(const Eigen::Vector3d& x) { return -wedge(x); };

};

#endif