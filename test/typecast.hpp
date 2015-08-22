/*
 * This is part of the FL library, a C++ Bayesian filtering library
 * (https://github.com/filtering-library)
 *
 * Copyright (c) 2014 Jan Issac (jan.issac@gmail.com)
 * Copyright (c) 2014 Manuel Wuthrich (manuel.wuthrich@gmail.com)
 *
 * Max-Planck Institute for Intelligent Systems, AMD Lab
 * University of Southern California, CLMC Lab
 *
 * This Source Code Form is subject to the terms of the MIT License (MIT).
 * A copy of the license can be found in the LICENSE file distributed with this
 * source code.
 */

/**
 * \file test_typing.cpp
 * \date June 2015
 * \author Jan Issac (jan.issac@gmail.com)
 */

#include <gtest/gtest.h>

#include <Eigen/Dense>
#include <fl/util/meta.hpp>

#ifndef FL__TEST__TYPECAST_HPP
#define FL__TEST__TYPECAST_HPP

namespace fl
{

struct NoParameter { };

template <typename Param = NoParameter>
struct DynamicTest { typedef Param Parameter; };

template <typename Param = NoParameter>
struct StaticTest  { typedef Param Parameter; };

template <int, typename> struct TestSize;

template <int Size, typename Param>
struct TestSize<Size, StaticTest<Param>>
{
    enum : signed int { Value = Size };
};

template <int Size, typename Param>
struct TestSize<Size, DynamicTest<Param>>
{
    enum : signed int { Value = Eigen::Dynamic };
};

template <int Key_, typename TypeName>
struct IntegerTypePair
{
    enum : signed int { Key = Key_ };
    typedef TypeName Type;
};

template <int Key, typename CurrentPair, typename ... Pairs>
struct IntegerTypeMapImp
    : IntegerTypeMapImp<Key, Pairs...>
{ };

template <int Key, typename TypeName, typename ... Pairs>
struct IntegerTypeMapImp<Key, IntegerTypePair<Key, TypeName>, Pairs...>
{
    typedef TypeName Type;
};

template <typename ... Pairs>
struct IntegerTypeMap
{
    template <int Key> struct Select: IntegerTypeMapImp<Key, Pairs...> { };
};

}
#endif

