// Copyright 2019 Ant Group Co., Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "yacl/base/dynamic_bitset.h"

#include <cstdint>

#include "gtest/gtest.h"

#include "yacl/base/exception.h"
#include "yacl/crypto/tools/prg.h"
#include "yacl/crypto/utils/rand.h"

namespace yacl {

namespace {
constexpr uint128_t kBlockNum = 3;
}  // namespace

template <typename T>
class DynamicBitsetTest : public testing::Test {
 public:
  dynamic_bitset<T> GetRandInstance(uint128_t seed, size_t bitlen) {
    dynamic_bitset<T> out;
    crypto::Prg<bool> prg(seed);
    for (size_t i = 0; i < bitlen; i++) {
      out.push_back(prg());
    }
    YACL_ENFORCE_EQ(out.size(), bitlen);
    return out;
  }
};

using TestTypes = ::testing::Types<uint16_t, uint32_t, uint64_t, uint128_t>;

TYPED_TEST_SUITE(DynamicBitsetTest, TestTypes);

TYPED_TEST(DynamicBitsetTest, ConstructorTest) {
  // nbits=1
  auto bitset1 = dynamic_bitset<TypeParam>(1);
  EXPECT_EQ(bitset1.capacity(), sizeof(TypeParam) * 8);

  // nbits=10
  // init value=0
  auto bitset2 = dynamic_bitset<TypeParam>(10, 0);
  EXPECT_EQ(bitset2.capacity(), sizeof(TypeParam) * 8);
  EXPECT_EQ(bitset2.size(), 10);
  for (size_t i = 0; i < bitset2.size(); i++) {
    EXPECT_EQ(bitset2[i], 0);
  }

  // init list = {T(1), T(2), T(3)}
  auto bitset3 = dynamic_bitset<TypeParam>({1, 2, 3});
  EXPECT_EQ(bitset3.size(), sizeof(TypeParam) * 8 * 3);
  EXPECT_EQ(*bitset3.data(), 1);
  EXPECT_EQ(*(bitset3.data() + 1), 2);
  EXPECT_EQ(*(bitset3.data() + 2), 3);

  // init dynamic bitset from a string, with "0"=>0, "1"=>1
  const char str1[] = "010";
  auto bitset4 = dynamic_bitset<TypeParam>(str1);
  EXPECT_EQ(bitset4.size(), 3);
  EXPECT_EQ(bitset4[0], 0);
  EXPECT_EQ(bitset4[1], 1);
  EXPECT_EQ(bitset4[2], 0);

  // init dynamic bitset from partial string, with "0"=>0, "1"=>1
  const char str2[] = "01101001";
  auto bitset5 = dynamic_bitset<TypeParam>(str2, 3, 3);
  EXPECT_EQ(bitset5.size(), 3);
  EXPECT_EQ(bitset5[0], 0);
  EXPECT_EQ(bitset5[1], 1);
  EXPECT_EQ(bitset5[2], 0);
}

TYPED_TEST(DynamicBitsetTest, XorTest) {
  auto r1 = crypto::RandVec<TypeParam>(kBlockNum);
  auto r2 = crypto::RandVec<TypeParam>(kBlockNum);

  dynamic_bitset<TypeParam> x;
  dynamic_bitset<TypeParam> y;
  dynamic_bitset<TypeParam> check;
  for (size_t i = 0; i < kBlockNum; i++) {
    x.append(r1[i]);
    y.append(r2[i]);
    check.append(r1[i] ^ r2[i]);
  }

  EXPECT_EQ(x ^ y, check);
}

TYPED_TEST(DynamicBitsetTest, AndTest) {
  auto r1 = crypto::RandVec<TypeParam>(kBlockNum);
  auto r2 = crypto::RandVec<TypeParam>(kBlockNum);

  dynamic_bitset<TypeParam> x;
  dynamic_bitset<TypeParam> y;
  dynamic_bitset<TypeParam> check;
  for (size_t i = 0; i < kBlockNum; i++) {
    x.append(r1[i]);
    y.append(r2[i]);
    check.append(r1[i] & r2[i]);
  }

  EXPECT_EQ(x & y, check);
}

TYPED_TEST(DynamicBitsetTest, OrTest) {
  auto r1 = crypto::RandVec<TypeParam>(kBlockNum);
  auto r2 = crypto::RandVec<TypeParam>(kBlockNum);

  dynamic_bitset<TypeParam> x;
  dynamic_bitset<TypeParam> y;
  dynamic_bitset<TypeParam> check;
  for (size_t i = 0; i < kBlockNum; i++) {
    x.append(r1[i]);
    y.append(r2[i]);
    check.append(r1[i] | r2[i]);
  }

  EXPECT_EQ(x | y, check);
}

TYPED_TEST(DynamicBitsetTest, NegTest) {
  auto r = crypto::RandVec<TypeParam>(kBlockNum);

  dynamic_bitset<TypeParam> x;
  dynamic_bitset<TypeParam> check;
  for (size_t i = 0; i < kBlockNum; i++) {
    x.append(r[i]);
    check.append(~r[i]);
  }

  EXPECT_EQ(~x, check);
}

TYPED_TEST(DynamicBitsetTest, LshiftTest) {
  auto tmp = dynamic_bitset<TypeParam>("010");
  tmp.resize(5);  // dynamic_bitset does not auto-resize
  tmp <<= 2;
  auto result = dynamic_bitset<TypeParam>("01000");
  EXPECT_EQ(tmp, result);
}

TYPED_TEST(DynamicBitsetTest, RshiftTest) {
  auto tmp = dynamic_bitset<TypeParam>("010");
  tmp.resize(5);  // dynamic_bitset does not auto-resize
  tmp >>= 2;
  auto result = dynamic_bitset<TypeParam>("00000");
  EXPECT_EQ(tmp, result);
}

TYPED_TEST(DynamicBitsetTest, IterateBitTest) {
  dynamic_bitset<TypeParam> bitset(3, true);
  EXPECT_EQ(bitset.size(), 3);

  size_t current_check_bit = 0;
  bitset.iterate_bits_on([&](size_t bit_pos) {
    while (current_check_bit < bit_pos) {
      EXPECT_EQ(bitset[current_check_bit], false);
      ++current_check_bit;
    }
    EXPECT_EQ(bitset[bit_pos], true);
    ++current_check_bit;
  });
}
}  // namespace yacl
