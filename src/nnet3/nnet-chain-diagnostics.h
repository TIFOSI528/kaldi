// nnet3/nnet-chain-diagnostics.h

// Copyright    2015  Johns Hopkins University (author: Daniel Povey)

// See ../../COPYING for clarification regarding multiple authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED
// WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE,
// MERCHANTABLITY OR NON-INFRINGEMENT.
// See the Apache 2 License for the specific language governing permissions and
// limitations under the License.

#ifndef KALDI_NNET3_NNET_CHAIN_DIAGNOSTICS_H_
#define KALDI_NNET3_NNET_CHAIN_DIAGNOSTICS_H_

#include "nnet3/nnet-example.h"
#include "nnet3/nnet-computation.h"
#include "nnet3/nnet-compute.h"
#include "nnet3/nnet-optimize.h"
#include "nnet3/nnet-chain-example.h"
#include "nnet3/nnet-diagnostics.h"
#include "chain/chain-training.h"
#include "chain/chain-den-graph.h"

namespace kaldi {
namespace nnet3 {



/** This class is for computing objective-function values in a nnet3+ctc setup,
    for diagnostics.  It also supports computing model derivatives.
 */
class NnetChainComputeProb {
 public:
  // does not store a reference to 'config' but does store one to 'nnet'.
  NnetChainComputeProb(const NnetComputeProbOptions &nnet_config,
                       const chain::ChainTrainingOptions &chain_config,
                       const fst::StdVectorFst &den_fst,
                       const Nnet &nnet);

  // Reset the likelihood stats, and the derivative stats (if computed).
  void Reset();

  // compute objective on one minibatch.
  void Compute(const NnetChainExample &chain_eg);

  // Prints out the final stats, and return true if there was a nonzero count.
  bool PrintTotalStats() const;

  // returns the objective-function info for this output name (e.g. "output"),
  // or NULL if there is no such info.
  const SimpleObjectiveInfo *GetObjective(const std::string &output_name) const;

  // if config.compute_deriv == true, returns a reference to the
  // computed derivative.  Otherwise crashes.
  const Nnet &GetDeriv() const;

  ~NnetChainComputeProb();
 private:
  void ProcessOutputs(const NnetChainExample &chain_eg,
                      NnetComputer *computer);

  NnetComputeProbOptions nnet_config_;
  chain::ChainTrainingOptions chain_config_;
  chain::DenominatorGraph den_graph_;
  const Nnet &nnet_;
  CachingOptimizingCompiler compiler_;
  Nnet *deriv_nnet_;
  int32 num_minibatches_processed_;  // this is only for diagnostics

  unordered_map<std::string, SimpleObjectiveInfo, StringHasher> objf_info_;

};




} // namespace nnet3
} // namespace kaldi

#endif // KALDI_NNET3_NNET_CHAIN_DIAGNOSTICS_H_