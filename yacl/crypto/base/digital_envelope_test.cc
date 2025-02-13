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

#include "yacl/crypto/base/digital_envelope.h"

#include "gtest/gtest.h"

#include "yacl/crypto/base/asymmetric_util.h"

namespace yacl::crypto {

TEST(SmDigitalEnvelope, SealOpen_shouldOk) {
  // GIVEN
  auto [public_key, private_key] = CreateSm2KeyPair();
  std::string plaintext = "I am a plaintext.";
  std::string iv = "1234567812345678";

  // WHEN
  std::vector<uint8_t> ciphertext;
  std::vector<uint8_t> encrypted_key;
  SmEnvSeal(public_key, iv, plaintext, &encrypted_key, &ciphertext);

  std::vector<uint8_t> decrypted;
  SmEnvOpen(private_key, iv, encrypted_key, ciphertext, &decrypted);

  // THEN
  EXPECT_EQ(plaintext, std::string(decrypted.begin(), decrypted.end()));
}

TEST(RsaDigitalEnvelope, SealOpen_shouldOk) {
  // GIVEN
  auto [public_key, private_key] = CreateRsaKeyPair();
  std::string plaintext = "I am a plaintext.";
  std::string iv = "123456781234";

  // WHEN
  std::vector<uint8_t> ciphertext;
  std::vector<uint8_t> encrypted_key;
  std::vector<uint8_t> mac;
  RsaEnvSeal(public_key, iv, plaintext, &encrypted_key, &ciphertext, &mac);

  std::vector<uint8_t> decrypted;
  RsaEnvOpen(private_key, iv, encrypted_key, ciphertext, mac, &decrypted);

  // THEN
  EXPECT_EQ(plaintext, std::string(decrypted.begin(), decrypted.end()));
}

}  // namespace yacl::crypto