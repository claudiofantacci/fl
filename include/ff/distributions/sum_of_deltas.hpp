/*
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2014 Max-Planck-Institute for Intelligent Systems,
 *                     University of Southern California
 *    Manuel Wuthrich (manuel.wuthrich@gmail.com)
 *    Jan Issac (jan.issac@gmail.com)
 *
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Willow Garage, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**
 * @date 05/25/2014
 * @author Manuel Wuthrich (manuel.wuthrich@gmail.com)
 * Max-Planck-Institute for Intelligent Systems,
 * University of Southern California
 */

#ifndef FAST_FILTERING_DISTRIBUTIONS_SUM_OF_DELTAS_HPP
#define FAST_FILTERING_DISTRIBUTIONS_SUM_OF_DELTAS_HPP


#include <Eigen/Dense>

// std
#include <vector>


#include <fl/util/assertions.hpp>
#include <fl/util/traits.hpp>
#include <fl/distribution/interface/moments.hpp>

namespace fl
{

/// \todo MISSING DOC. MISSING UTESTS

// Forward declarations
template <typename Vector> class SumOfDeltas;

/**
 * SumOfDeltas distribution traits specialization
 * \internal
 */
template <typename Vector>
struct Traits<SumOfDeltas<Vector>>
{
    typedef typename Vector::Scalar Scalar;
    typedef Eigen::Matrix<
                Scalar,
                Vector::SizeAtCompileTime,
                Vector::SizeAtCompileTime
            > SecondMoment;

    typedef std::vector<Vector> Deltas;
    typedef Eigen::Matrix<Scalar, Eigen::Dynamic, 1> Weights;

    typedef Moments<Vector, SecondMoment> MomentsBase;
};

/**
 * \class SumOfDeltas
 * \ingroup distributions
 */
template <typename Vector>
class SumOfDeltas:
        public Traits<SumOfDeltas<Vector>>::MomentsBase
{
public:
    typedef SumOfDeltas<Vector> This;

    typedef typename Traits<This>::Scalar     Scalar;
    typedef typename Traits<This>::SecondMoment   SecondMoment;
    typedef typename Traits<This>::Deltas     Deltas;
    typedef typename Traits<This>::Weights    Weights;

public:
    explicit SumOfDeltas(const unsigned& dimension = Vector::SizeAtCompileTime)
    {
        static_assert_base(Vector, Eigen::Matrix<Scalar, Vector::SizeAtCompileTime, 1>);

        deltas_ = Deltas(1, Vector::Zero(dimension == Eigen::Dynamic ? 0 : dimension));
        weights_ = Weights::Ones(1);
    }

    virtual ~SumOfDeltas() { }

    virtual void SetDeltas(const Deltas& deltas, const Weights& weights)
    {
        deltas_ = deltas;
        weights_ = weights.normalized();
    }

    virtual void SetDeltas(const Deltas& deltas)
    {
        deltas_ = deltas;
        weights_ = Weights::Ones(deltas_.size())/Scalar(deltas_.size());
    }

    virtual void GetDeltas(Deltas& deltas, Weights& weights) const
    {
        deltas = deltas_;
        weights = weights_;
    }

    virtual Vector mean() const
    {
        Vector mu(Vector::Zero(dimension()));
        for(size_t i = 0; i < deltas_.size(); i++)
            mu += weights_[i] * deltas_[i];

        return mu;
    }

    virtual SecondMoment covariance() const
    {
        Vector mu = mean();
        SecondMoment cov(SecondMoment::Zero(dimension(), dimension()));
        for(size_t i = 0; i < deltas_.size(); i++)
            cov += weights_[i] * (deltas_[i]-mu) * (deltas_[i]-mu).transpose();

        return cov;
    }

    virtual int dimension() const
    {
        return deltas_[0].rows();
    }

protected:
    Deltas  deltas_;
    Weights weights_;
};

}

#endif
